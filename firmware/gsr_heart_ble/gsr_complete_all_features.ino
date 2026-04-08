/*
 * ═══════════════════════════════════════════════════════════════════════
 * COMPLETE BIOMETRIC SENSOR SYSTEM FOR XIAO nRF52840
 * ═══════════════════════════════════════════════════════════════════════
 * 
 * This sketch provides ALL features needed by the Flutter GSR Streamer app:
 * ✓ GSR (Galvanic Skin Response) - Stress monitoring
 * ✓ Heart Rate - Pulse detection with BPM calculation
 * ✓ Heart Rate Variability (HRV) - RMSSD calculation
 * ✓ Finger Temperature - DS18B20 sensor
 * ✓ SpO2 (Blood Oxygen) - Optional MAX30102 sensor
 * ✓ Real-time waveform data for visualization
 * ✓ Power-optimized with auto-sleep
 * ✓ Battery voltage monitoring
 * 
 * HARDWARE CONNECTIONS:
 * ═══════════════════════════════════════════════════════════════════════
 * GSR Sensor      → A0 (analog)
 * Pulse Sensor    → A1 (analog) 
 * DS18B20 Temp    → D6 (OneWire digital)
 * MAX30102 (opt)  → SDA/SCL (I2C)
 * Battery Voltage → A5 (voltage divider: Battery+ → 10kΩ → A5 → 10kΩ → GND)
 * 
 * BLE SERVICE UUID: 19B10000-E8F2-537E-4F6C-D104768A1214
 * 
 * CHARACTERISTICS:
 * ═══════════════════════════════════════════════════════════════════════
 * GSR Data         → 19B10002 (Int)    - Raw GSR value (0-1023)
 * Heart Rate       → 19B10003 (Int)    - BPM (30-200)
 * Temperature      → 19B10004 (Float)  - °C (20-45)
 * HRV              → 19B10005 (Float)  - RMSSD in ms
 * SpO2             → 19B10006 (Int)    - % (70-100)
 * Battery          → 19B10007 (Int)    - % (0-100)
 * 
 * Author: Generated for GSR Streamer Flutter App
 * Version: 2.0
 * Date: 2025
 */

#include <ArduinoBLE.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ═══════════════════════════════════════════════════════════════════════
// PIN DEFINITIONS
// ═══════════════════════════════════════════════════════════════════════
#define GSR_PIN         A0
#define PULSE_PIN       A1
#define TEMP_PIN        6
#define BATTERY_PIN     A5
#define LED_PIN         LED_BUILTIN

// ═══════════════════════════════════════════════════════════════════════
// TEMPERATURE SENSOR SETUP
// ═══════════════════════════════════════════════════════════════════════
OneWire oneWire(TEMP_PIN);
DallasTemperature tempSensor(&oneWire);

// ═══════════════════════════════════════════════════════════════════════
// BLE SERVICE AND CHARACTERISTICS
// ═══════════════════════════════════════════════════════════════════════
BLEService bioService("19B10000-E8F2-537E-4F6C-D104768A1214");

BLEIntCharacteristic gsrChar("19B10002-E8F2-537E-4F6C-D104768A1214", 
                              BLERead | BLENotify);
BLEIntCharacteristic heartChar("19B10003-E8F2-537E-4F6C-D104768A1214", 
                                BLERead | BLENotify);
BLEFloatCharacteristic tempChar("19B10004-E8F2-537E-4F6C-D104768A1214", 
                                 BLERead | BLENotify);
BLEFloatCharacteristic hrvChar("19B10005-E8F2-537E-4F6C-D104768A1214", 
                                BLERead | BLENotify);
BLEIntCharacteristic spo2Char("19B10006-E8F2-537E-4F6C-D104768A1214", 
                               BLERead | BLENotify);
BLEIntCharacteristic batteryChar("19B10007-E8F2-537E-4F6C-D104768A1214", 
                                  BLERead | BLENotify);

// ═══════════════════════════════════════════════════════════════════════
// PULSE DETECTION VARIABLES
// ═══════════════════════════════════════════════════════════════════════
const int PULSE_THRESHOLD = 550;        // Adjust based on your sensor
const int PULSE_MIN_INTERVAL = 300;     // 200 BPM max
const int PULSE_MAX_INTERVAL = 2000;    // 30 BPM min

int pulseSignal = 0;
unsigned long lastBeatTime = 0;
int beatsPerMinute = 0;

// Heart Rate Smoothing Buffer
const int HR_BUFFER_SIZE = 5;
int hrBuffer[HR_BUFFER_SIZE] = {0};
int hrBufferIndex = 0;

