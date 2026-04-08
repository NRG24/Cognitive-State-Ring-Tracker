import 'dart:async';
import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'package:flutter_reactive_ble/flutter_reactive_ble.dart';
import 'package:permission_handler/permission_handler.dart';
import 'gsr_analyzer.dart';
import 'cognitive_scorer.dart';

void main() async {
  runZonedGuarded(() async {
    WidgetsFlutterBinding.ensureInitialized();
    
    try {
      print('Requesting permissions...');
      await Permission.bluetooth.request();
      await Permission.bluetoothScan.request();
      await Permission.bluetoothConnect.request();
      await Permission.location.request();
      
      print('Starting app...');
      runApp(const GsrStreamerApp());
    } catch (e) {
      print('Error during initialization: $e');
    }
  }, (error, stack) {
    print('Uncaught error: $error');
  });
}

class GsrStreamerApp extends StatelessWidget {
  const GsrStreamerApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Mental State',
      theme: ThemeData(
        primarySwatch: Colors.blue,
        scaffoldBackgroundColor: Colors.white,
        appBarTheme: AppBarTheme(
          backgroundColor: Colors.grey[50],
          elevation: 0,
          centerTitle: true,
          foregroundColor: Colors.black,
          titleTextStyle: const TextStyle(
            color: Colors.black,
            fontSize: 17,
            fontWeight: FontWeight.w600,
          ),
        ),
        tabBarTheme: TabBarThemeData(
          labelColor: Colors.blue,
          unselectedLabelColor: Colors.grey[600],
          indicatorSize: TabBarIndicatorSize.label,
        ),
      ),
      home: const GsrChartPage(),
    );
  }
}

class GsrChartPage extends StatefulWidget {
  const GsrChartPage({super.key});

  @override
  State<GsrChartPage> createState() => _GsrChartPageState();
}

class _GsrChartPageState extends State<GsrChartPage> with SingleTickerProviderStateMixin {
  final List<FlSpot> _spots = [];
  final FlutterReactiveBle _ble = FlutterReactiveBle();
  final GsrAnalyzer _analyzer = GsrAnalyzer();
  late TabController _tabController;
  StreamSubscription? _scanSubscription;
  StreamSubscription<ConnectionStateUpdate>? _connectionSubscription;
  StreamSubscription<List<int>>? _characteristicSubscription;
  StreamSubscription<List<int>>? _heartCharacteristicSubscription;

  // GSR variables
  double _arousalLevel = 0.0;
  double _baseline = 0.0;
  double _recentActivity = 0.0;
  double _cognitiveScore = 0.0;
  double _personalAverage = 0.0;
  final String _userId = 'default_user';

  // Heart rate variables
  double _heartRate = 0.0;
  int _heartRateAvg = 0;
  double _spo2 = 0.0;
  double _hrv = 0.0;
  List<FlSpot> _heartRateSpots = [];
  List<FlSpot> _hrvSpots = [];
  List<FlSpot> _spo2Spots = [];
  int _heartDataIndex = 0;

  static const String serviceUuid = '19B10000-E8F2-537E-4F6C-D104768A1214';
  static const String gsrCharacteristicUuid = '19B10002-E8F2-537E-4F6C-D104768A1214';
  static const String heartCharacteristicUuid = '19B10003-E8F2-537E-4F6C-D104768A1214';
  
  int _dataIndex = 0;
  String _status = 'Scanning for device...';
  bool _isSessionActive = false;
  DateTime? _sessionStartTime;

  @override
  void initState() {
    super.initState();
    _tabController = TabController(length: 4, vsync: this);
    _startScan();
  }

  @override
  void dispose() {
    _tabController.dispose();
    _scanSubscription?.cancel();
    _connectionSubscription?.cancel();
    _characteristicSubscription?.cancel();
    _heartCharacteristicSubscription?.cancel();
    super.dispose();
  }

  void _startScan() {
    _scanSubscription?.cancel();
    setState(() => _status = 'Starting BLE scan...');
    
    final discoveredDevices = <String, DiscoveredDevice>{};
    
    _scanSubscription = _ble.scanForDevices(
      withServices: [],
      scanMode: ScanMode.lowLatency,
    ).listen((device) async {
      print('Device found: ${device.name} (${device.id})');
      
      if (device.name.isNotEmpty) {
        discoveredDevices[device.id] = device;
      }
      
      final devicesInfo = discoveredDevices.values
          .map((d) => '• ${d.name} (${d.id})')
          .join('\n');
      
      setState(() => _status = 'Scanning...\n\nDevices:\n$devicesInfo');
      
      if (device.name.toUpperCase().contains('GSR_HEART') || 
          device.serviceUuids.contains(Uuid.parse(serviceUuid))) {
        await _scanSubscription?.cancel();
        _connectToDevice(device.id);
      }
    }, onError: (error) {
      setState(() => _status = 'Error scanning: $error');
    });
  }

