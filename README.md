# 🤖 ESP32 Dual-Core RTOS Robotics Platform

Real-time embedded robotics system built on ESP32 (Dual Core) using FreeRTOS, focused on structured task separation, sensor integration, and coordinated motion control. <br>
Designed to explore deterministic scheduling and hardware–software co-design in robotics systems.<br>

# What’s Inside:

# Legged Robot<br>
Servo-based walking system	<br>
IR edge detection	<br>
Ultrasonic distance sensing<br>
MPU6050 motion-triggered behavior<br>
OLED display rendering<br>
Dual-core FreeRTOS task separation<br>

# Wheel Robot<br>
Differential drive control<br>
PWM motor actuation (TB6612FNG)<br>
Obstacle detection<br>
Modular RTOS-based firmware<br>

# System Architecture<br>
Core 1 (APP CPU) → Sensor Tasks + Motion Control<br>
Core 0 (PRO CPU) → Display Task + Behavior Engine<br>
~100 Hz task loop<br>
Non-blocking RTOS scheduling<br>
I2C + PWM communication handling<br>
Architecture diagram available in /docs.<br>

# Project Version History:<br>
ROBO LEGS<br><br>

Version 1 :  Basic movement <br>
Version 2 : Improved moment <br>
Version 3 : code with minimal loops, pure logic, instant movement <br>
Version 4 : Human/realistic logic <br>
Version 5 : Continuous walk chatgpt <br>
Version 6 : Robot moving correctly <br><br>

ROBO SENSORS<br>
Version 1 : Basic sensor monitoring <br><br>

Emotions <br>
Version 1 : Basic emotion display <br><br>

Integration <br>
Version 1 : Basic integration of all components <br>
Version 2 : Integration according to the wheels, all sensors are OK, but oled blinking problem<br><br>

Remove all clear.display and display.display<br>
Also delay() [optional]<br>
Version 3 : Flickering stopped but eyes wobbling <br>
Version 4 : working ✓✓<br><br>

## Tech Stack<br>
ESP32 • FreeRTOS • MPU6050 • HC-SR04 • MG90S Servos • TB6612FNG • OLED (I2C)<br><br>

# Key Focus Areas
Real-time task scheduling<br>
Embedded motion control<br>
Sensor-driven state machines<br>
Power stability under dynamic load<br>
Structured firmware architecture<br>

# How to Run<br>
Install ESP32 board support in Arduino IDE<br>
Install required libraries (see below)<br>
Upload firmware to ESP32 Dev Module<br>
Dependencies are managed via Arduino Library Manager.<br>

# Author<br>
Harshit Mahajan<br>
Embedded Systems • Robotics • RTOS<br>