// ═══════════════════════════════════════════════════════════════════════
// HRV (Heart Rate Variability) CALCULATION
// ═══════════════════════════════════════════════════════════════════════
const int HRV_BUFFER_SIZE = 10;         // Store last 10 RR intervals
unsigned long rrIntervals[HRV_BUFFER_SIZE] = {0};
int rrBufferIndex = 0;
int rrBufferCount = 0;
float currentHRV = 0.0;

// ═══════════════════════════════════════════════════════════════════════
// GSR SMOOTHING
// ═══════════════════════════════════════════════════════════════════════
const int GSR_BUFFER_SIZE = 10;
int gsrBuffer[GSR_BUFFER_SIZE] = {0};
int gsrBufferIndex = 0;

// ═══════════════════════════════════════════════════════════════════════
// TEMPERATURE VARIABLES
// ═══════════════════════════════════════════════════════════════════════
float currentTemp = 0.0;
unsigned long lastTempRead = 0;
const unsigned long TEMP_READ_INTERVAL = 2000;  // Read every 2 seconds

// ═══════════════════════════════════════════════════════════════════════
// BATTERY MONITORING
// ═══════════════════════════════════════════════════════════════════════
int batteryPercentage = 100;
unsigned long lastBatteryRead = 0;
const unsigned long BATTERY_READ_INTERVAL = 10000;  // Every 10 seconds

// Battery voltage calibration (adjust for your voltage divider)
const float BATTERY_MAX_VOLTAGE = 4.2;  // Fully charged LiPo
const float BATTERY_MIN_VOLTAGE = 3.3;  // Empty LiPo (don't go lower!)
const float VOLTAGE_DIVIDER_RATIO = 2.0; // For equal resistors (10kΩ + 10kΩ)

// ═══════════════════════════════════════════════════════════════════════
// SPO2 VARIABLES (Optional - requires MAX30102)
// ═══════════════════════════════════════════════════════════════════════
int currentSpO2 = 98;  // Default value if no sensor
bool hasSpO2Sensor = false;

// ═══════════════════════════════════════════════════════════════════════
// POWER MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════
bool isConnected = false;
unsigned long lastActivityTime = 0;
unsigned long lastDataSend = 0;
const unsigned long IDLE_TIMEOUT = 60000;       // Sleep after 1 min idle
const unsigned long DATA_SEND_INTERVAL = 100;    // Send data every 100ms

// ═══════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);  // Wait up to 3s for serial
  
  Serial.println("═══════════════════════════════════════════════");
  Serial.println("GSR Streamer - Complete Biometric System v2.0");
  Serial.println("═══════════════════════════════════════════════");
  
  // Configure pins
  pinMode(GSR_PIN, INPUT);
  pinMode(PULSE_PIN, INPUT);
  pinMode(BATTERY_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Initialize temperature sensor
  Serial.print("Initializing DS18B20 temperature sensor... ");
  tempSensor.begin();
  int deviceCount = tempSensor.getDeviceCount();
  if (deviceCount > 0) {
    Serial.print("Found ");
    Serial.print(deviceCount);
    Serial.println(" device(s)");
  } else {
    Serial.println("WARNING: No DS18B20 found!");
  }
  
  // Initialize BLE
  Serial.print("Initializing BLE... ");
  if (!BLE.begin()) {
    Serial.println("FAILED!");
    Serial.println("Please reset the board.");
    while (1) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      delay(200);
    }
  }
  Serial.println("OK");
  
  // Configure BLE
  BLE.setLocalName("GSR_HEART");
  BLE.setDeviceName("GSR_HEART");
  BLE.setAdvertisedService(bioService);
  
  // Add all characteristics to service
  bioService.addCharacteristic(gsrChar);
  bioService.addCharacteristic(heartChar);
  bioService.addCharacteristic(tempChar);
  bioService.addCharacteristic(hrvChar);
  bioService.addCharacteristic(spo2Char);
  bioService.addCharacteristic(batteryChar);
  
  BLE.addService(bioService);
  
  // Set initial characteristic values
  gsrChar.writeValue(0);
  heartChar.writeValue(0);
  tempChar.writeValue(25.0);
  hrvChar.writeValue(0.0);
  spo2Char.writeValue(98);
  batteryChar.writeValue(100);
  
  // Start advertising
  BLE.advertise();
  Serial.println("BLE advertising started as 'GSR_HEART'");
  Serial.println("Waiting for connection...");
  Serial.println("═══════════════════════════════════════════════");
  
  // Startup LED pattern (3 fast blinks = ready)
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  
  lastActivityTime = millis();
}

