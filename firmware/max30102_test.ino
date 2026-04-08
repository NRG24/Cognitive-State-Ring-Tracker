/*
  MAX30102 Diagnostic Test
  This is a simple test to check if your MAX30102 sensor is working
  Use this to troubleshoot sensor connection issues
*/

#include <Wire.h>
#include "MAX30105.h"

MAX30105 particleSensor;

void setup() {
  Serial.begin(9600);
  Serial.println("MAX30102 Diagnostic Test");
  Serial.println("=========================");
  
  // Initialize I2C
  Wire.begin();
  Serial.println("I2C initialized");
  
  // Try different initialization methods
  Serial.println("Attempting to initialize MAX30102...");
  
  // Method 1: Default initialization
  if (particleSensor.begin()) {
    Serial.println("✓ MAX30102 found with default settings!");
    setupSensor();
    return;
  }
  
  // Method 2: Specify I2C explicitly
  if (particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("✓ MAX30102 found with standard I2C speed!");
    setupSensor();
    return;
  }
  
  // Method 3: Try fast I2C
  if (particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("✓ MAX30102 found with fast I2C speed!");
    setupSensor();
    return;
  }
  
  // If we get here, sensor not found
  Serial.println("✗ MAX30102 sensor not found!");
  Serial.println("\nTroubleshooting steps:");
  Serial.println("1. Check wiring:");
  Serial.println("   - VCC to 3.3V");
  Serial.println("   - GND to GND");
  Serial.println("   - SDA to Pin 4 (or A4)");
  Serial.println("   - SCL to Pin 5 (or A5)");
  Serial.println("2. Check if sensor LED is lit");
  Serial.println("3. Try different I2C address if available");
  
  // Try I2C scan
  Serial.println("\nScanning I2C bus for devices...");
  scanI2C();
}

void setupSensor() {
  Serial.println("Configuring sensor...");
  
  // Basic setup
  particleSensor.setup();
  
  // Set LED brightness (lower values if sensor is too sensitive)
  particleSensor.setPulseAmplitudeRed(0x0A);    // Low setting
  particleSensor.setPulseAmplitudeIR(0x0A);     // Low setting  
  particleSensor.setPulseAmplitudeGreen(0);     // Turn off
  
  Serial.println("✓ Sensor configured successfully!");
  Serial.println("Place finger on sensor...");
}

void loop() {
  static unsigned long lastPrint = 0;
  
  if (millis() - lastPrint > 1000) {  // Print every second
    lastPrint = millis();
    
    long irValue = particleSensor.getIR();
    long redValue = particleSensor.getRed();
    
    Serial.print("IR: ");
    Serial.print(irValue);
    Serial.print(" | Red: ");
    Serial.print(redValue);
    
    if (irValue > 50000) {
      Serial.print(" | ✓ Good signal - finger detected");
    } else if (irValue > 10000) {
      Serial.print(" | ~ Weak signal - press finger firmly");
    } else {
      Serial.print(" | ✗ No finger detected");
    }
    
    Serial.println();
  }
}

void scanI2C() {
  byte error, address;
  int devicesFound = 0;
  
  Serial.println("Scanning I2C addresses...");
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      
      if (address == 0x57) {
        Serial.print(" (This is likely the MAX30102!)");
      }
      
      Serial.println();
      devicesFound++;
    }
  }
  
  if (devicesFound == 0) {
    Serial.println("No I2C devices found. Check wiring!");
  } else {
    Serial.print("Found ");
    Serial.print(devicesFound);
    Serial.println(" I2C device(s).");
  }
}
