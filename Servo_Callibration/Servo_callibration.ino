#include <ESP32Servo.h>

// ---------------- SERVO OBJECTS ----------------
Servo hipL;
Servo hipR;
Servo footL;
Servo footR;

// ---------------- NEUTRAL POSITIONS ----------------
const int HIP_L_NEUTRAL  = 0;
const int HIP_R_NEUTRAL  = 90;
const int FOOT_L_NEUTRAL = 90; // change
const int FOOT_R_NEUTRAL = 90; // change

// ---------------- FUNCTION DECLARATIONS ----------------
void stand() {
  hipL.write(HIP_L_NEUTRAL);
  hipR.write(HIP_R_NEUTRAL);
  footL.write(FOOT_L_NEUTRAL);
  footR.write(FOOT_R_NEUTRAL);
}


// ---------------- SETUP ----------------
void setup() {

  Serial.begin(115200);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);


  hipL.attach(13);// s2
  hipR.attach(26); // s1
  footL.attach(27); // end eff
  footR.attach(25);

  stand();
  delay(1000);
}

// ---------------- LOOP ----------------
void loop() {
  stand();
  Serial.print("Done");
  delay(1000);
}