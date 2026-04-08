#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <ArduinoBLE.h>

MAX30105 particleSensor;

#define LSM6_ADDR 0x6A
#define WHO_AM_I  0x0F
#define CTRL1_XL  0x10
#define CTRL2_G   0x11
#define OUTX_L_XL 0x28
#define OUTX_L_G  0x22

BLEService bioService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEIntCharacteristic gsrChar("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEIntCharacteristic heartChar("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic tempChar("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEFloatCharacteristic hrvChar("19B10005-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLEIntCharacteristic spo2Char("19B10006-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);
BLECharacteristic accelChar("19B10008-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 6);
BLECharacteristic gyroChar("19B10009-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 6);

bool deviceConnected = false;
bool imuAvailable = false;

const byte RATE_SIZE = 20;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute = 0;
int beatAvg = 0;
float smoothedBPM = 0.0;

const int HRV_BUFFER_SIZE = 10;
unsigned long rrIntervals[HRV_BUFFER_SIZE];
int rrIndex = 0;
unsigned long lastRRTime = 0;
float hrv = 0.0;

const int GSR_PIN = A0;
float gsrValue = 0.0;
const int GSR_SAMPLES = 10;
float gsrReadings[GSR_SAMPLES] = {0};
int gsrIndex = 0;

float spo2 = 98.0;
float redACValue = 0, irACValue = 0;
float redDCValue = 0, irDCValue = 0;
int spo2SampleCount = 0;

float temperature = 0.0;
unsigned long lastTempRead = 0;
const unsigned long TEMP_READ_INTERVAL = 2000;

int16_t accelX = 0, accelY = 0, accelZ = 0;
int16_t gyroX = 0, gyroY = 0, gyroZ = 0;

uint8_t readReg(uint8_t reg) {
  Wire.beginTransmission(LSM6_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(LSM6_ADDR, 1);
  return Wire.read();
}

void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(LSM6_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

int16_t read16(uint8_t reg) {
  Wire.beginTransmission(LSM6_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(LSM6_ADDR, 2);
  uint8_t lo = Wire.read();
  uint8_t hi = Wire.read();
  return (int16_t)(hi << 8 | lo);
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("=== GSR Streamer ===");
  
  Serial.print("Initializing MAX30102... ");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("FAILED!");
    while (1);
  }
  Serial.println("OK");
  
  byte ledBrightness = 0x1F;
  byte sampleAverage = 4;
  byte ledMode = 2;
  int sampleRate = 400;
  int pulseWidth = 411;
  int adcRange = 4096;
  
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
  particleSensor.setPulseAmplitudeRed(0x1F);
  particleSensor.setPulseAmplitudeGreen(0);
  particleSensor.enableDIETEMPRDY();
  
  Serial.print("Initializing IMU... ");
  uint8_t imuId = readReg(WHO_AM_I);
  if (imuId == 0x6B || imuId == 0x6A) {
    writeReg(CTRL1_XL, 0x40);
    writeReg(CTRL2_G, 0x40);
    imuAvailable = true;
    Serial.println("OK");
  } else {
    Serial.println("NOT FOUND");
    imuAvailable = false;
  }
  
  Serial.print("Initializing BLE... ");
  if (!BLE.begin()) {
    Serial.println("FAILED!");
    while (1);
  }
  Serial.println("OK");
  
  BLE.setLocalName("GSR_HEART");
  BLE.setAdvertisedService(bioService);
  
  bioService.addCharacteristic(gsrChar);
  bioService.addCharacteristic(heartChar);
  bioService.addCharacteristic(tempChar);
  bioService.addCharacteristic(hrvChar);
  bioService.addCharacteristic(spo2Char);
  bioService.addCharacteristic(accelChar);
  bioService.addCharacteristic(gyroChar);
  
  BLE.addService(bioService);
  
  gsrChar.writeValue(0);
  heartChar.writeValue(0);
  tempChar.writeValue(0.0);
  hrvChar.writeValue(0.0);
  spo2Char.writeValue(0);
  
  BLE.advertise();
  Serial.println("Ready");
}

void loop() {
  BLEDevice central = BLE.central();
  
  if (central) {
    if (!deviceConnected) {
      deviceConnected = true;
      Serial.println("✓ Connected");
    }
    BLE.poll();
  } else {
    if (deviceConnected) {
      deviceConnected = false;
      Serial.println("✗ Disconnected");
    }
  }
  
  long irValue = particleSensor.getIR();
  uint32_t redValue = particleSensor.getRed();
  
  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();
    beatsPerMinute = 60 / (delta / 1000.0);
    
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++) {
        beatAvg += rates[x];
      }
      beatAvg /= RATE_SIZE;
      
      if (smoothedBPM == 0.0) {
        smoothedBPM = beatAvg;
      } else {
        smoothedBPM = (smoothedBPM * 0.85) + (beatAvg * 0.15);
      }
      
      if (lastRRTime > 0) {
        unsigned long rrInterval = millis() - lastRRTime;
        if (rrInterval > 400 && rrInterval < 1500) {
          rrIntervals[rrIndex] = rrInterval;
          rrIndex = (rrIndex + 1) % HRV_BUFFER_SIZE;
          calculateHRV();
        }
      }
      lastRRTime = millis();
    }
  }
  
  if (irValue > 50000 && redValue > 1000) {
    calculateSpO2(irValue, redValue);
  }
  
  updateGSR();
  
  if (millis() - lastTempRead >= TEMP_READ_INTERVAL) {
    temperature = particleSensor.readTemperature();
    lastTempRead = millis();
  }
  
  if (imuAvailable) {
    accelX = read16(OUTX_L_XL);
    accelY = read16(OUTX_L_XL + 2);
    accelZ = read16(OUTX_L_XL + 4);
    
    gyroX = read16(OUTX_L_G);
    gyroY = read16(OUTX_L_G + 2);
    gyroZ = read16(OUTX_L_G + 4);
  }
  
  if (deviceConnected && central) {
    int gsrInt = constrain((int)gsrValue, 0, 1023);
    int heartInt = constrain((int)smoothedBPM, 0, 255);
    int spo2Int = constrain((int)spo2, 85, 100);
    
    gsrChar.writeValue(gsrInt);
    heartChar.writeValue(heartInt);
    tempChar.writeValue(temperature);
    hrvChar.writeValue(hrv);
    spo2Char.writeValue(spo2Int);
    
    if (imuAvailable) {
      uint8_t accelData[6];
      accelData[0] = accelX & 0xFF;
      accelData[1] = (accelX >> 8) & 0xFF;
      accelData[2] = accelY & 0xFF;
      accelData[3] = (accelY >> 8) & 0xFF;
      accelData[4] = accelZ & 0xFF;
      accelData[5] = (accelZ >> 8) & 0xFF;
      accelChar.writeValue(accelData, 6);
      
      uint8_t gyroData[6];
      gyroData[0] = gyroX & 0xFF;
      gyroData[1] = (gyroX >> 8) & 0xFF;
      gyroData[2] = gyroY & 0xFF;
      gyroData[3] = (gyroY >> 8) & 0xFF;
      gyroData[4] = gyroZ & 0xFF;
      gyroData[5] = (gyroZ >> 8) & 0xFF;
      gyroChar.writeValue(gyroData, 6);
    }
    
    delay(10);
  }
  
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 1000) {
    Serial.print("HR:");
    Serial.print((int)smoothedBPM);
    Serial.print(" GSR:");
    Serial.print((int)gsrValue);
    Serial.print(" T:");
    Serial.print(temperature, 1);
    Serial.print(" HRV:");
    Serial.print(hrv, 0);
    Serial.print(" SpO2:");
    Serial.print((int)spo2);
    
    if (imuAvailable) {
      Serial.print(" A:");
      Serial.print(accelX); Serial.print(",");
      Serial.print(accelY); Serial.print(",");
      Serial.print(accelZ);
      Serial.print(" G:");
      Serial.print(gyroX); Serial.print(",");
      Serial.print(gyroY); Serial.print(",");
      Serial.print(gyroZ);
    }
    
    if (deviceConnected) Serial.print(" BLE:✓");
    Serial.println();
    lastPrint = millis();
  }
}

void calculateHRV() {
  if (rrIndex < 3) return;
  
  float sumSquaredDiffs = 0;
  int validPairs = 0;
  
  for (int i = 1; i < HRV_BUFFER_SIZE; i++) {
    if (rrIntervals[i] > 0 && rrIntervals[i-1] > 0) {
      long diff = rrIntervals[i] - rrIntervals[i-1];
      sumSquaredDiffs += (diff * diff);
      validPairs++;
    }
  }
  
  if (validPairs > 1) {
    float rmssd = sqrt(sumSquaredDiffs / validPairs);
    rmssd = rmssd * 0.3;
    rmssd = constrain(rmssd, 15, 80);
    hrv = (hrv * 0.9) + (rmssd * 0.1);
  }
}

void calculateSpO2(long irValue, uint32_t redValue) {
  if (spo2SampleCount == 0) {
    redDCValue = redValue;
    irDCValue = irValue;
    spo2 = 98.0;
  }
  
  redACValue = abs(redValue - redDCValue);
  irACValue = abs(irValue - irDCValue);
  
  redDCValue = (redDCValue * 0.95) + (redValue * 0.05);
  irDCValue = (irDCValue * 0.95) + (irValue * 0.05);
  
  spo2SampleCount++;
  
  if (spo2SampleCount > 100 && irACValue > 0 && redACValue > 0) {
    float ratio = (redACValue / redDCValue) / (irACValue / irDCValue);
    
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
    
    calculatedSpO2 = constrain(calculatedSpO2, 85, 100);
    spo2 = (spo2 * 0.9) + (calculatedSpO2 * 0.1);
  }
}

void updateGSR() {
  float rawGSR = analogRead(GSR_PIN);
  gsrValue = rawGSR;
  
  gsrReadings[gsrIndex] = gsrValue;
  gsrIndex = (gsrIndex + 1) % GSR_SAMPLES;
  
  float sum = 0;
  for (int i = 0; i < GSR_SAMPLES; i++) {
    sum += gsrReadings[i];
  }
  gsrValue = sum / GSR_SAMPLES;
}
