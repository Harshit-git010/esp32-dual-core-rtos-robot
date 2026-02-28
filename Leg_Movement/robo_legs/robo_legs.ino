#include <ESP32Servo.h>

// ---------------- SERVO OBJECTS ----------------
Servo hipL;
Servo hipR;
Servo footL;
Servo footR;

// ---------------- NEUTRAL POSITIONS ----------------
int HIP_L_NEUTRAL  = 90;
int HIP_R_NEUTRAL  = 90;
int FOOT_L_NEUTRAL = 90;
int FOOT_R_NEUTRAL = 90;

// ---------------- STEP PARAMETERS ----------------
int HIP_STEP_ANGLE   = 40;   // step length
int FOOT_TILT_ANGLE  = 22;   // balance tilt
int FOOT_PLANT_ANGLE = 12;    // foot landing

int STEP_DELAY = 1;         // movement speed

// ---------------- FUNCTION DECLARATIONS ----------------
void stand();
void stepRight();
void stepLeft();
void walkForward(int steps);
void smoothMove(Servo &s, int start, int end, int delayMs);

// ---------------- SETUP ----------------
void setup() {

  hipL.attach(13);
  hipR.attach(26);
  footL.attach(25);
  footR.attach(27);

  stand();
  delay(1000);
}

// ---------------- LOOP ----------------
void loop() {

  walkForward(3);
  delay(1000);

}

// ---------------- BASIC STAND ----------------
void stand() {
  hipL.write(HIP_L_NEUTRAL);
  hipR.write(HIP_R_NEUTRAL);
  footL.write(FOOT_L_NEUTRAL);
  footR.write(FOOT_R_NEUTRAL);
}

// ---------------- STEP RIGHT ----------------
void stepRight() {

  // Lean LEFT
  smoothMove(footL, FOOT_L_NEUTRAL, FOOT_L_NEUTRAL + FOOT_TILT_ANGLE, STEP_DELAY);

  // Swing RIGHT leg
  smoothMove(hipR, HIP_R_NEUTRAL, HIP_R_NEUTRAL - HIP_STEP_ANGLE, STEP_DELAY);

  // Plant RIGHT foot
  smoothMove(footR, FOOT_R_NEUTRAL, FOOT_R_NEUTRAL - FOOT_PLANT_ANGLE, STEP_DELAY);

  // Reset LEFT foot
  smoothMove(footL, FOOT_L_NEUTRAL + FOOT_TILT_ANGLE, FOOT_L_NEUTRAL, STEP_DELAY);

  // Reset RIGHT hip
  smoothMove(hipR, HIP_R_NEUTRAL - HIP_STEP_ANGLE, HIP_R_NEUTRAL, STEP_DELAY);
}

// ---------------- STEP LEFT ----------------
void stepLeft() {

  // Lean RIGHT
  smoothMove(footR, FOOT_R_NEUTRAL, FOOT_R_NEUTRAL + FOOT_TILT_ANGLE, STEP_DELAY);

  // Swing LEFT leg
  smoothMove(hipL, HIP_L_NEUTRAL, HIP_L_NEUTRAL + HIP_STEP_ANGLE, STEP_DELAY);

  // Plant LEFT foot
  smoothMove(footL, FOOT_L_NEUTRAL, FOOT_L_NEUTRAL - FOOT_PLANT_ANGLE, STEP_DELAY);

  // Reset RIGHT foot
  smoothMove(footR, FOOT_R_NEUTRAL + FOOT_TILT_ANGLE, FOOT_R_NEUTRAL, STEP_DELAY);

  // Reset LEFT hip
  smoothMove(hipL, HIP_L_NEUTRAL + HIP_STEP_ANGLE, HIP_L_NEUTRAL, STEP_DELAY);
}

// ---------------- WALK FORWARD ----------------
void walkForward(int steps) {
  for (int i = 0; i < steps; i++) {
    stepRight();
    stepLeft();
  }
}

// ---------------- SMOOTH SERVO MOTION ----------------
void smoothMove(Servo &s, int start, int end, int delayMs) {
  int step = (start < end) ? 1 : -1;
  for (int pos = start; pos != end; pos += step) {
    s.write(pos);
    delay(10);
  }
  delay(delayMs);
}
