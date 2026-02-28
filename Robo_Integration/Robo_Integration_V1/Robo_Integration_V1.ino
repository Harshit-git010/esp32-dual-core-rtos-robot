//*********************************************************************** List of all libraries to be used *********************************************************
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <FluxGarage_RoboEyes.h>

#include "MPU6050.h"
#include <ESP32Servo.h>

// **************************************************************************** Creating Objects **************************************************************************
MPU6050 mpu;
Servo hipL, hipR, footL, footR;

//***************************************************************************** Defining PINs *****************************************************************************
#define IR_EDGE_PIN 34

#define TRIG_PIN 4
#define ECHO_PIN 16

#define HIPL 13
#define HIPR 26
#define FOOTL 25
#define FOOTR 27

//************************************************************************ Defining variables for SERVOs *************************************************************
const int HIP_L_N = 90;
const int HIP_R_N = 90;
const int FOOT_L_N = 90;
const int FOOT_R_N = 90;

const int HIP_STEP = 40;
const int HIP_PLANT = 35;  // Added
const int FOOT_TILT = 25;
const int FOOT_PLANT = 13;

const int MOVE_DELAY = 120;

//******************************************************************************** Defining Sensors ***************************************************************************
int16_t ax, ay, az;
int16_t gx, gy, gz;

int prevAcc = 0;

// Threshold for detecting hand acceleration
#define ACC_THRESHOLD 18000

// ---------- ULTRASONIC ----------
long duration;
float distance;

// ---------- STATES ----------
volatile bool pickedUp = false;
volatile bool edgeDetected = false;
volatile bool edgeHandled = false;
volatile bool obstacleDetected = false;
volatile bool shaken = false;

//*********************************************************************************** Display *************************************************************************************
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RoboEyes template
RoboEyes<Adafruit_SH1106G> roboEyes(display);

// ********************************************************************************* ALL Functions ***********************************************************************

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

  duration = pulseIn(ECHO_PIN, HIGH, 30000);  // timeout 30ms

  if (duration == 0) {
    obstacleDetected = false;
    return;
  }

  distance = (duration * 0.0343) / 2;

  obstacleDetected = (distance < 15);
  Serial.println(distance);
}

void readMPU() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  int totalAcc = abs(ax) + abs(ay) + abs(az);
  int delta = abs(totalAcc - prevAcc);

  pickedUp = delta > 6000;  // tune this

  prevAcc = totalAcc;

  Serial.print(pickedUp);
}

