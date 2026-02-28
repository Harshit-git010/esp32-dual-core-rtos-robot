#include <ESP32Servo.h>

// ---------------- SERVO OBJECTS ----------------
Servo hipL;
Servo hipR;
Servo footL;
Servo footR;

// ---------------- NEUTRAL POSITIONS ----------------
const int HIP_L_NEUTRAL  = 90;
const int HIP_R_NEUTRAL  = 90;
const int FOOT_L_NEUTRAL = 90;
const int FOOT_R_NEUTRAL = 90;

// ---------------- STEP PARAMETERS ----------------
const int HIP_STEP_ANGLE   = 40;   // step length
const int FOOT_TILT_ANGLE  = 22;   // balance tilt
const int FOOT_PLANT_ANGLE = 12;    // foot landing

const int STEP_DELAY = 120;         // movement speed

void stepRight();
void stepLeft();
void move(Servo &s, int end);

void setup() {
  Serial.begin(115200);
  hipL.attach(13);
  hipR.attach(26);
  footL.attach(25);
  footR.attach(27);

  hipL.write(HIP_L_NEUTRAL);
  hipR.write(HIP_R_NEUTRAL);
  footL.write(FOOT_L_NEUTRAL);
  footR.write(FOOT_R_NEUTRAL);

  delay(1000);

}

void loop() {
  // put your main code here, to run repeatedly:
  stepRight();
  stepLeft();

  delay(STEP_DELAY);

}

void stepRight(){
  move(footL,FOOT_L_NEUTRAL + FOOT_TILT_ANGLE); /// correct
  Serial.print("2");
  delay(1000);
  move(hipR,HIP_R_NEUTRAL - HIP_STEP_ANGLE); // correct
  Serial.print("3");
  delay(1000);
  move(footR,FOOT_R_NEUTRAL + FOOT_PLANT_ANGLE); //////// change
  Serial.print("4");
  delay(1000);
  move(footL,FOOT_L_NEUTRAL);
  Serial.print("5");
  delay(1000);
  move(hipR,HIP_R_NEUTRAL);
  Serial.print("6");
  delay(1000);
}

void stepLeft(){
  move(footR,FOOT_R_NEUTRAL + FOOT_TILT_ANGLE);
  Serial.print("7");
  delay(1000);
  move(hipL,HIP_L_NEUTRAL + HIP_STEP_ANGLE);
  Serial.print("8");
  delay(1000);
  move(footL,FOOT_L_NEUTRAL - FOOT_PLANT_ANGLE);
  Serial.print("9");
  delay(1000);
  move(footR,FOOT_R_NEUTRAL);
  Serial.print("10");
  delay(1000);
  move(hipL,HIP_L_NEUTRAL);
  Serial.print("11");
  delay(1000);
}

void move(Servo &s, int end){
  s.write(end);
  delay(STEP_DELAY);
}

/* Try this after ^ works perfectly
void moveSmart(Servo &s, int target) {
  int current = s.read();
  int step = (current < target) ? 2 : -2;

  while (abs(current - target) > 2) {
    current += step;
    s.write(current);
    delay(5);   // not microseconds
  }
  s.write(target);
}

*/