  void _connectToDevice(String deviceId) {
    setState(() => _status = 'Connecting to device...');
    
    _connectionSubscription = _ble.connectToDevice(
      id: deviceId,
      connectionTimeout: const Duration(seconds: 10),
    ).listen((connectionState) {
      setState(() => _status = 'Connection state: ${connectionState.connectionState}');
      
      if (connectionState.connectionState == DeviceConnectionState.connected) {
        _startListening(deviceId);
      }
    }, onError: (error) {
      setState(() => _status = 'Error connecting: $error');
    });
  }

  void _startListening(String deviceId) {
    _ble.discoverServices(deviceId).then((services) {
      final targetService = services.firstWhere(
        (service) => service.serviceId.toString().toLowerCase() == serviceUuid.toLowerCase(),
        orElse: () => throw Exception('Service not found'),
      );
      
      // Get both GSR and heart rate characteristics
      final gsrCharacteristic = targetService.characteristicIds.firstWhere(
        (characteristic) => characteristic.toString().toLowerCase() == gsrCharacteristicUuid.toLowerCase(),
        orElse: () => throw Exception('GSR Characteristic not found'),
      );

      final heartCharacteristic = targetService.characteristicIds.firstWhere(
        (characteristic) => characteristic.toString().toLowerCase() == heartCharacteristicUuid.toLowerCase(),
        orElse: () => throw Exception('Heart Characteristic not found'),
      );
      
      setState(() => _status = 'Connected and ready for data');

      // Subscribe to GSR data
      final qualifiedGsrCharacteristic = QualifiedCharacteristic(
        serviceId: targetService.serviceId,
        characteristicId: gsrCharacteristic,
        deviceId: deviceId,
      );

      // Subscribe to heart rate data
      final qualifiedHeartCharacteristic = QualifiedCharacteristic(
        serviceId: targetService.serviceId,
        characteristicId: heartCharacteristic,
        deviceId: deviceId,
      );

      _characteristicSubscription = _ble.subscribeToCharacteristic(qualifiedGsrCharacteristic).listen(
        (data) => _handleGsrData(data),
        onError: (error) => setState(() => _status = 'Error reading GSR data: $error'),
      );

      _heartCharacteristicSubscription = _ble.subscribeToCharacteristic(qualifiedHeartCharacteristic).listen(
        (data) => _handleHeartData(data),
        onError: (error) => setState(() => _status = 'Error reading heart data: $error'),
      );
    }).catchError((error) {
      setState(() => _status = 'Error discovering services: $error');
    });
  }

  void _handleGsrData(List<int> data) {
    if (data.isEmpty) return;
    try {
      String stringValue = String.fromCharCodes(data);
      print('Received GSR data: $stringValue');
      
      final regex = RegExp(r'{"g":([\d.]+),"b":([\d.]+),"v":([\d.]+)}');
      final match = regex.firstMatch(stringValue);
      
      if (match != null) {
        final gsrValue = double.parse(match.group(1)!);
        final baseline = double.parse(match.group(2)!);
        final variability = double.parse(match.group(3)!);
        
        setState(() {
          if (_isSessionActive) {
            _spots.add(FlSpot(_dataIndex.toDouble(), gsrValue * 1023.0));
            _dataIndex++;
            
            if (_spots.length > 300) {
              _spots.removeAt(0);
            }

            List<double> values = _spots.map((spot) => spot.y).toList();
            _baseline = baseline;
            _arousalLevel = gsrValue;
            _recentActivity = _analyzer.calculateRecentActivity(values);
            _cognitiveScore = CognitiveScorer.calculateScore(values);
            
            CognitiveScorer.updatePersonalAverage(_userId, _cognitiveScore).then((_) {
              CognitiveScorer.getPersonalAverage(_userId).then((avg) {
                setState(() => _personalAverage = avg);
              });
            });
          }
        });
      }
    } catch (e) {
      print('Error parsing GSR data: $e');
    }
  }

