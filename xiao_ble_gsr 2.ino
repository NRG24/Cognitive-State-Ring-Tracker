#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <ArduinoBLE.h>  // BLE activated!

MAX30105 particleSensor;

// BLE Service and Characteristics - exact Flutter app UUIDs
BLEService gsrService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEStringCharacteristic gsrCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 50);
BLEStringCharacteristic heartCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 50);

// BLE variables - ready to activate
bool deviceConnected = false;

const byte RATE_SIZE = 20;  //Increased for much smoother averaging
byte rates[RATE_SIZE];     //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;  //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
float smoothedBPM = 0.0;  //Additional smoothing layer

// GSR Variables - simple
const int GSR_PIN = A0;
float gsrValue = 0.0;
float gsrBaseline = 0.0;
float gsrVariability = 0.0;
const int GSR_SAMPLES = 10;
float gsrReadings[GSR_SAMPLES] = {0};
int gsrIndex = 0;

// HRV Variables - minimal and careful
const int HRV_BUFFER_SIZE = 10;
unsigned long rrIntervals[HRV_BUFFER_SIZE];
int rrIndex = 0;
unsigned long lastRRTime = 0;
float hrv = 0.0;

// SpO2 Variables - simple and careful
float spo2 = 0.0;
float redACValue = 0, irACValue = 0;
float redDCValue = 0, irDCValue = 0;
int spo2SampleCount = 0;

// Temperature Variables - for pattern recognition
float fingerTemp = 0.0;
float tempBaseline = 32.0; // Normal skin temperature baseline in Celsius
unsigned long lastTempReading = 0;
const unsigned long TEMP_INTERVAL = 5000; // Read temperature every 5 seconds

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1)
      ;
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup();                     //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A);  //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);   //Turn off Green LED
  
  // Initialize BLE (placeholder for now)
  initializeBLE();
}

void loop() {
  long irValue = particleSensor.getIR();
  uint32_t redValue = particleSensor.getRed(); // Get red value for SpO2

  if (checkForBeat(irValue) == true) {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;  //Store this reading in the array
      rateSpot %= RATE_SIZE;                     //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
      
      // Additional exponential smoothing for very stable BPM
      if (smoothedBPM == 0.0) {
        smoothedBPM = beatAvg;  // Initialize on first reading
      } else {
        smoothedBPM = (smoothedBPM * 0.85) + (beatAvg * 0.15);  // Heavy smoothing
      }
      
      // HRV calculation - only when we have valid beats
      if (lastRRTime > 0) {
        unsigned long rrInterval = millis() - lastRRTime;
        if (rrInterval > 400 && rrInterval < 1500) { // Tighter range for more realistic RR intervals (40-150 BPM)
          rrIntervals[rrIndex] = rrInterval;
          rrIndex = (rrIndex + 1) % HRV_BUFFER_SIZE;
          calculateHRV();
        }
      }
      lastRRTime = millis();
    }
  }

  // SpO2 calculation - simple and non-interfering
  if (irValue > 50000 && redValue > 1000) {
    calculateSpO2(irValue, redValue);
  }

  // Simple GSR reading with baseline calculation
  updateGSR();

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", Final BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);
  Serial.print(", Smooth BPM=");
  Serial.print(smoothedBPM, 1);
  Serial.print(", Signal Quality=");
  Serial.print(irValue > 50000 ? "GOOD" : "POOR");
  Serial.print(", GSR=");
  Serial.print(gsrValue);
  Serial.print(", HRV=");
  Serial.print(hrv);
  Serial.print(", SpO2=");
  Serial.print(spo2);

  if (irValue < 50000)
    Serial.print(" No finger?");

  if (deviceConnected) {
    Serial.print(" [BLE Connected]");
  } else {
    Serial.print(" [BLE Advertising]");
  }

  Serial.println();
  
  // Update temperature reading periodically
  updateTemperature();
  
  // Send BLE data in Flutter app format
  sendBLEData();
}

void calculateHRV() {
  if (rrIndex < 3) return; // Need at least 3 intervals
  
  float sumSquaredDifferences = 0;
  int validPairs = 0;
  
  // Calculate RMSSD (simple HRV measure)
  for (int i = 1; i < HRV_BUFFER_SIZE; i++) {
    if (rrIntervals[i] > 0 && rrIntervals[i-1] > 0) {
      long diff = rrIntervals[i] - rrIntervals[i-1];
      sumSquaredDifferences += (diff * diff);
      validPairs++;
    }
  }
  
  if (validPairs > 1) {
    float rmssd = sqrt(sumSquaredDifferences / validPairs);
    // Scale down the HRV to realistic range and apply limits
    rmssd = rmssd * 0.3; // Scale down significantly
    rmssd = constrain(rmssd, 15, 80); // Realistic HRV range
    hrv = (hrv * 0.9) + (rmssd * 0.1); // Heavy smoothing
  }
}

