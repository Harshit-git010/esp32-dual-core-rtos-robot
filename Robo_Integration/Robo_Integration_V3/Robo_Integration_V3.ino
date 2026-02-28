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

#define PWMA 27
#define PWMB 14

#define AIN1 25
#define AIN2 26
#define BIN1 32
#define BIN2 33
#define STBY 13


//******************************************************************************** Defining Sensors ***************************************************************************
int16_t ax, ay, az;
int16_t gx, gy, gz;

// Threshold for detecting hand acceleration
int prevAcc = 0;

// ---------- ULTRASONIC ----------
long duration;
float distance;

// ---------- STATES ----------

enum EyeState {
  E_EXPLORE,
  E_FUZZY,
  E_CAUTIOUS,
  E_ANGRY
};

volatile EyeState eyeState = E_EXPLORE;
volatile bool shaken = false;
volatile bool edgeDetected = false;
volatile bool obstacleDetected = false;

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
  edgeDetected = (irValue > 500);
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

  shaken = delta > 6500;  // tune this

  prevAcc = totalAcc;

  Serial.print(delta);
}

void stepBackward() {

  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);

  ledcWrite(PWMA, 180); // speed motor A, speed -> 180/255 ~ 70 %
  ledcWrite(PWMB, 180); // speed motor B, speed

  delay(2000);
}

void stepTurn() {

  while(obstacleDetected || edgeDetected){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  ledcWrite(PWMA, 0); // speed motor A, speed -> 180/255 ~ 70 %
  ledcWrite(PWMB, 180); // speed motor B, speed

  delay(1000);

  readIR();          // For edge
  readUltrasonic();

  }

  ledcWrite(PWMA, 0); // speed motor A, speed -> 180/255 ~ 70 %
  ledcWrite(PWMB, 0); // speed motor B, speed

}

void updateEyeState() {
  if (obstacleDetected) eyeState = E_ANGRY;
  else if (edgeDetected) eyeState = E_CAUTIOUS;
  else if (shaken) eyeState = E_FUZZY;
  else eyeState = E_EXPLORE;
}


// ***************************************************************************** Cores Code **************************************************************************************
void core1(void *pvParameters) {  // For Movement, IR & Ultrasonic
  for (;;) {

    readIR();          // For edge
    readUltrasonic();  // For Obstacle

    // ---------- LOGIC ----------
    if (shaken) {
      toStop();
    }

    if (edgeDetected) {
      Serial.println("EDGE DETECTED! STOP!");
      stepBackward();
      stepTurn();
    }

    if (obstacleDetected) {
      Serial.println("Obstacle Ahead!");
      stepTurn();
      // slow down or turn
    }

    walk();

    // vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void core0(void *pvParameters) {
  for (;;) {

    readMPU();
    updateEyeState();

    switch (eyeState) {
      case E_ANGRY:    angry();    break;
      case E_CAUTIOUS: cautious(); break;
      case E_FUZZY:    fuzzy();    break;
      case E_EXPLORE:  explore();  break;
    }

    vTaskDelay(pdMS_TO_TICKS(40)); // ~25 FPS (perfect for OLED)
  }
}


//****************************************************************************** Cores Code End ****************************************************************************************

// Function to stop **********************************
void toStop() {

  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
  delay(3000);
}

// Function for SERVOs **************************************************************************
void walk() {

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  ledcWrite(PWMA, 180); // speed motor A, speed -> 180/255 ~ 70 %
  ledcWrite(PWMB, 180); // speed motor B, speed

}

// Function for OLED ****************************************************************************

void explore() {

  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2);

  roboEyes.setMood(DEFAULT);

  roboEyes.update();
}

void fuzzy() {

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

}


void cautious() {

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

  cautious();

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

  // For Motors ***********************************************************************************
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);

  // NEW ESP32 PWM API (Core v3.x)
  ledcAttach(PWMA, 1000, 8); // pin, freq, resolution
  ledcAttach(PWMB, 1000, 8);  // 8 -> 2^8 = 0-255

  toStop(); 

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