  void _handleHeartData(List<int> data) {
    if (data.isEmpty) return;
    try {
      String stringValue = String.fromCharCodes(data);
      print('Received heart data: $stringValue');
      
      final regex = RegExp(r'{"bpm":([\d.]+),"avg":([\d]+),"spo2":([\d.]+),"hrv":([\d.]+)}');
      final match = regex.firstMatch(stringValue);
      
      if (match != null && _isSessionActive) {
        setState(() {
          _heartRate = double.parse(match.group(1)!);
          _heartRateAvg = int.parse(match.group(2)!);
          _spo2 = double.parse(match.group(3)!);
          _hrv = double.parse(match.group(4)!);

          // Add data points to graphs
          _heartRateSpots.add(FlSpot(_heartDataIndex.toDouble(), _heartRate));
          _hrvSpots.add(FlSpot(_heartDataIndex.toDouble(), _hrv));
          _spo2Spots.add(FlSpot(_heartDataIndex.toDouble(), _spo2));
          _heartDataIndex++;

          // Keep last 300 points (5 minutes at 1 sample per second)
          if (_heartRateSpots.length > 300) {
            _heartRateSpots.removeAt(0);
            _hrvSpots.removeAt(0);
            _spo2Spots.removeAt(0);
          }
        });
      }
    } catch (e) {
      print('Error parsing heart data: $e');
    }
  }

  void _toggleSession() {
    setState(() {
      _isSessionActive = !_isSessionActive;
      if (_isSessionActive) {
        _sessionStartTime = DateTime.now();
        _spots.clear();
        _heartRateSpots.clear();
        _hrvSpots.clear();
        _spo2Spots.clear();
        _dataIndex = 0;
        _heartDataIndex = 0;
      }
    });
  }

  String _getSessionDuration() {
    if (!_isSessionActive || _sessionStartTime == null) return '00:00';
    final duration = DateTime.now().difference(_sessionStartTime!);
    return '${duration.inMinutes.toString().padLeft(2, '0')}:${(duration.inSeconds % 60).toString().padLeft(2, '0')}';
  }