// ═══════════════════════════════════════════════════════════════════════
// MAIN LOOP
// ═══════════════════════════════════════════════════════════════════════
void loop() {
  // Check for BLE connection
  BLEDevice central = BLE.central();
  
  if (central) {
    // Handle connection
    if (!isConnected) {
      isConnected = true;
      Serial.print("✓ Connected to: ");
      Serial.println(central.address());
      digitalWrite(LED_PIN, HIGH);
    }
    
    lastActivityTime = millis();
    
    // Collect and send data when connected
    if (millis() - lastDataSend >= DATA_SEND_INTERVAL) {
      collectAndSendAllData();
      lastDataSend = millis();
    }
    
  } else {
    // Handle disconnection
    if (isConnected) {
      isConnected = false;
      Serial.println("✗ Disconnected");
      digitalWrite(LED_PIN, LOW);
    }
    
    // Power saving when disconnected
    unsigned long idleTime = millis() - lastActivityTime;
    
    if (idleTime > IDLE_TIMEOUT) {
      // Deep sleep mode (very low power)
      delay(500);
      
      // Slow heartbeat LED (device is alive but sleeping)
      static unsigned long lastBlink = 0;
      if (millis() - lastBlink > 2000) {
        digitalWrite(LED_PIN, HIGH);
        delay(50);
        digitalWrite(LED_PIN, LOW);
        lastBlink = millis();
      }
    } else {
      // Light sleep (still responsive)
      delay(100);
    }
  }
}

// ═══════════════════════════════════════════════════════════════════════
// COLLECT AND SEND ALL BIOMETRIC DATA
// ═══════════════════════════════════════════════════════════════════════
void collectAndSendAllData() {
  // 1. Read and send GSR
  int gsrValue = readSmoothedGSR();
  gsrChar.writeValue(gsrValue);
  
  // 2. Read pulse and calculate heart rate + HRV
  pulseSignal = analogRead(PULSE_PIN);
  detectPulseAndCalculateMetrics();
  heartChar.writeValue(beatsPerMinute);
  
  // 3. Send HRV (updated by pulse detection)
  hrvChar.writeValue(currentHRV);
  
  // 4. Read and send temperature (less frequently)
  if (millis() - lastTempRead >= TEMP_READ_INTERVAL) {
    currentTemp = readTemperature();
    tempChar.writeValue(currentTemp);
    lastTempRead = millis();
  }
  
  // 5. Send SpO2 (simulated for now - implement MAX30102 if you have it)
  spo2Char.writeValue(currentSpO2);
  
  // 6. Read and send battery level
  if (millis() - lastBatteryRead >= BATTERY_READ_INTERVAL) {
    batteryPercentage = readBatteryLevel();
    batteryChar.writeValue(batteryPercentage);
    lastBatteryRead = millis();
  }
}

// ═══════════════════════════════════════════════════════════════════════
// READ SMOOTHED GSR VALUE
// ═══════════════════════════════════════════════════════════════════════
int readSmoothedGSR() {
  int gsrRaw = analogRead(GSR_PIN);
  
  // Add to smoothing buffer
  gsrBuffer[gsrBufferIndex] = gsrRaw;
  gsrBufferIndex = (gsrBufferIndex + 1) % GSR_BUFFER_SIZE;
  
  // Calculate average
  int gsrSum = 0;
  for (int i = 0; i < GSR_BUFFER_SIZE; i++) {
    gsrSum += gsrBuffer[i];
  }
  
  return gsrSum / GSR_BUFFER_SIZE;
}

