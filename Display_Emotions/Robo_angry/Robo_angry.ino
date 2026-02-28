#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// ✅ Use SH1106G (NOT Adafruit_SH110X)
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ✅ Correct RoboEyes template
RoboEyes<Adafruit_SH1106G> roboEyes(display);

bool obstacleDetected = true;
bool angryActive = false;

// Angry eye shape
const int EYE_ANGRY_WIDTH  = 42;
const int EYE_ANGRY_HEIGHT = 20;

void setup() {
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
}

void startAngry() {
  angryActive = true;

  roboEyes.setIdleMode(OFF);
  roboEyes.setAutoblinker(ON, 2, 1);   // fast, irritated blinking

  roboEyes.setMood(ANGRY);
  roboEyes.setPosition(DEFAULT);

  // Sharp angry eyes
  roboEyes.setWidth(EYE_ANGRY_WIDTH, EYE_ANGRY_WIDTH);
  roboEyes.setHeight(EYE_ANGRY_HEIGHT, EYE_ANGRY_HEIGHT);
}

void stopAngry() {
  angryActive = false;

  roboEyes.setMood(ANGRY);
  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2);
}

void loop() {
  roboEyes.update();

  if (obstacleDetected && !angryActive) {
    startAngry(); // nothing else runs
  }

  if (!obstacleDetected && angryActive){
    stopAngry();
  }

}
