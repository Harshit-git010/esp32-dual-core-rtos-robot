#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FluxGarage_RoboEyes.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

RoboEyes<Adafruit_SSD1306> roboEyes(display);

#define LED_PIN 26

void blinkLight(void *pvParameters){
  for(;;){
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(500)); // for delays we use this
    digitalWrite(LED_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(500)); // for delays we use this

  }
} 

void oledDisplay(void *pvParameters) {
  while (true) {
    roboEyes.update();
    vTaskDelay(10 / portTICK_PERIOD_MS); // ~100 FPS ,Try 5 if it get slow
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(); 

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  pinMode(LED_PIN, OUTPUT);

  xTaskCreatePinnedToCore(
    blinkLight,
    "Light",
    4096,
    NULL,
    1, // Task Priority
    NULL,
    0   // CORE 0
  );

  xTaskCreatePinnedToCore(
    oledDisplay,
    "Oled",
    4096,
    NULL,
    2,  // Task Priority
    NULL,
    1   // CORE 1
  );

  roboEyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 100);
  roboEyes.setAutoblinker(ON, 3, 2);
  roboEyes.setIdleMode(ON, 2, 2); 
}


void loop() {
  // empty — RTOS is running everything
}