void calculateSpO2(long irValue, uint32_t redValue) {
  // Initialize DC values on first run
  if (spo2SampleCount == 0) {
    redDCValue = redValue;
    irDCValue = irValue;
    spo2 = 98.0; // Start with normal value
  }
  
  // Calculate AC components (variation from baseline)
  redACValue = abs(redValue - redDCValue);
  irACValue = abs(irValue - irDCValue);
  
  // Update DC baseline slowly
  redDCValue = (redDCValue * 0.95) + (redValue * 0.05);
  irDCValue = (irDCValue * 0.95) + (irValue * 0.05);
  
  spo2SampleCount++;
  
  // Calculate SpO2 after enough samples
  if (spo2SampleCount > 100 && irACValue > 0 && redACValue > 0) {
    float ratio = (redACValue / redDCValue) / (irACValue / irDCValue);
    
    // Simple SpO2 calculation using standard curve
    float calculatedSpO2;
    if (ratio < 0.5) {
      calculatedSpO2 = 100;
    } else if (ratio < 1.0) {
      calculatedSpO2 = 100 - 5 * ratio;
    } else if (ratio < 2.0) {
      calculatedSpO2 = 100 - 25 * ratio;
    } else {
      calculatedSpO2 = 85;
    }
    
    // Clamp to reasonable range
    calculatedSpO2 = constrain(calculatedSpO2, 85, 100);
    
    // Apply smoothing
    spo2 = (spo2 * 0.9) + (calculatedSpO2 * 0.1);
  }
}

void updateTemperature() {
  // Read temperature every few seconds to avoid overheating the sensor
  if (millis() - lastTempReading > TEMP_INTERVAL) {
    fingerTemp = particleSensor.readTemperature();
    
    // Set baseline on first reading
    if (tempBaseline == 32.0 && fingerTemp > 25.0) {
      tempBaseline = fingerTemp;
    }
    
    lastTempReading = millis();
    
    Serial.print("Temperature: ");
    Serial.print(fingerTemp);
    Serial.println("°C");
  }
}

void updateGSR() {
  float rawGSR = analogRead(GSR_PIN);
  gsrValue = rawGSR;

  // Update baseline using moving average
  gsrReadings[gsrIndex] = gsrValue;
  gsrIndex = (gsrIndex + 1) % GSR_SAMPLES;
  
  float sum = 0;
  for (int i = 0; i < GSR_SAMPLES; i++) {
    sum += gsrReadings[i];
  }
  gsrBaseline = sum / GSR_SAMPLES;
  
  // Calculate variability
  gsrVariability = gsrBaseline > 0 ? abs(gsrValue - gsrBaseline) / gsrBaseline : 0;
}

void sendBLEData() {
  // Check for BLE connection changes
  BLEDevice central = BLE.central();
  
  if (central) {
    if (!deviceConnected) {
      deviceConnected = true;
      Serial.println("BLE device connected!");
    }
    
    // Format data exactly as Flutter app expects - now with temperature
    String gsrData = String(gsrValue) + "," + String(gsrBaseline) + "," + String(gsrVariability) + "," + String(fingerTemp);
    String heartData = String((int)smoothedBPM) + "," + String(hrv) + "," + String(spo2);  // Use smoothed BPM
    
    // Send data to BLE characteristics
    gsrCharacteristic.writeValue(gsrData);
    heartCharacteristic.writeValue(heartData);
    
    // GSR Characteristic: "gsrValue,baseline,variability"
    Serial.print("BLE_GSR: ");
    Serial.println(gsrData);
    
    // Heart Characteristic: "heartRate,hrv,spo2"
    Serial.print("BLE_HEART: ");
    Serial.println(heartData);
    
  } else {
    if (deviceConnected) {
      deviceConnected = false;
      Serial.println("BLE device disconnected!");
    }
  }
}

void initializeBLE() {
  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  // Set BLE device name and service
  BLE.setLocalName("GSR_HEART");
  BLE.setAdvertisedService(gsrService);

  // Add characteristics to service
  gsrService.addCharacteristic(gsrCharacteristic);
  gsrService.addCharacteristic(heartCharacteristic);

  // Add service
  BLE.addService(gsrService);

  // Set initial values
  gsrCharacteristic.writeValue("0,0,0");
  heartCharacteristic.writeValue("0,0,0");

  // Start advertising
  BLE.advertise();
  Serial.println("BLE device active, waiting for connections...");
}