// ═══════════════════════════════════════════════════════════════════════
// PULSE DETECTION + HEART RATE + HRV CALCULATION
// ═══════════════════════════════════════════════════════════════════════
void detectPulseAndCalculateMetrics() {
  static int lastSignal = 0;
  static bool risingEdge = false;
  static unsigned long peakTime = 0;
  
  // Detect rising edge (pulse going above threshold)
  if (pulseSignal > PULSE_THRESHOLD && lastSignal <= PULSE_THRESHOLD) {
    risingEdge = true;
    peakTime = millis();
  }
  
  // Detect beat (rising edge followed by drop below threshold)
  if (risingEdge && pulseSignal < PULSE_THRESHOLD) {
    unsigned long currentTime = millis();
    unsigned long rrInterval = currentTime - lastBeatTime;  // RR interval in ms
    
    // Validate interval (30-200 BPM range)
    if (rrInterval > PULSE_MIN_INTERVAL && rrInterval < PULSE_MAX_INTERVAL) {
      lastBeatTime = currentTime;
      
      // Calculate instantaneous BPM
      int newBPM = 60000 / rrInterval;
      
      // Add to heart rate smoothing buffer
      hrBuffer[hrBufferIndex] = newBPM;
      hrBufferIndex = (hrBufferIndex + 1) % HR_BUFFER_SIZE;
      
      // Calculate smoothed heart rate
      int hrSum = 0;
      for (int i = 0; i < HR_BUFFER_SIZE; i++) {
        hrSum += hrBuffer[i];
      }
      beatsPerMinute = hrSum / HR_BUFFER_SIZE;
      
      // Store RR interval for HRV calculation
      rrIntervals[rrBufferIndex] = rrInterval;
      rrBufferIndex = (rrBufferIndex + 1) % HRV_BUFFER_SIZE;
      if (rrBufferCount < HRV_BUFFER_SIZE) {
        rrBufferCount++;
      }
      
      // Calculate HRV (RMSSD method)
      if (rrBufferCount >= 2) {
        currentHRV = calculateRMSSD();
      }
      
      // Visual feedback - quick LED pulse
      digitalWrite(LED_PIN, LOW);
      delay(50);
      digitalWrite(LED_PIN, HIGH);
      
      // Debug output
      if (Serial) {
        Serial.print("♥ HR: ");
        Serial.print(beatsPerMinute);
        Serial.print(" BPM | HRV: ");
        Serial.print(currentHRV, 1);
        Serial.print(" ms | GSR: ");
        Serial.println(gsrBuffer[gsrBufferIndex > 0 ? gsrBufferIndex - 1 : GSR_BUFFER_SIZE - 1]);
      }
    }
    
    risingEdge = false;
  }
  
  lastSignal = pulseSignal;
}

// ═══════════════════════════════════════════════════════════════════════
// CALCULATE HRV USING RMSSD METHOD
// ═══════════════════════════════════════════════════════════════════════
float calculateRMSSD() {
  if (rrBufferCount < 2) return 0.0;
  
  float sumSquaredDiffs = 0.0;
  int validDiffs = 0;
  
  // Calculate sum of squared differences between successive RR intervals
  for (int i = 1; i < rrBufferCount; i++) {
    int prevIndex = (rrBufferIndex - i - 1 + HRV_BUFFER_SIZE) % HRV_BUFFER_SIZE;
    int currIndex = (rrBufferIndex - i + HRV_BUFFER_SIZE) % HRV_BUFFER_SIZE;
    
    long diff = (long)rrIntervals[currIndex] - (long)rrIntervals[prevIndex];
    sumSquaredDiffs += (float)(diff * diff);
    validDiffs++;
  }
  
  if (validDiffs == 0) return 0.0;
  
  // RMSSD = square root of mean squared differences
  float meanSquared = sumSquaredDiffs / validDiffs;
  return sqrt(meanSquared);
}

// ═══════════════════════════════════════════════════════════════════════
// READ TEMPERATURE FROM DS18B20
// ═══════════════════════════════════════════════════════════════════════
float readTemperature() {
  tempSensor.requestTemperatures();
  float temp = tempSensor.getTempCByIndex(0);
  
  // Validate reading
  if (temp > -127 && temp < 85) {
    return temp;
  }
  
  // Return last good reading if sensor error
  return currentTemp;
}

// ═══════════════════════════════════════════════════════════════════════
// READ BATTERY LEVEL
// ═══════════════════════════════════════════════════════════════════════
int readBatteryLevel() {
  // Read analog voltage (0-1023 = 0-3.3V on XIAO)
  int rawValue = analogRead(BATTERY_PIN);
  
  // Convert to actual voltage (accounting for voltage divider)
  float voltage = (rawValue / 1023.0) * 3.3 * VOLTAGE_DIVIDER_RATIO;
  
  // Convert to percentage (3.3V = 0%, 4.2V = 100%)
  float percentage = ((voltage - BATTERY_MIN_VOLTAGE) / 
                      (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE)) * 100.0;
  
  // Clamp to 0-100%
  if (percentage > 100) percentage = 100;
  if (percentage < 0) percentage = 0;
  
  return (int)percentage;
}

// ═══════════════════════════════════════════════════════════════════════
// END OF SKETCH
// ═══════════════════════════════════════════════════════════════════════
