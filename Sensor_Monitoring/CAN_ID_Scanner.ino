#include <SPI.h>
#include <mcp_can.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// MCP2515 CAN module setup
const int CAN_CS_PIN = 10;
const int CAN_INT_PIN = 2;  // Define the CAN interrupt pin
MCP_CAN CAN(CAN_CS_PIN);

volatile bool messageAvailable = false; // Flag for message availability

void setup() {
    Serial.begin(115200);

    if (CAN.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
        Serial.println("CAN bus initialized");
    } else {
        Serial.println("CAN bus failed to initialize");
        while (1);
    }

    pinMode(CAN_INT_PIN, INPUT); // Set CAN interrupt pin as input
    attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), canMessageReceived, FALLING); // Attach ISR

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("OLED initialization failed"));
        while (1);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("CAN Bus Sniffing...");
    display.display();
}

void loop() {
    if (messageAvailable) {
        messageAvailable = false; // Reset flag
        unsigned long canId;
        unsigned char len = 0;
        unsigned char buf[8];

        CAN.readMsgBuf(&canId, &len, buf);

        Serial.print("CAN ID: 0x");
        Serial.print(canId, HEX);
        Serial.print(" Data: ");
        for (int i = 0; i < len; i++) {
            Serial.print(buf[i], HEX);
            Serial.print(" ");
        }
        Serial.println();

        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("CAN ID: 0x");
        display.print(canId, HEX);
        display.setCursor(0, 10);
        display.print("Data: ");
        for (int i = 0; i < len; i++) {
            display.print(buf[i], HEX);
            display.print(" ");
        }
        display.display();
    }
}

void canMessageReceived() {
    messageAvailable = true; // Set flag when a message is received
}
