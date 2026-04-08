import 'package:shared_preferences/shared_preferences.dart';

class GsrAnalyzer {
  static const int LONG_WINDOW = 300; // 5 minutes (assuming readings per second)
  static const int SHORT_WINDOW = 30;  // 30 seconds for recent changes
  static const String _baselineKey = 'gsr_baseline';

  // Long-term moving average (baseline)
  double calculateBaseline(List<double> values) {
    if (values.isEmpty) return 0.0;
    int windowSize = values.length < LONG_WINDOW ? values.length : LONG_WINDOW;
    var baseline = values.sublist(values.length - windowSize)
        .reduce((a, b) => a + b) / windowSize;
    return baseline;
  }

  // Recent activity average
  double calculateRecentActivity(List<double> values) {
    if (values.isEmpty) return 0.0;
    int windowSize = values.length < SHORT_WINDOW ? values.length : SHORT_WINDOW;
    var recent = values.sublist(values.length - windowSize)
        .reduce((a, b) => a + b) / windowSize;
    return recent;
  }

  // Calculate deviation from baseline (arousal level)
  double calculateArousalLevel(double baseline, double recent) {
    if (baseline == 0.0) return 0.0;
    return ((recent - baseline).abs() / baseline) * 100;
  }

  String interpretArousalLevel(double arousalLevel) {
    // Higher GSR values typically indicate relaxation/fatigue
    if (arousalLevel > 0.45) return "Very Tired";
    if (arousalLevel > 0.40) return "Tired";
    if (arousalLevel > 0.35) return "Somewhat Tired";
    if (arousalLevel > 0.30) return "Normal";
    return "Alert";
  }

  bool isFatigued(double baselineGSR, double variability) {
    // Now working with normalized values (0-1)
    // Low baseline (< 0.3) and low variability (< 0.03) indicate fatigue
    return baselineGSR < 0.3 && variability < 0.03;
  }

  String getArousalDescription(double arousalLevel) {
    // Higher values indicate more relaxation/tiredness
    if (arousalLevel > 0.7) return "Very relaxed - likely fatigued";
    if (arousalLevel > 0.6) return "Relaxed and calm - possibly tired";
    if (arousalLevel > 0.5) return "Moderately relaxed";
    if (arousalLevel > 0.4) return "Normal arousal state";
    return "Heightened arousal - alert or stressed";
  }

  // Store baseline for future reference
  Future<void> storeBaseline(double baseline) async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setDouble(_baselineKey, baseline);
  }

  // Retrieve stored baseline
  Future<double> getStoredBaseline() async {
    final prefs = await SharedPreferences.getInstance();
    return prefs.getDouble(_baselineKey) ?? 0.0;
  }
}
