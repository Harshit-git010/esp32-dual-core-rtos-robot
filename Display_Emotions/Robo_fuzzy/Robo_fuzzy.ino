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

bool pickedUp = true;
bool fuzzyActive = false;
unsigned long fuzzyStartTime = 0;
const unsigned long FUZZY_DURATION = 2500; // ms

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

void startFuzzy() {
  fuzzyActive = true;
  fuzzyStartTime = millis();

  roboEyes.setIdleMode(OFF);
  roboEyes.setAutoblinker(ON, 1, 1);   // fast blinking
  roboEyes.setMood(TIRED);

  roboEyes.setHFlicker(ON, 2);
  roboEyes.setVFlicker(ON, 2);

  roboEyes.anim_confused();  // one-shot animation
}

void updateFuzzy() {
  if (!fuzzyActive) return;

  if (millis() - fuzzyStartTime > FUZZY_DURATION) {
    stopFuzzy();
  }
  pickedUp = true;
}

void stopFuzzy() {
  fuzzyActive = false;

  roboEyes.setHFlicker(OFF, 0);
  roboEyes.setVFlicker(OFF, 0);

  roboEyes.setMood(DEFAULT);
  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2);
}

void loop() {
  roboEyes.update();   // always

  // Trigger fuzzy once
  if (pickedUp && !fuzzyActive) {
    startFuzzy();
    pickedUp = false;
  }

  updateFuzzy();
}
