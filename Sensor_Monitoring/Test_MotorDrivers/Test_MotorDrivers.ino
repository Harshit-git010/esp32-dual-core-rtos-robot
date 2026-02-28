#define PWMA 27
#define PWMB 14

#define AIN1 25
#define AIN2 26
#define BIN1 32
#define BIN2 33
#define STBY 13

void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);

  // NEW ESP32 PWM API (Core v3.x)
  ledcAttach(PWMA, 1000, 8); // pin, freq, resolution
  ledcAttach(PWMB, 1000, 8);  // 8 -> 2^8 = 0-255
}

void loop() {
  // Forward
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  ledcWrite(PWMA, 180); // speed motor A, speed -> 180/255 ~ 70 %
  ledcWrite(PWMB, 180); // speed motor B, speed

  delay(3000);

  // Stop
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
  delay(2000);
}
