/*
 * Complete Biometric Sensor System for XIAO nRF52840
 * Includes: GSR, Heart Rate (Pulse Sensor), and Temperature (DS18B20)
 * BLE Service for streaming to Flutter app
 * 
 * Hardware Connections:
 * - GSR Sensor: A0 (analog input)
 * - Pulse Sensor: A1 (analog input)
 * - DS18B20 Temperature: D6 (digital, OneWire)
 * - LED: Built-in LED for status indication
 */

#include <ArduinoBLE.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin Definitions
#define GSR_PIN A0
#define PULSE_PIN A1
#define TEMP_PIN 6
#define LED_PIN LED_BUILTIN

// Temperature sensor setup
OneWire oneWire(TEMP_PIN);
DallasTemperature tempSensor(&oneWire);

// BLE Service and Characteristics
BLEService bioService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEIntCharacteristic gsrChar("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEIntCharacteristic heartChar("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic tempChar("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

// Pulse Detection Variables
const int PULSE_THRESHOLD = 550;  // Adjust based on your sensor
const int PULSE_MIN_INTERVAL = 300; // Minimum ms between beats (200 BPM max)
const int PULSE_MAX_INTERVAL = 2000; // Maximum ms between beats (30 BPM min)
int pulseSignal;
int pulseThreshold = PULSE_THRESHOLD;
unsigned long lastBeatTime = 0;
unsigned long currentBeatTime = 0;
int beatsPerMinute = 0;
bool beatDetected = false;
int beatCount = 0;
unsigned long beatStartTime = 0;

// Moving average for heart rate smoothing
const int HR_BUFFER_SIZE = 5;
int hrBuffer[HR_BUFFER_SIZE];
int hrBufferIndex = 0;
int hrBufferFilled = 0;

// GSR smoothing
const int GSR_BUFFER_SIZE = 10;
int gsrBuffer[GSR_BUFFER_SIZE];
int gsrBufferIndex = 0;
int gsrBufferFilled = 0;

// Temperature variables
float currentTemp = 0.0;
unsigned long lastTempRead = 0;
const unsigned long TEMP_READ_INTERVAL = 2000; // Read temp every 2 seconds

// Timing variables
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 50; // Send data every 50ms (20Hz)

// Status LED
bool ledState = false;
unsigned long lastLedBlink = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000); // Wait up to 3 seconds for serial
  
  Serial.println("=== Complete Biometric Sensor System ===");
  Serial.println("Initializing...");
  
  // Initialize pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(GSR_PIN, INPUT);
  pinMode(PULSE_PIN, INPUT);
  
  // Initialize temperature sensor
  tempSensor.begin();
  Serial.print("Temperature sensors found: ");
  Serial.println(tempSensor.getDeviceCount());
  tempSensor.setResolution(12); // 12-bit resolution (0.0625°C precision)
  
  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("ERROR: Failed to initialize BLE!");
    while (1) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      delay(100);
    }
  }
  
  Serial.println("BLE initialized successfully");
  
  // Set BLE device name and local name
  BLE.setLocalName("GSR_HEART");
  BLE.setDeviceName("GSR_HEART");
  
  // Add characteristics to service
  bioService.addCharacteristic(gsrChar);
  bioService.addCharacteristic(heartChar);
  bioService.addCharacteristic(tempChar);
  
  // Add service
  BLE.addService(bioService);
  
  // Set initial values
  gsrChar.writeValue(0);
  heartChar.writeValue(0);
  tempChar.writeValue(0.0);
  
  // Set advertising parameters
  BLE.setAdvertisingInterval(160); // 100ms intervals (160 * 0.625ms)
  BLE.setConnectable(true);
  
  // Start advertising
  BLE.advertise();
  
  Serial.println("BLE Service started - waiting for connections...");
  Serial.println("Service UUID: 19B10000-E8F2-537E-4F6C-D104768A1214");
  Serial.println("GSR Char UUID: 19B10002-E8F2-537E-4F6C-D104768A1214");
  Serial.println("Heart Char UUID: 19B10003-E8F2-537E-4F6C-D104768A1214");
  Serial.println("Temp Char UUID: 19B10004-E8F2-537E-4F6C-D104768A1214");
  
  // Initialize buffers
  for (int i = 0; i < HR_BUFFER_SIZE; i++) {
    hrBuffer[i] = 0;
  }
  for (int i = 0; i < GSR_BUFFER_SIZE; i++) {
    gsrBuffer[i] = 0;
  }
  
  beatStartTime = millis();
  
  // Blink LED to show ready
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

