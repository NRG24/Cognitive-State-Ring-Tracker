import 'package:flutter_reactive_ble/flutter_reactive_ble.dart';

class BleService {
  final FlutterReactiveBle _ble = FlutterReactiveBle();

  final Uuid serviceUuid = Uuid.parse("19B10000-E8F2-537E-4F6C-D104768A1214");
  final Uuid charUuid = Uuid.parse("19B10001-E8F2-537E-4F6C-D104768A1214");

  Stream<List<int>> subscribeToGsr(String deviceId) {
    final characteristic = QualifiedCharacteristic(
      serviceId: serviceUuid,
      characteristicId: charUuid,
      deviceId: deviceId,
    );

    return _ble.subscribeToCharacteristic(characteristic);
  }

  Stream<DiscoveredDevice> scanForDevice() {
    return _ble.scanForDevices(withServices: [serviceUuid]);
  }
}