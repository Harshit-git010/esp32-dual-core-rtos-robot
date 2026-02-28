#include <ESP32Servo.h>

// ---------------- SERVOS ----------------
Servo hipL;
Servo hipR;
Servo footL;
Servo footR;

// ---------------- NEUTRAL ----------------
const int HIP_L_NEUTRAL  = 90;
const int HIP_R_NEUTRAL  = 90;
const int FOOT_L_NEUTRAL = 90;
const int FOOT_R_NEUTRAL = 90;

// ---------------- TUNED ANGLES (SAFE) ----------------
const int HIP_STEP  = 40;   // keep small (torque!)
const int FOOT_TILT = 24;
const int FOOT_PLANT = 6;

// ---------------- SPEED ----------------
const int MOVE_DELAY = 200;   // ms (fast, not smooth)

void moveFast(Servo &s, int angle);
void stepRight();
void stepLeft();

void setup() {

  hipL.attach(13);
  hipR.attach(26);
  footL.attach(25);
  footR.attach(27);

  // Stand
  hipL.write(HIP_L_NEUTRAL);
  hipR.write(HIP_R_NEUTRAL);
  footL.write(FOOT_L_NEUTRAL);
  footR.write(FOOT_R_NEUTRAL);

  delay(1000);
}

void loop() {
  stepRight();
  stepLeft();
  delay(200);   // breathing time for servos
}

// ---------------- MOVEMENT ----------------
void stepRight() {

  moveFast(footL, FOOT_L_NEUTRAL + FOOT_TILT);          // shift weight
  moveFast(hipR, HIP_R_NEUTRAL - HIP_STEP);             // swing leg
  moveFast(footR,FOOT_R_NEUTRAL - FOOT_PLANT);         // plant
  moveFast(footL, FOOT_L_NEUTRAL);                      // recover
  moveFast(hipR, HIP_R_NEUTRAL);                        // neutral
}

void stepLeft() {

  moveFast(footR,FOOT_R_NEUTRAL + FOOT_TILT);
  moveFast(hipL, HIP_L_NEUTRAL + HIP_STEP);
  moveFast(footL, FOOT_L_NEUTRAL - FOOT_PLANT);
  moveFast(footR, FOOT_R_NEUTRAL);
  moveFast(hipL,HIP_L_NEUTRAL);
}

// ---------------- FAST SAFE MOVE ----------------
void moveFast(Servo &s, int angle) {
  s.write(angle);
  delay(MOVE_DELAY);
}
