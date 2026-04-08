/*
  Basic XIAO Test - No sensors, just serial output
  This tests if your XIAO nRF52840 is working at all
*/

void setup() {
  Serial.begin(9600);
  delay(2000);  // Give serial time to initialize
  
  Serial.println("=== XIAO nRF52840 Basic Test ===");
  Serial.println("If you see this, your XIAO is working!");
  Serial.println("Testing built-in LED...");
  
  // Test built-in LED if available
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  static int counter = 0;
  
  Serial.print("Loop count: ");
  Serial.println(counter++);
  
  // Blink LED
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  
  // Test your GSR sensor too
  int gsrReading = analogRead(A0);
  Serial.print("GSR reading: ");
  Serial.println(gsrReading);
}
