# 🤖 ESP32 Dual-Core RTOS Robotics Platform

Real-time embedded robotics system built on ESP32 (Dual Core) using FreeRTOS, focused on structured task separation, sensor integration, and coordinated motion control.
Designed to explore deterministic scheduling and hardware–software co-design in robotics systems.

#What’s Inside:

#Legged Robot
Servo-based walking system
IR edge detection
Ultrasonic distance sensing
MPU6050 motion-triggered behavior
OLED display rendering
Dual-core FreeRTOS task separation

#Wheel Robot
Differential drive control
PWM motor actuation (TB6612FNG)
Obstacle detection
Modular RTOS-based firmware

#System Architecture
Core 1 (APP CPU) → Sensor Tasks + Motion Control
Core 0 (PRO CPU) → Display Task + Behavior Engine
~100 Hz task loop
Non-blocking RTOS scheduling
I2C + PWM communication handling
Architecture diagram available in /docs.

Project Version History:
ROBO LEGS

Version 1 :  Basic movement 
Version 2 : Improved moment 
Version 3 : code with minimal loops, pure logic, instant movement 
Version 4 : Human/realistic logic 
Version 5 : Continuous walk chatgpt 
Version 6 : Robot moving correctly 

ROBO SENSORS
Version 1 : Basic sensor monitoring 

Emotions 
Version 1 : Basic emotion display 

Integration 
Version 1 : Basic integration of all components 
Version 2 : Integration according to the wheels, all sensors are OK, but oled blinking problem

Remove all clear.display and display.display
Also delay() [optional]
Version 3 : Flickering stopped but eyes wobbling 
Version 4 : working ✓✓

#Tech Stack
ESP32 • FreeRTOS • MPU6050 • HC-SR04 • MG90S Servos • TB6612FNG • OLED (I2C)

#Key Focus Areas
Real-time task scheduling
Embedded motion control
Sensor-driven state machines
Power stability under dynamic load
Structured firmware architecture

#How to Run
Install ESP32 board support in Arduino IDE
Install required libraries (see below)
Upload firmware to ESP32 Dev Module
Dependencies are managed via Arduino Library Manager.

#Author
Harshit Mahajan
Embedded Systems • Robotics • RTOS
