void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.print("HELLO FROM ESP32");
}

void loop() {}


/*
void setup() {
  pinMode(2, OUTPUT);   // onboard LED on many ESP32s
}

void loop() {
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(500);
}
*/