import 'dart:math';
import 'package:shared_preferences/shared_preferences.dart';

class CognitiveScorer {
  static const String _avgKeyPrefix = 'avg_gsr_';
  static const String _countKeyPrefix = 'count_gsr_';
  
  // Calculate cognitive score based on GSR data
  static double calculateScore(List<double> gsrValues) {
    if (gsrValues.isEmpty) return 0.0;
    
    // Calculate metrics
    double variability = _calculateVariability(gsrValues);
    double trend = _calculateTrend(gsrValues);
    double stability = _calculateStability(gsrValues);
    
    // Weighted scoring (adjust weights based on importance)
    double score = (variability * 0.4) + (trend * 0.3) + (stability * 0.3);
    
    // Normalize to 0-100 range
    return score.clamp(0.0, 100.0);
  }
  
  // Calculate GSR variability (normalized)
  static double _calculateVariability(List<double> values) {
    if (values.length < 2) return 0.0;
    
    double sum = 0.0;
    for (int i = 1; i < values.length; i++) {
      sum += (values[i] - values[i-1]).abs();
    }
    
    double avgVariability = sum / (values.length - 1);
    // Normalize to 0-100 range (adjust these values based on your GSR range)
    return (avgVariability / 10.0) * 100;
  }
  
  // Calculate trend (increasing/decreasing patterns)
  static double _calculateTrend(List<double> values) {
    if (values.length < 5) return 0.0;
    
    int increasingCount = 0;
    int totalPatterns = values.length - 1;
    
    for (int i = 1; i < values.length; i++) {
      if (values[i] > values[i-1]) increasingCount++;
    }
    
    // Convert to percentage and normalize around 50%
    double trendScore = (increasingCount / totalPatterns) * 100;
    return 100 - ((trendScore - 50).abs());
  }
  
  // Calculate stability
  static double _calculateStability(List<double> values) {
    if (values.isEmpty) return 0.0;
    
    double mean = values.reduce((a, b) => a + b) / values.length;
    double variance = values.map((v) => pow(v - mean, 2)).reduce((a, b) => a + b) / values.length;
    double stdDev = sqrt(variance);
    
    // Normalize to 0-100 range (inverse - lower deviation is better)
    return 100 - ((stdDev / mean) * 100).clamp(0.0, 100.0);
  }
  
  // Get interpretation of the score
  static String interpretScore(double score) {
    if (score >= 90) return "Excellent cognitive state";
    if (score >= 80) return "Very good cognitive state";
    if (score >= 70) return "Good cognitive state";
    if (score >= 60) return "Average cognitive state";
    if (score >= 50) return "Below average cognitive state";
    if (score >= 40) return "Poor cognitive state";
    return "Insufficient data";
  }
  
  // Store and retrieve personal averages
  static Future<void> updatePersonalAverage(String userId, double score) async {
    final prefs = await SharedPreferences.getInstance();
    
    double currentAvg = prefs.getDouble(_avgKeyPrefix + userId) ?? score;
    int count = prefs.getInt(_countKeyPrefix + userId) ?? 0;
    
    // Calculate new average
    double newAvg = ((currentAvg * count) + score) / (count + 1);
    
    await prefs.setDouble(_avgKeyPrefix + userId, newAvg);
    await prefs.setInt(_countKeyPrefix + userId, count + 1);
  }
  
  static Future<double> getPersonalAverage(String userId) async {
    final prefs = await SharedPreferences.getInstance();
    return prefs.getDouble(_avgKeyPrefix + userId) ?? 0.0;
  }
}
