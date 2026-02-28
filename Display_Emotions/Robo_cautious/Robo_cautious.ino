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

// Eye size control
int eyeWidthCurrent = 36;
int eyeHeightCurrent = 36;

const int EYE_NORMAL = 36;
const int EYE_CAUTION = 24;   // smaller = cautious

unsigned long lastEyeUpdate = 0;
const unsigned long EYE_UPDATE_INTERVAL = 40; // ms

bool cautiousActive = false;

bool edgeDetected = true;

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

void updateCautiousEyes() {
  if (millis() - lastEyeUpdate < EYE_UPDATE_INTERVAL) return;
  lastEyeUpdate = millis();

  int targetSize = edgeDetected ? EYE_CAUTION : EYE_NORMAL;

  if (eyeWidthCurrent < targetSize) eyeWidthCurrent++;
  if (eyeWidthCurrent > targetSize) eyeWidthCurrent--;

  if (eyeHeightCurrent < targetSize) eyeHeightCurrent++;
  if (eyeHeightCurrent > targetSize) eyeHeightCurrent--;

  roboEyes.setWidth(eyeWidthCurrent, eyeWidthCurrent);
  roboEyes.setHeight(eyeHeightCurrent, eyeHeightCurrent);
}

void startCautious() {
  cautiousActive = true;

  roboEyes.setIdleMode(OFF);
  roboEyes.setAutoblinker(ON, 5, 1);
  roboEyes.setMood(TIRED);
  roboEyes.setPosition(S); // look down
}

void stopCautious() {
  cautiousActive = false;

  roboEyes.setPosition(DEFAULT);
  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2);
}

void loop() {
  roboEyes.update();

  // Enter cautious
  if (edgeDetected && !cautiousActive) {
    startCautious();
  }

  // Exit cautious
  if (!edgeDetected && cautiousActive) {
    stopCautious();
  }

  // Smooth eye size transition
  updateCautiousEyes();

}


