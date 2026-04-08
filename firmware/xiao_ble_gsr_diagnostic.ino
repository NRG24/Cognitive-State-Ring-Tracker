#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <ArduinoBLE.h>

MAX30105 particleSensor;

// BLE Service and Characteristics
BLEService gsrService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEStringCharacteristic gsrCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 50);
BLEStringCharacteristic heartCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 50);

bool deviceConnected = false;

// Heart rate variables
const byte RATE_SIZE = 20;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;
float smoothedBPM = 0.0;

// GSR Variables
const int GSR_PIN = A0;
float gsrValue = 0.0;
float gsrBaseline = 0.0;
float gsrVariability = 0.0;
const int GSR_SAMPLES = 10;
float gsrReadings[GSR_SAMPLES] = {0};
int gsrIndex = 0;

// DIAGNOSTIC MODE - Set to true for troubleshooting
const bool DIAGNOSTIC_MODE = true;
unsigned long lastDiagnostic = 0;
const unsigned long DIAGNOSTIC_INTERVAL = 2000; // Print diagnostics every 2 seconds

// GSR Health Check Variables
int lowReadingCount = 0;
int highReadingCount = 0;
bool sensorWarningIssued = false;

// HRV Variables
const int HRV_BUFFER_SIZE = 10;
unsigned long rrIntervals[HRV_BUFFER_SIZE];
int rrIndex = 0;
unsigned long lastRRTime = 0;
float hrv = 0.0;

// SpO2 Variables
float spo2 = 0.0;
float redACValue = 0, irACValue = 0;
float redDCValue = 0, irDCValue = 0;
int spo2SampleCount = 0;

// Temperature Variables
float fingerTemp = 0.0;
unsigned long lastTempReading = 0;
const unsigned long TEMP_INTERVAL = 5000;

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("\n\n=================================");
  Serial.println("GSR SENSOR DIAGNOSTIC MODE");
  Serial.println("=================================\n");

  // GSR Pin Diagnostic
  pinMode(GSR_PIN, INPUT);
  Serial.print("Testing GSR Pin (A0)...");
  int rawReading = analogRead(GSR_PIN);
  Serial.print(" Raw ADC: ");
  Serial.print(rawReading);
  Serial.print(" | Voltage: ");
  Serial.print((rawReading / 1023.0) * 3.3);
  Serial.println("V");

  if (rawReading < 10) {
    Serial.println("⚠️  WARNING: Very low reading - check for short circuit");
  } else if (rawReading > 1000) {
    Serial.println("⚠️  WARNING: Very high reading - check electrode contact");
  } else if (rawReading > 20 && rawReading < 50) {
    Serial.println("⚠️  POSSIBLE ISSUE: Reading ~30 suggests poor contact or damaged sensor");
  } else {
    Serial.println("✓ Pin reading in normal range");
  }

  Serial.println("\nInitializing MAX30102...");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("❌ MAX30102 not found. Check wiring/power.");
    while (1);
  }
  Serial.println("✓ MAX30102 initialized");

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
  
  Serial.println("\nInitializing BLE...");
  initializeBLE();
  
  Serial.println("\n=================================");
  Serial.println("DIAGNOSTIC INFO:");
  Serial.println("- Normal GSR range: 100-800");
  Serial.println("- Electrode contact: 200-600");
  Serial.println("- No contact: >900");
  Serial.println("- Short circuit: <50");
  Serial.println("=================================\n");
}

void loop() {
  // Read GSR with diagnostics
  readGSRWithDiagnostics();
  
  // Read heart rate data
  long irValue = particleSensor.getIR();
  uint32_t redValue = particleSensor.getRed();

  if (checkForBeat(irValue) == true) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
      
      if (smoothedBPM == 0.0) {
        smoothedBPM = beatAvg;
      } else {
        smoothedBPM = (smoothedBPM * 0.85) + (beatAvg * 0.15);
      }
      
      // HRV calculation
      if (lastRRTime > 0) {
        unsigned long rrInterval = delta;
        if (rrInterval > 300 && rrInterval < 2000) {
          rrIntervals[rrIndex++] = rrInterval;
          if (rrIndex >= HRV_BUFFER_SIZE) rrIndex = 0;
          calculateHRV();
        }
      }
      lastRRTime = millis();
    }
  }

  // Calculate SpO2
  if (irValue > 50000) {
    calculateSpO2(redValue, irValue);
  }

  // Update temperature
  updateTemperature();

  // Send BLE data
  if (deviceConnected) {
    sendBLEData();
  }

  // Print diagnostic info
  if (DIAGNOSTIC_MODE && (millis() - lastDiagnostic > DIAGNOSTIC_INTERVAL)) {
    printDiagnostics();
    lastDiagnostic = millis();
  }

  delay(100);
}

