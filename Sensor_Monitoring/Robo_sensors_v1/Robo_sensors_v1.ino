#include <Wire.h>
#include "MPU6050.h"

MPU6050 mpu;

// ---------- PIN DEFINITIONS ----------
#define IR_EDGE_PIN 34

#define TRIG_PIN 4
#define ECHO_PIN 16

// ---------- MPU VARIABLES ----------
int16_t ax, ay, az;
int16_t gx, gy, gz;

int prevAcc = 0;

// Threshold for detecting hand acceleration
#define ACC_THRESHOLD 18000  

// ---------- ULTRASONIC ----------
long duration;
float distance;

// ---------- STATES ----------
bool pickedUp = false;
bool edgeDetected = false;
bool edgeHandled = false;
bool obstacleDetected = false;

// Moods of Robo 

void explore();
void sad();
void sleepy();
void angry();
void fuzzy();
void pet();
void cautious();
void scary();

void fuzzy(){
  for(int i = 0; i<5 ; i++){
    Serial.println("Robot picked / shaken!");
    Serial.println(i);
    delay(200);
  }
  pickedUp = false;
}

void cautious(){
  // call the function to get back and display for caution
  Serial.println("Cautious Face/n");
}

void scary(){
  Serial.println("I am scared of falling !!!");
}

void setup() {
  Serial.begin(115200);

  // I2C for MPU6050
  Wire.begin(21, 22);
  Serial.println("ESP32 I2C OK");
  mpu.initialize();

  if (!mpu.testConnection()) {
  Serial.println("MPU6050 not connected! (continuing)");
  } else {
  Serial.println("MPU6050 connected");
  }


  pinMode(IR_EDGE_PIN, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Robot Sensors Initialized");
}

void loop() {
  readMPU();
  readIR();
  readUltrasonic();

  // ---------- LOGIC ----------
  if (pickedUp) {
    fuzzy();
    // trigger fuzzy animation / stop walking
  }

  if (edgeDetected && !edgeHandled) {
  Serial.println("EDGE DETECTED! STOP!");
  scary();

  edgeHandled = true;   // ✅ correct
  }

  if (!edgeDetected) {
    edgeHandled = false;  // reset when back on table
  }


  if (obstacleDetected) {
    Serial.println("Obstacle Ahead!");
    cautious();
    // slow down or turn
  }

  delay(1000);
}

// ---------- FUNCTIONS ----------

void readMPU() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  int totalAcc = abs(ax) + abs(ay) + abs(az);
  int delta = abs(totalAcc - prevAcc);

  pickedUp = delta > 6000;   // tune this

  prevAcc = totalAcc;
}

void readIR() {
  int irValue = analogRead(IR_EDGE_PIN);

  // TCRT5000: LOW = no surface (edge)
  edgeDetected = (irValue < 500);
  Serial.println(irValue);
}

void readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms

  if (duration == 0) {
    obstacleDetected = false;
    return;
  }

  distance = (duration * 0.0343) / 2;

  obstacleDetected = (distance < 15);
}
