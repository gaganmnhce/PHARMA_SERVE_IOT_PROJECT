 # PHARMA_SERVE_IOT_PROJECT

# PHARMA SERVE: Medicine Pill Drop System with Blynk , NodeMCU Esp8266 and Arduino

# Overview

PHARMA SERVE is an automation system built using the ESP8266 microcontroller to detect motion via a PIR sensor, control a servo motor, and trigger a buzzer for alerts. The system displays the time and date on an LCD screen, while also allowing real-time monitoring and control via the Blynk app. Time synchronization is handled through an NTP server, ensuring accurate time is shown on the LCD display.

# Components Used

ESP8266 Wi-Fi Module
PIR Motion Sensor
Servo Motor
Buzzer
LCD (16x2) Display with I2C
Blynk App (for remote control)
NTP Server (for time synchronization)

# Features

Motion Detection: Detects motion with the PIR sensor.
Servo Motor Control: Moves the servo to different angles based on motion detection.
Buzzer Activation: Turns on the buzzer when motion is detected.
Remote Monitoring: Control and monitor the system via the Blynk app.
Time Display: Shows real-time time and date fetched from the NTP server on the LCD.
App Feedback: Displays motion detection status and allows remote control of the buzzer and servo.

# Pin Connections

PIR Motion Sensor: GPIO 12
Servo Motor: GPIO 13
Buzzer: GPIO 14
LCD Display: I2C connection (Address 0x27, 16x2 size)

# Libraries Used
ESP8266WiFi.h - For connecting to Wi-Fi.
BlynkSimpleEsp8266.h - For integrating with the Blynk app.
LiquidCrystal_I2C.h - For controlling the LCD.
Servo.h - For controlling the servo motor.
NTPClient.h - For synchronizing the time from an NTP server.
WiFiUdp.h - For handling UDP communication for NTP.

# Blynk Setup

Create a project on the Blynk app.
Obtain the Auth Token from the Blynk app and replace it in the code (#define BLYNK_AUTH_TOKEN).
Add widgets (sliders, buttons, etc.) for controlling the servo motor and buzzer in the Blynk app:
V2: Controls Buzzer.
V3: Controls Servo Position.
V4: Controls Buzzer.
V5: Displays motion detection status.

# Steps to Run

Connect hardware: Follow the pin connections to wire all components to the ESP8266.
Install libraries: Ensure all required libraries are installed in the Arduino IDE.
Upload code: Upload the provided code to your ESP8266 board using the Arduino IDE.
Connect to Wi-Fi: Ensure your ESP8266 connects to the Wi-Fi network.
Start Blynk: Open the Blynk app on your smartphone, and begin controlling the system remotely.

# Code Explanation

The ESP8266 connects to your Wi-Fi network and fetches time from an NTP server.
Motion detection is handled by the PIR sensor, which activates the servo motor and buzzer based on the detected motion.
The LCD shows the current time and motion detection status.
The Blynk app provides control over the servo and buzzer, while also offering real-time updates about the system.

# Troubleshooting
Double-check the Wi-Fi credentials in the code.
Ensure the servo is properly connected to the correct GPIO pin.
If the LCD does not display, confirm the I2C wiring and address.
If the Blynk app does not respond, verify the Auth Token is correctly set.

# License
This project is open-source, and you can modify it for your personal use.

