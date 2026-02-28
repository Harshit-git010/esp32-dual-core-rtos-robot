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
  Serial.print("2\n");
  //delay(1000);
  move(hipR,HIP_R_NEUTRAL - HIP_STEP_ANGLE); // correct
  Serial.print("3\n");
  //delay(1000);
  move(footL,FOOT_L_NEUTRAL); /// change
  Serial.print("4\n");
  //delay(1000);
  move(footR,FOOT_R_NEUTRAL - FOOT_PLANT_ANGLE); //////// change
  Serial.print("5\n");
  //delay(1000);
  move(hipL,HIP_L_NEUTRAL + HIP_STEP_ANGLE);/// change
  Serial.print("6\n");
  //delay(1000);
  move(footR,FOOT_R_NEUTRAL);
  Serial.print("7\n");
  //delay(1000);
}

void stepLeft(){

  // 1. Shift weight to RIGHT foot
  move(footR, FOOT_R_NEUTRAL + FOOT_TILT_ANGLE);

  // 2. Swing LEFT leg forward
  move(hipL, HIP_L_NEUTRAL + HIP_STEP_ANGLE);

  // 3. Bring RIGHT hip back to neutral (body follows)
  move(hipR, HIP_R_NEUTRAL);

  // 4. Plant LEFT foot
  move(footL, FOOT_L_NEUTRAL - FOOT_PLANT_ANGLE);

  // 5. Reset RIGHT foot
  move(footR, FOOT_R_NEUTRAL);

  // 6. Reset LEFT hip
  move(hipL, HIP_L_NEUTRAL); 
  /*
  move(footR,FOOT_R_NEUTRAL - FOOT_TILT_ANGLE);
  Serial.print("8\n");
  //delay(1000);
  move(hipL,HIP_L_NEUTRAL + HIP_STEP_ANGLE);
  Serial.print("9\n");
  //delay(1000);
  move(footR,FOOT_R_NEUTRAL); // change
  Serial.print("10\n");
  //delay(1000);
  move(footL,FOOT_L_NEUTRAL - FOOT_TILT_ANGLE);
  Serial.print("11\n");
  //delay(1000);
  move(hipR,HIP_R_NEUTRAL + HIP_STEP_ANGLE);
  Serial.print("12\n");
  //delay(1000);
  move(footL,FOOT_L_NEUTRAL);
  Serial.print("13\n");
  //delay(1000);
  */

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
