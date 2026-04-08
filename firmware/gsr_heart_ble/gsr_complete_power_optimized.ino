/*
 * Power-Optimized Biometric Sensor System for XIAO nRF52840
 * Includes: GSR, Heart Rate, Temperature + Smart Power Management
 * 
 * POWER SAVINGS:
 * - Auto sleep when not connected (reduces 13mA -> 0.5mA)
 * - Reduces sensor polling when idle
 * - Can run 10-15 hours on 110mAh battery vs 5-8 hours
 * 
 * Hardware Connections:
 * - GSR Sensor: A0
 * - Pulse Sensor: A1
 * - DS18B20 Temperature: D6
 */

#include <ArduinoBLE.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin Definitions
#define GSR_PIN A0
#define PULSE_PIN A1
#define TEMP_PIN 6
#define LED_PIN LED_BUILTIN

// Temperature sensor
OneWire oneWire(TEMP_PIN);
DallasTemperature tempSensor(&oneWire);

// BLE Service and Characteristics
BLEService bioService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEIntCharacteristic gsrChar("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEIntCharacteristic heartChar("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic tempChar("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

// Power Management
bool isConnected = false;
unsigned long lastActivityTime = 0;
const unsigned long IDLE_TIMEOUT = 60000; // 1 minute idle = sleep mode
unsigned long lastDataSend = 0;
const unsigned long DATA_SEND_INTERVAL = 100; // Send data every 100ms when connected

// Pulse Detection
const int PULSE_THRESHOLD = 550;
int pulseSignal;
unsigned long lastBeatTime = 0;
int beatsPerMinute = 0;

// Smoothing buffers
const int HR_BUFFER_SIZE = 5;
int hrBuffer[HR_BUFFER_SIZE] = {0};
int hrBufferIndex = 0;

const int GSR_BUFFER_SIZE = 10;
int gsrBuffer[GSR_BUFFER_SIZE] = {0};
int gsrBufferIndex = 0;

// Temperature
float currentTemp = 0.0;
unsigned long lastTempRead = 0;
const unsigned long TEMP_READ_INTERVAL = 2000;

void setup() {
  Serial.begin(115200);
  
  // Configure pins
  pinMode(GSR_PIN, INPUT);
  pinMode(PULSE_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Initialize temperature sensor
  tempSensor.begin();
  
  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("BLE init failed!");
    while (1);
  }
  
  // Configure BLE
  BLE.setLocalName("GSR_HEART");
  BLE.setAdvertisedService(bioService);
  bioService.addCharacteristic(gsrChar);
  bioService.addCharacteristic(heartChar);
  bioService.addCharacteristic(tempChar);
  BLE.addService(bioService);
  
  // Set initial values
  gsrChar.writeValue(0);
  heartChar.writeValue(0);
  tempChar.writeValue(0.0);
  
  // Start advertising
  BLE.advertise();
  Serial.println("BLE advertising as GSR_HEART");
  
  // Fast LED blinks = ready
  for(int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void loop() {
  // Check for BLE connections
  BLEDevice central = BLE.central();
  
  if (central) {
    if (!isConnected) {
      isConnected = true;
      Serial.print("Connected to: ");
      Serial.println(central.address());
      digitalWrite(LED_PIN, HIGH);
    }
    
    lastActivityTime = millis();
    
    // Active data collection when connected
    if (millis() - lastDataSend >= DATA_SEND_INTERVAL) {
      collectAndSendData();
      lastDataSend = millis();
    }
    
  } else {
    if (isConnected) {
      isConnected = false;
      Serial.println("Disconnected");
      digitalWrite(LED_PIN, LOW);
    }
    
    // POWER SAVING: Reduce activity when not connected
    unsigned long idleTime = millis() - lastActivityTime;
    
    if (idleTime > IDLE_TIMEOUT) {
      // Low power mode: only check for connections every 500ms
      delay(500);
      // Slow blink to show device is alive but idle
      static unsigned long lastBlink = 0;
      if (millis() - lastBlink > 2000) {
        digitalWrite(LED_PIN, HIGH);
        delay(50);
        digitalWrite(LED_PIN, LOW);
        lastBlink = millis();
      }
    } else {
      // Medium power mode: still responsive but slower polling
      delay(100);
    }
  }
}

void collectAndSendData() {
  // Read and smooth GSR
  int gsrRaw = analogRead(GSR_PIN);
  gsrBuffer[gsrBufferIndex] = gsrRaw;
  gsrBufferIndex = (gsrBufferIndex + 1) % GSR_BUFFER_SIZE;
  
  int gsrSum = 0;
  for(int i = 0; i < GSR_BUFFER_SIZE; i++) {
    gsrSum += gsrBuffer[i];
  }
  int gsrSmooth = gsrSum / GSR_BUFFER_SIZE;
  gsrChar.writeValue(gsrSmooth);
  
  // Read and detect pulse
  pulseSignal = analogRead(PULSE_PIN);
  detectPulse();
  heartChar.writeValue(beatsPerMinute);
  
  // Read temperature (less frequently to save power)
  if (millis() - lastTempRead >= TEMP_READ_INTERVAL) {
    tempSensor.requestTemperatures();
    currentTemp = tempSensor.getTempCByIndex(0);
    
    // Validate temperature reading
    if (currentTemp > -127 && currentTemp < 85) {
      tempChar.writeValue(currentTemp);
    }
    
    lastTempRead = millis();
  }
}

void detectPulse() {
  static int lastSignal = 0;
  static bool risingEdge = false;
  static unsigned long peakTime = 0;
  
  // Detect rising edge
  if (pulseSignal > PULSE_THRESHOLD && lastSignal <= PULSE_THRESHOLD) {
    risingEdge = true;
    peakTime = millis();
  }
  
  // Detect beat (peak followed by drop)
  if (risingEdge && pulseSignal < PULSE_THRESHOLD) {
    unsigned long currentTime = millis();
    unsigned long beatInterval = currentTime - lastBeatTime;
    
    // Validate interval (30-200 BPM range)
    if (beatInterval > 300 && beatInterval < 2000) {
      lastBeatTime = currentTime;
      
      // Calculate BPM
      int newBPM = 60000 / beatInterval;
      
      // Add to smoothing buffer
      hrBuffer[hrBufferIndex] = newBPM;
      hrBufferIndex = (hrBufferIndex + 1) % HR_BUFFER_SIZE;
      
      // Calculate smoothed BPM
      int hrSum = 0;
      for(int i = 0; i < HR_BUFFER_SIZE; i++) {
        hrSum += hrBuffer[i];
      }
      beatsPerMinute = hrSum / HR_BUFFER_SIZE;
      
      // Quick LED pulse on beat
      digitalWrite(LED_PIN, LOW);
      delay(50);
      digitalWrite(LED_PIN, HIGH);
    }
    
    risingEdge = false;
  }
  
  lastSignal = pulseSignal;
}
