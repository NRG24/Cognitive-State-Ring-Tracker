/*
  Enhanced MAX30102 Scanner with Reset
  Tries to wake up and reset the sensor before scanning
*/

#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Serial.println("Enhanced MAX30102 I2C Scanner");
  Serial.println("===============================");
  Serial.println("If this hangs at 'Wire.begin()', SDA is shorted to ground!");
  
  Wire.begin();
  Serial.println("✓ Wire.begin() completed successfully");
  
  // Try to reset sensor at common addresses
  Serial.println("Attempting to reset sensor...");
  Serial.println("If this hangs, there's an I2C bus lockup!");
  
  resetSensorAtAddress(0x57); // MAX30102 address
  resetSensorAtAddress(0x56); // Alternative address
  resetSensorAtAddress(0x55); // Another alternative
  
  delay(1000); // Wait for reset to complete
  
  Serial.println("Starting I2C scan...");
}

void loop() {
  int devicesFound = 0;
  
  Serial.println("\nScanning I2C addresses 0x01 to 0x7F...");
  
  for(byte address = 1; address < 128; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("Device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      
      // Check known pulse sensor addresses
      if (address == 0x57) Serial.print(" (MAX30102)");
      else if (address == 0x56) Serial.print(" (Possible MAX30102 variant)");
      else if (address == 0x55) Serial.print(" (Another pulse sensor?)");
      else if (address == 0x48) Serial.print(" (Unknown sensor)");
      
      Serial.println();
      
      // Try to read device ID
      readDeviceID(address);
      
      devicesFound++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  
  if (devicesFound == 0) {
    Serial.println("No I2C devices found!");
    Serial.println("Check:");
    Serial.println("- VCC connected to 3.3V");
    Serial.println("- GND connected");
    Serial.println("- SDA connected to A4 (Uno)");
    Serial.println("- SCL connected to A5 (Uno)");
  } else {
    Serial.print("Found ");
    Serial.print(devicesFound);
    Serial.println(" device(s)");
  }
  
  delay(5000);
}

void resetSensorAtAddress(byte address) {
  Serial.print("Trying reset at 0x");
  if (address < 16) Serial.print("0");
  Serial.print(address, HEX);
  Serial.print("... ");
  
  Wire.beginTransmission(address);
  Wire.write(0x09); // Mode Configuration register
  Wire.write(0x40); // Reset bit
  byte error = Wire.endTransmission();
  
  if (error == 0) {
    Serial.println("Reset command sent successfully");
  } else {
    Serial.print("Failed (error ");
    Serial.print(error);
    Serial.println(")");
  }
}

void readDeviceID(byte address) {
  Serial.print("  Reading device ID... ");
  
  Wire.beginTransmission(address);
  Wire.write(0xFF); // Device ID register
  byte error = Wire.endTransmission(false);
  
  if (error == 0) {
    Wire.requestFrom(address, 1);
    if (Wire.available()) {
      byte deviceID = Wire.read();
      Serial.print("ID: 0x");
      Serial.print(deviceID, HEX);
      
      if (deviceID == 0x15) {
        Serial.println(" ✓ This is a MAX30102!");
      } else if (deviceID == 0x11) {
        Serial.println(" (This might be a MAX30100)");
      } else {
        Serial.println(" (Unknown device)");
      }
    } else {
      Serial.println("No data received");
    }
  } else {
    Serial.print("Read failed (error ");
    Serial.print(error);
    Serial.println(")");
  }
}
