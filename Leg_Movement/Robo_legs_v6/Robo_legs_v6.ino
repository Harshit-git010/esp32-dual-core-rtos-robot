#include <ESP32Servo.h>

// ---------------- SERVO OBJECTS ----------------
Servo hipL, hipR, footL, footR;

// ---------------- NEUTRAL ----------------
const int HIP_L_N  = 90;
const int HIP_R_N  = 90;
const int FOOT_L_N = 90;
const int FOOT_R_N = 90;

// ---------------- MOVEMENT AMPLITUDE ----------------
const int HIP_STEP  = 40;
const int HIP_PLANT = 35;   // Added
const int FOOT_TILT = 25;
const int FOOT_PLANT = 13;

// ---------------- SPEED ----------------
const int MOVE_DELAY = 120;

// ---------------- SETUP ----------------
void setup() {
  hipL.attach(13);
  hipR.attach(26);
  footL.attach(25);
  footR.attach(27);

  // Step-1: Initial neutral
  hipL.write(HIP_L_N);
  hipR.write(HIP_R_N);
  footL.write(FOOT_L_N);
  footR.write(FOOT_R_N);

  delay(1000);
}

// ---------------- LOOP ----------------
void loop() {
  stepRight();
  stepLeft();
}

// ---------------- STEP RIGHT ----------------
void stepRight() {

  // (1 & 2) feet move together
  footL.write(FOOT_L_N + FOOT_TILT);  // +
  footR.write(FOOT_R_N + FOOT_PLANT); // +
  delay(MOVE_DELAY);

  // (3 & 4) hips move together
  hipR.write(HIP_R_N - HIP_PLANT);   // either both + or both -
  hipL.write(HIP_L_N - HIP_STEP);
  delay(MOVE_DELAY);

  // (5 & 6) feet return together
  footL.write(FOOT_L_N);
  footR.write(FOOT_R_N);
  delay(MOVE_DELAY);
}

// ---------------- STEP LEFT ----------------
void stepLeft() {

  // (1 & 2) feet move together
  footR.write(FOOT_R_N - FOOT_TILT);  // -
  footL.write(FOOT_L_N - FOOT_PLANT); // -
  delay(MOVE_DELAY);

  // (3 & 4) hips return together
  hipL.write(HIP_L_N);
  hipR.write(HIP_R_N);
  delay(MOVE_DELAY);

  // (5 & 6) feet return together
  footR.write(FOOT_R_N);
  footL.write(FOOT_L_N);
  delay(MOVE_DELAY);
}
