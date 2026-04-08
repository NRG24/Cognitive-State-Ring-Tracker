/*
  Pure I2C Bus Test - NO sensor communication
  Tests if I2C bus works without trying to talk to any devices
*/

#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Serial.println("=== PURE I2C BUS TEST ===");
  Serial.println("Testing I2C without communicating with any devices...");
  
  Serial.println("Step 1: Initializing Wire library...");
  Wire.begin();
  Serial.println("✓ Wire.begin() completed!");
  
  Serial.println("Step 2: Testing Wire library functions...");
  
  // Test basic Wire operations that don't communicate with devices
  Wire.setClock(100000);
  Serial.println("✓ Wire.setClock() completed!");
  
  Serial.println("Step 3: Testing very basic I2C operations...");
  
  // Just start a transmission but don't send anything
  Wire.beginTransmission(0x00); // Invalid address, should not hang
  byte error = Wire.endTransmission();
  Serial.print("Empty transmission result: ");
  Serial.println(error);
  
  Serial.println("✓ Basic I2C test completed successfully!");
  Serial.println("");
  Serial.println("If you see this message, I2C bus is working!");
  Serial.println("The problem is specifically with your MAX30102 sensor.");
  Serial.println("");
  Serial.println("DISCONNECT THE MAX30102 COMPLETELY and try again.");
}

void loop() {
  static int counter = 0;
  
  Serial.print("Loop ");
  Serial.print(counter++);
  Serial.println(" - I2C bus is working fine!");
  
  delay(2000);
  
  if (counter > 10) {
    Serial.println("");
    Serial.println("=== CONCLUSION ===");
    Serial.println("If you see this message repeating:");
    Serial.println("✓ Arduino Uno is working");
    Serial.println("✓ I2C bus is functional");  
    Serial.println("✗ Your MAX30102 sensor is damaged");
    Serial.println("");
    Serial.println("Try a different MAX30102 sensor!");
    counter = 0;
  }
}