void stepBackward() {

  for (int i = 0; i < 5; i++) {

    // Step backward right  ************
    // (1 & 2) feet move together
    footL.write(FOOT_L_N + FOOT_TILT);   // +
    footR.write(FOOT_R_N + FOOT_PLANT);  // +
    delay(MOVE_DELAY);

    // (3 & 4) hips move together
    hipR.write(HIP_R_N + HIP_PLANT);  // either both + or both -
    hipL.write(HIP_L_N + HIP_STEP);
    delay(MOVE_DELAY);

    // (5 & 6) feet return together
    footL.write(FOOT_L_N);
    footR.write(FOOT_R_N);
    delay(MOVE_DELAY);

    // Step backward left *****************
    // (1 & 2) feet move together
    footR.write(FOOT_R_N - FOOT_TILT);   // -
    footL.write(FOOT_L_N - FOOT_PLANT);  // -
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
}

void stepTurn() {


  /*
  // Right Movement **************
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

  // Move Established *************
  hipR.write(HIP_R_N);   // either both + or both -
  hipL.write(HIP_L_N);
  delay(MOVE_DELAY);
  */

  // (1 & 2) feet move together
  footL.write(FOOT_L_N + FOOT_TILT);   // +
  footR.write(FOOT_R_N + FOOT_PLANT);  // +
  delay(MOVE_DELAY);

  // (3 & 4) hips move together
  hipR.write(HIP_R_N - HIP_STEP);  // either both + or both -
  delay(MOVE_DELAY);

  footL.write(FOOT_L_N - FOOT_PLANT);  // +
  footR.write(FOOT_R_N - FOOT_TILT);   // +
  delay(MOVE_DELAY);

  hipR.write(HIP_R_N);
  delay(MOVE_DELAY);

  hipL.write(HIP_L_N - HIP_STEP);
  delay(MOVE_DELAY);

  footL.write(FOOT_L_N);
  footR.write(FOOT_R_N);
  delay(MOVE_DELAY);

  footL.write(FOOT_L_N + FOOT_TILT);   // +
  footR.write(FOOT_R_N + FOOT_PLANT);  // +
  delay(MOVE_DELAY);

  hipL.write(HIP_L_N);
  delay(MOVE_DELAY);

  footL.write(FOOT_L_N);
  footR.write(FOOT_R_N);
  delay(MOVE_DELAY);
}

// ***************************************************************************** Cores Code **************************************************************************************
void core1(void *pvParameters) {  // For Movement, IR & Ultrasonic
  for (;;) {

    readIR();          // For edge
    readUltrasonic();  // For Obstacle

    // ---------- LOGIC ----------
    if (shaken) {
      getNeutral();
      vTaskDelay(2000);
    }


    if (edgeDetected && !edgeHandled) {
      Serial.println("EDGE DETECTED! STOP!");
      stepBackward();
      stepTurn();
      stepTurn();

      // edgeHandled = true;   // ✅ correct
    }
    if (!edgeDetected) {
      edgeHandled = false;  // reset when back on table
    }


    if (obstacleDetected) {
      Serial.println("Obstacle Ahead!");
      while (obstacleDetected) {
        readUltrasonic();
        stepTurn();
      }
      // slow down or turn
    }

    walk();

    // vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void core0(void *pvParameters) {  // For OLED & MPU
  for (;;) {

    readMPU();

    if (shaken) {
      fuzzy();
    }

    while (edgeDetected) {
      cautious();
    }

    while (obstacleDetected) {
      angry();
    }

    explore();

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

//****************************************************************************** Cores Code End ****************************************************************************************

// Function for Default Position **********************************
void getNeutral() {

  hipL.write(HIP_L_N);
  hipR.write(HIP_R_N);
  footL.write(FOOT_L_N);
  footR.write(FOOT_R_N);
  delay(1000);
}

// Function for SERVOs **************************************************************************
void walk() {
  stepRight();
  stepLeft();
}

// ---------------- STEP RIGHT ----------------
void stepRight() {

  // (1 & 2) feet move together
  footL.write(FOOT_L_N + FOOT_TILT);   // +
  footR.write(FOOT_R_N + FOOT_PLANT);  // +
  delay(MOVE_DELAY);
 
  // (3 & 4) hips move together
  hipR.write(HIP_R_N - HIP_PLANT);  // either both + or both -
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
  footR.write(FOOT_R_N - FOOT_TILT);   // -
  footL.write(FOOT_L_N - FOOT_PLANT);  // -
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


// Function for OLED ****************************************************************************

void explore() {

  display.clearDisplay();
  display.display();

  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2);

  roboEyes.setMood(DEFAULT);

  roboEyes.update();
}

void fuzzy() {

  display.clearDisplay();
  display.display();

  // start fuzzy
  unsigned long fuzzyStartTime = millis();

  // start fuzzy
  roboEyes.setIdleMode(OFF);
  roboEyes.setAutoblinker(ON, 1, 1);  // fast blinking
  roboEyes.setMood(TIRED);

  roboEyes.setHFlicker(ON, 2);
  roboEyes.setVFlicker(ON, 2);

  roboEyes.anim_confused();  // one-shot animation

  // update
  roboEyes.update();
  // Clear Display
}


void cautious() {

  display.clearDisplay();
  display.display();
  unsigned long lastEyeUpdate = 0;
  const unsigned long EYE_UPDATE_INTERVAL = 40;  // ms

  int eyeWidthCurrent = 36;
  int eyeHeightCurrent = 36;

  const int EYE_NORMAL = 36;
  const int EYE_CAUTION = 24;

  // start cautious
  roboEyes.setIdleMode(OFF);
  roboEyes.setAutoblinker(ON, 5, 1);
  roboEyes.setMood(TIRED);
  roboEyes.setPosition(S);  // look down

  // stop cautious
  roboEyes.setPosition(DEFAULT);
  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2);

  // update cautious

  if (millis() - lastEyeUpdate < EYE_UPDATE_INTERVAL) return;
  lastEyeUpdate = millis();

  int targetSize = edgeDetected ? EYE_CAUTION : EYE_NORMAL;

  if (eyeWidthCurrent < targetSize) eyeWidthCurrent++;
  if (eyeWidthCurrent > targetSize) eyeWidthCurrent--;

  if (eyeHeightCurrent < targetSize) eyeHeightCurrent++;
  if (eyeHeightCurrent > targetSize) eyeHeightCurrent--;

  roboEyes.setWidth(eyeWidthCurrent, eyeWidthCurrent);
  roboEyes.setHeight(eyeHeightCurrent, eyeHeightCurrent);

  roboEyes.update();

}

void angry() {
  // Clear Display
  display.clearDisplay();
  display.display();

  const int EYE_ANGRY_WIDTH = 42;
  const int EYE_ANGRY_HEIGHT = 20;

  roboEyes.setIdleMode(OFF);
  roboEyes.setAutoblinker(ON, 2, 1);  // fast, irritated blinking

  roboEyes.setMood(ANGRY);
  roboEyes.setPosition(DEFAULT);

  // Sharp angry eyes
  roboEyes.setWidth(EYE_ANGRY_WIDTH, EYE_ANGRY_WIDTH);
  roboEyes.setHeight(EYE_ANGRY_HEIGHT, EYE_ANGRY_HEIGHT);

  // Get
  roboEyes.setMood(ANGRY);
  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2);

  roboEyes.update();

}


//********************************************************************************* Initial Setup *******************************************************************************
void setup() {

  // For Display *************************************************************************
  Serial.begin(115200);
  Wire.begin();  // ESP32 default SDA/SCL

  // Start OLED
  if (!display.begin(0x3C, true)) {
    Serial.println("OLED init failed");
    while (1);
  }

  display.clearDisplay();
  display.display();

  // Start RoboEyes
  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 80);

  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2);

  // For Sensors ******************************************************************************
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

  // For Servo ***********************************************************************************
  hipL.attach(HIPL);
  hipR.attach(HIPR);
  footL.attach(FOOTL);
  footR.attach(FOOTR);

  getNeutral();

  // Defining Objects for Cores
  xTaskCreatePinnedToCore(
    core0,     // Name of function
    "Core_0",  // Name of Function used by us
    4096,
    NULL,
    1,  // Task Priority
    NULL,
    0  // CORE 0
  );

  xTaskCreatePinnedToCore(
    core1,     // Name of function
    "Core_1",  // Name of Function used by us
    4096,
    NULL,
    2,  // Task Priority
    NULL,
    1  // CORE 0
  );
}

//*********************************************************************************Void Loop ************************************************************************************
void loop() { 
  // empty - RTOS handle everything
}