void loop() {
  // Poll for BLE events
  BLEDevice central = BLE.central();
  
  if (central) {
    Serial.print("Connected to: ");
    Serial.println(central.address());
    digitalWrite(LED_PIN, HIGH);
    
    while (central.connected()) {
      unsigned long currentTime = millis();
      
      // Read pulse sensor for beat detection (continuous)
      detectHeartbeat();
      
      // Read temperature periodically (slower update)
      if (currentTime - lastTempRead >= TEMP_READ_INTERVAL) {
        readTemperature();
        lastTempRead = currentTime;
      }
      
      // Update BLE characteristics at defined interval
      if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
        // Read GSR
        int gsrValue = readGSR();
        
        // Update BLE characteristics
        gsrChar.writeValue(gsrValue);
        heartChar.writeValue(beatsPerMinute);
        tempChar.writeValue(currentTemp);
        
        // Debug output
        Serial.print("GSR: ");
        Serial.print(gsrValue);
        Serial.print(" | BPM: ");
        Serial.print(beatsPerMinute);
        Serial.print(" | Temp: ");
        Serial.print(currentTemp, 1);
        Serial.println("°C");
        
        lastUpdate = currentTime;
      }
      
      // Blink LED when connected
      if (currentTime - lastLedBlink >= 1000) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        lastLedBlink = currentTime;
      }
    }
    
    Serial.println("Disconnected from central");
    digitalWrite(LED_PIN, LOW);
  } else {
    // Not connected - fast blink to show advertising
    unsigned long currentTime = millis();
    if (currentTime - lastLedBlink >= 200) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
      lastLedBlink = currentTime;
    }
  }
}

// Read GSR with smoothing
int readGSR() {
  int rawValue = analogRead(GSR_PIN);
  
  // Add to circular buffer
  gsrBuffer[gsrBufferIndex] = rawValue;
  gsrBufferIndex = (gsrBufferIndex + 1) % GSR_BUFFER_SIZE;
  if (gsrBufferFilled < GSR_BUFFER_SIZE) {
    gsrBufferFilled++;
  }
  
  // Calculate average
  long sum = 0;
  for (int i = 0; i < gsrBufferFilled; i++) {
    sum += gsrBuffer[i];
  }
  
  return sum / gsrBufferFilled;
}

// Detect heartbeat using pulse sensor
void detectHeartbeat() {
  pulseSignal = analogRead(PULSE_PIN);
  unsigned long currentTime = millis();
  
  // Beat detection with hysteresis
  if (pulseSignal > pulseThreshold && !beatDetected) {
    unsigned long timeSinceLastBeat = currentTime - lastBeatTime;
    
    // Only count if enough time has passed since last beat
    if (timeSinceLastBeat > PULSE_MIN_INTERVAL && timeSinceLastBeat < PULSE_MAX_INTERVAL) {
      beatDetected = true;
      currentBeatTime = currentTime;
      
      // Calculate instantaneous BPM
      int instantBPM = 60000 / timeSinceLastBeat;
      
      // Add to moving average buffer
      hrBuffer[hrBufferIndex] = instantBPM;
      hrBufferIndex = (hrBufferIndex + 1) % HR_BUFFER_SIZE;
      if (hrBufferFilled < HR_BUFFER_SIZE) {
        hrBufferFilled++;
      }
      
      // Calculate smoothed BPM
      long sum = 0;
      for (int i = 0; i < hrBufferFilled; i++) {
        sum += hrBuffer[i];
      }
      beatsPerMinute = sum / hrBufferFilled;
      
      // Constrain to reasonable values
      beatsPerMinute = constrain(beatsPerMinute, 30, 200);
      
      lastBeatTime = currentBeatTime;
      beatCount++;
      
      Serial.print("♥ Beat detected! BPM: ");
      Serial.println(beatsPerMinute);
    }
  }
  
  // Reset beat detection when signal drops
  if (pulseSignal < (pulseThreshold - 50)) {
    beatDetected = false;
  }
  
  // Auto-calibrate threshold (slow adaptation)
  static int maxSignal = 0;
  static int minSignal = 1023;
  static unsigned long lastCalibration = 0;
  
  if (currentTime - lastCalibration > 5000) { // Recalibrate every 5 seconds
    maxSignal = max(maxSignal, pulseSignal);
    minSignal = min(minSignal, pulseSignal);
    pulseThreshold = (maxSignal + minSignal) / 2;
    lastCalibration = currentTime;
  } else {
    maxSignal = max(maxSignal, pulseSignal);
    minSignal = min(minSignal, pulseSignal);
  }
  
  // Timeout detection - if no beat for too long, reset BPM
  if (currentTime - lastBeatTime > 3000) {
    beatsPerMinute = 0;
    beatDetected = false;
  }
}

// Read temperature from DS18B20 sensor
void readTemperature() {
  tempSensor.requestTemperatures();
  float tempC = tempSensor.getTempCByIndex(0);
  
  // Check if reading is valid
  if (tempC != DEVICE_DISCONNECTED_C && tempC > -50 && tempC < 125) {
    currentTemp = tempC;
  } else {
    Serial.println("Warning: Temperature sensor read error");
    // Keep last valid temperature
  }
}