  Widget _buildOverviewTab() {
    return SingleChildScrollView(
      padding: const EdgeInsets.all(16.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          if (_status != 'Connected and ready for data')
            _buildStatusCard(),
          const SizedBox(height: 16),
          _buildSessionCard(),
          const SizedBox(height: 16),
          _buildMentalStateCard(),
          const SizedBox(height: 16),
          _buildArousalCard(),
        ],
      ),
    );
  }

  Widget _buildDetailsTab() {
    return SingleChildScrollView(
      padding: const EdgeInsets.all(16.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          if (_status != 'Connected and ready for data')
            _buildStatusCard(),
          const SizedBox(height: 16),
          _buildSessionCard(),
          const SizedBox(height: 16),
          _buildFocusGraphCard(),
        ],
      ),
    );
  }

  Widget _buildInsightsTab() {
    return SingleChildScrollView(
      padding: const EdgeInsets.all(16.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          _buildInsightCard(
            'Current Session',
            'Based on your data, your mental state is ${_cognitiveScore >= 65 ? 'optimal' : 'below optimal'} with ${_cognitiveScore.toStringAsFixed(1)}% engagement.',
            Icons.timeline,
          ),
          const SizedBox(height: 16),
          _buildInsightCard(
            'Mental State Analysis',
            _analyzer.getArousalDescription(_arousalLevel),
            Icons.psychology,
          ),
          const SizedBox(height: 16),
          _buildInsightCard(
            'Historical Performance',
            'Your personal best is ${_personalAverage.toStringAsFixed(1)}%. Keep working on maintaining consistent engagement levels.',
            Icons.trending_up,
          ),
        ],
      ),
    );
  }

  Widget _buildBiometricsTab() {
    return SingleChildScrollView(
      padding: const EdgeInsets.all(16.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          if (_status != 'Connected and ready for data')
            _buildStatusCard(),
          const SizedBox(height: 16),
          _buildSessionCard(),
          const SizedBox(height: 16),
          _buildHeartMetricsCard(),
          const SizedBox(height: 16),
          _buildHeartRateGraph(),
          const SizedBox(height: 16),
          _buildHrvGraph(),
          const SizedBox(height: 16),
          _buildSpO2Graph(),
        ],
      ),
    );
  }

  Widget _buildHeartMetricsCard() {
    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: Colors.white,
        borderRadius: BorderRadius.circular(12),
        boxShadow: [
          BoxShadow(
            color: Colors.black.withOpacity(0.05),
            blurRadius: 10,
            offset: const Offset(0, 2),
          ),
        ],
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          const Text(
            'Heart Rate Metrics',
            style: TextStyle(
              fontSize: 16,
              fontWeight: FontWeight.w500,
            ),
          ),
          const SizedBox(height: 16),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceAround,
            children: [
              _buildHeartMetric(
                'BPM',
                _heartRate.toStringAsFixed(1),
                Icons.favorite,
                Colors.red,
              ),
              _buildHeartMetric(
                'SpO2',
                '${_spo2.toStringAsFixed(1)}%',
                Icons.water_drop,
                Colors.blue,
              ),
              _buildHeartMetric(
                'HRV',
                _hrv.toStringAsFixed(3),
                Icons.timeline,
                Colors.purple,
              ),
            ],
          ),
          const SizedBox(height: 12),
          Text(
            'Average Heart Rate: $_heartRateAvg BPM',
            style: const TextStyle(
              fontSize: 14,
              color: Colors.black54,
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildHeartMetric(String label, String value, IconData icon, Color color) {
    return Column(
      children: [
        Icon(icon, color: color, size: 24),
        const SizedBox(height: 8),
        Text(
          value,
          style: const TextStyle(
            fontSize: 20,
            fontWeight: FontWeight.bold,
          ),
        ),
        Text(
          label,
          style: const TextStyle(
            fontSize: 12,
            color: Colors.black54,
          ),
        ),
      ],
    );
  }

  Widget _buildHeartRateGraph() {
    return _buildGraphCard(
      'Heart Rate',
      _heartRateSpots,
      Colors.red,
      'BPM',
      40,
      200,
    );
  }

  Widget _buildHrvGraph() {
    return _buildGraphCard(
      'Heart Rate Variability',
      _hrvSpots,
      Colors.purple,
      'ms',
      0,
      100,
    );
  }

  Widget _buildSpO2Graph() {
    return _buildGraphCard(
      'Blood Oxygen Saturation',
      _spo2Spots,
      Colors.blue,
      '%',
      90,
      100,
    );
  }

  Widget _buildGraphCard(
    String title,
    List<FlSpot> spots,
    Color color,
    String unit,
    double minY,
    double maxY,
  ) {
    return Container(
      height: 250,
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: Colors.white,
        borderRadius: BorderRadius.circular(12),
        boxShadow: [
          BoxShadow(
            color: Colors.black.withOpacity(0.05),
            blurRadius: 10,
            offset: const Offset(0, 2),
          ),
        ],
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            title,
            style: const TextStyle(
              fontSize: 16,
              fontWeight: FontWeight.w500,
            ),
          ),
          const SizedBox(height: 16),
          Expanded(
            child: spots.isEmpty
                ? Center(
                    child: Text(
                      _isSessionActive ? 'Waiting for data...' : 'Start a session to see data',
                      style: TextStyle(color: Colors.grey[600]),
                    ),
                  )
                : LineChart(
                    LineChartData(
                      lineBarsData: [
                        LineChartBarData(
                          spots: spots,
                          isCurved: true,
                          dotData: FlDotData(show: false),
                          color: color,
                          barWidth: 2,
                        ),
                      ],
                      minY: minY,
                      maxY: maxY,
                      titlesData: FlTitlesData(
                        rightTitles: AxisTitles(
                          sideTitles: SideTitles(showTitles: false),
                        ),
                        topTitles: AxisTitles(
                          sideTitles: SideTitles(showTitles: false),
                        ),
                        bottomTitles: AxisTitles(
                          sideTitles: SideTitles(showTitles: false),
                        ),
                      ),
                      gridData: FlGridData(show: true),
                      borderData: FlBorderData(show: true),
                    ),
                  ),
          ),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Row(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Icon(Icons.monitor_heart_outlined, size: 24),
            SizedBox(width: 8),
            Text("Mental State Monitor"),
          ],
        ),
        bottom: TabBar(
          controller: _tabController,
          tabs: const [
            Tab(icon: Icon(Icons.dashboard_outlined), text: "Overview"),
            Tab(icon: Icon(Icons.show_chart), text: "Details"),
            Tab(icon: Icon(Icons.insights_outlined), text: "Insights"),
            Tab(icon: Icon(Icons.favorite_outline), text: "Biometrics"),
          ],
        ),
      ),
      body: TabBarView(
        controller: _tabController,
        children: [
          _buildOverviewTab(),
          _buildDetailsTab(),
          _buildInsightsTab(),
          _buildBiometricsTab(),
        ],
      ),
      floatingActionButton: _status == 'Connected and ready for data'
          ? null
          : FloatingActionButton(
              onPressed: _startScan,
              backgroundColor: Colors.blue,
              child: const Icon(Icons.refresh),
            ),
    );
  }
}