void readGSRWithDiagnostics() {
  int rawValue = analogRead(GSR_PIN);
  float voltage = (rawValue / 1023.0) * 3.3;
  
  // Store raw reading for diagnostics
  gsrReadings[gsrIndex] = rawValue;
  gsrIndex = (gsrIndex + 1) % GSR_SAMPLES;
  
  // Calculate average
  float sum = 0;
  for (int i = 0; i < GSR_SAMPLES; i++) {
    sum += gsrReadings[i];
  }
  gsrValue = sum / GSR_SAMPLES;
  
  // Health check for stuck/bad readings
  if (gsrValue < 50) {
    lowReadingCount++;
    highReadingCount = 0;
  } else if (gsrValue > 900) {
    highReadingCount++;
    lowReadingCount = 0;
  } else {
    lowReadingCount = 0;
    highReadingCount = 0;
  }
  
  // Issue warning if reading is stuck
  if ((lowReadingCount > 20 || highReadingCount > 20) && !sensorWarningIssued) {
    Serial.println("\n⚠️  GSR SENSOR WARNING ⚠️");
    if (lowReadingCount > 20) {
      Serial.println("Stuck at LOW value (~30)");
      Serial.println("Possible causes:");
      Serial.println("  1. Short circuit");
      Serial.println("  2. Damaged sensor");
      Serial.println("  3. Wrong pin configuration");
    } else {
      Serial.println("Stuck at HIGH value (>900)");
      Serial.println("Possible causes:");
      Serial.println("  1. No electrode contact");
      Serial.println("  2. Broken wire");
      Serial.println("  3. Open circuit");
    }
    Serial.println("");
    sensorWarningIssued = true;
  }
  
  // Calculate baseline and variability
  if (gsrBaseline == 0) {
    gsrBaseline = gsrValue;
  } else {
    gsrBaseline = gsrBaseline * 0.95 + gsrValue * 0.05;
  }
  
  float deviation = abs(gsrValue - gsrBaseline);
  gsrVariability = deviation / (gsrBaseline + 1);
}

void printDiagnostics() {
  Serial.println("\n--- GSR DIAGNOSTICS ---");
  Serial.print("Raw ADC: ");
  Serial.print((int)gsrValue);
  Serial.print(" | Voltage: ");
  Serial.print((gsrValue / 1023.0) * 3.3, 3);
  Serial.print("V | Baseline: ");
  Serial.print((int)gsrBaseline);
  Serial.print(" | Variability: ");
  Serial.println(gsrVariability, 3);
  
  // Show stability
  float minReading = 9999, maxReading = 0;
  for (int i = 0; i < GSR_SAMPLES; i++) {
    if (gsrReadings[i] < minReading) minReading = gsrReadings[i];
    if (gsrReadings[i] > maxReading) maxReading = gsrReadings[i];
  }
  Serial.print("Range: ");
  Serial.print((int)minReading);
  Serial.print(" - ");
  Serial.print((int)maxReading);
  Serial.print(" | Stability: ");
  
  float range = maxReading - minReading;
  if (range < 5) {
    Serial.println("STUCK - No variation!");
  } else if (range < 20) {
    Serial.println("Very stable (might be stuck)");
  } else if (range < 50) {
    Serial.println("Stable");
  } else if (range < 100) {
    Serial.println("Normal variation");
  } else {
    Serial.println("High variation");
  }
  
  // Heart rate status
  Serial.print("Heart Rate: ");
  Serial.print((int)smoothedBPM);
  Serial.print(" BPM | HRV: ");
  Serial.print(hrv, 1);
  Serial.print(" ms | SpO2: ");
  Serial.print((int)spo2);
  Serial.println("%");
  Serial.println("----------------------\n");
}

void calculateHRV() {
  if (rrIndex < 2) return;
  
  float sum = 0;
  int count = 0;
  for (int i = 0; i < HRV_BUFFER_SIZE - 1; i++) {
    if (rrIntervals[i] > 0 && rrIntervals[i + 1] > 0) {
      long diff = abs((long)rrIntervals[i] - (long)rrIntervals[i + 1]);
      sum += (diff * diff);
      count++;
    }
  }
  
  if (count > 0) {
    hrv = sqrt(sum / count);
  }
}

void calculateSpO2(uint32_t redValue, long irValue) {
  spo2SampleCount++;
  
  if (redDCValue == 0) redDCValue = redValue;
  if (irDCValue == 0) irDCValue = irValue;
  
  redDCValue = redDCValue * 0.95 + redValue * 0.05;
  irDCValue = irDCValue * 0.95 + irValue * 0.05;
  
  float redAC = abs(redValue - redDCValue);
  float irAC = abs(irValue - irDCValue);
  
  redACValue = redACValue * 0.9 + redAC * 0.1;
  irACValue = irACValue * 0.9 + irAC * 0.1;
  
  if (spo2SampleCount >= 50 && irACValue > 0 && irDCValue > 0) {
    float ratio = (redACValue / redDCValue) / (irACValue / irDCValue);
    spo2 = 110.0 - 25.0 * ratio;
    spo2 = constrain(spo2, 80, 100);
    spo2SampleCount = 0;
  }
}

void updateTemperature() {
  if (millis() - lastTempReading > TEMP_INTERVAL) {
    fingerTemp = particleSensor.readTemperature();
    lastTempReading = millis();
  }
}

void sendBLEData() {
  String gsrData = String(gsrValue) + "," + String(gsrBaseline) + "," + String(gsrVariability) + "," + String(fingerTemp);
  gsrCharacteristic.writeValue(gsrData);

  String heartData = String((int)smoothedBPM) + "," + String(hrv) + "," + String((int)spo2);
  heartCharacteristic.writeValue(heartData);
}

void initializeBLE() {
  if (!BLE.begin()) {
    Serial.println("❌ Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("GSR_HEART");
  BLE.setAdvertisedService(gsrService);
  
  gsrService.addCharacteristic(gsrCharacteristic);
  gsrService.addCharacteristic(heartCharacteristic);
  
  BLE.addService(gsrService);
  BLE.advertise();
  
  Serial.println("✓ BLE initialized - Waiting for connections...");
  
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
}

void blePeripheralConnectHandler(BLEDevice central) {
  deviceConnected = true;
  Serial.println("✓ Connected to central device");
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  deviceConnected = false;
  Serial.println("✗ Disconnected from central device");
}
