# PHARMA_SERVE_IOT_PROJECT

## RFID Access Control System: Code Execution and Explanation

# Overview
This project implements an RFID-based access control system using an MFRC522 RFID reader, ISD1820 voice recorder, and an Arduino. The system reads RFID tags, compares the tag's UID with an authorized UID, and provides feedback via a buzzer or voice playback. When an authorized tag is scanned, the system signals access granted. If an unauthorized tag is scanned, access is denied.

# Components Used:
MFRC522 RFID Reader: Reads RFID tags.
Arduino Board: Controls the logic and interfaces with the RFID reader and other components.
ISD1820 Voice Recorder: Plays audio messages for feedback (e.g., "Access Denied" or "Access Granted").
Buzzer: Provides additional feedback for unauthorized access.
Wires and Breadboard: For circuit connections.

# Required Libraries:
SPI.h: This library allows the Arduino to communicate with the MFRC522 module using SPI protocol.
MFRC522.h: This library provides functions for interacting with the MFRC522 RFID module.

# Circuit Connections:
MFRC522 RFID Reader:
SS_PIN (10) -> Arduino Pin 10
RST_PIN (9) -> Arduino Pin 9
SCK, MISO, MOSI pins -> Arduino SPI pins
ISD1820 Voice Recorder:
REC (2), PLAY_E (3), PLAY_L (4), FT (5) connected to appropriate control pins on the ISD1820.
VCC, GND to power pins.
Buzzer: Connected to A0 pin for feedback.

# Code Explanation
The following steps describe how the code works, what happens at each stage, and how the components interact.

# 1. Header Files and Pin Definitions

#include <SPI.h>            
#include <MFRC522.h>        

SPI.h is required for communication with the MFRC522 RFID module.
MFRC522.h provides the necessary functions to interact with the MFRC522 module.

#define SS_PIN 10          
#define RST_PIN 9           
MFRC522 mfrc522(SS_PIN, RST_PIN); 

Defines the Slave Select (SS) and Reset (RST) pins for the MFRC522 RFID module.
MFRC522 mfrc522(SS_PIN, RST_PIN) initializes the RFID module to communicate with the Arduino via the defined pins.

# 2. Pin Initialization in setup()

void setup() 
{
  pinMode(BUZZER_PIN, OUTPUT);  
  pinMode(REC, OUTPUT);         
  pinMode(PLAY_L, OUTPUT);      
  pinMode(PLAY_E, OUTPUT);      
  pinMode(FT, OUTPUT);          
  Serial.begin(9600);           
  SPI.begin();                  
  mfrc522.PCD_Init();           
  Serial.println("Approximate your card to the reader...");
}

The setup() function initializes the pins, sets the mode for the buzzer and ISD1820 voice recorder pins to OUTPUT, and begins the serial communication at 9600 baud rate for debugging.
SPI.begin() initializes the SPI bus, allowing communication with the MFRC522.
mfrc522.PCD_Init() initializes the MFRC522 module.

# 3. Detecting and Reading RFID Cards in loop()

if (!mfrc522.PICC_IsNewCardPresent()) {
  return;
}

This line checks if a new RFID card is detected. If no new card is present, it skips the rest of the code inside loop() and waits for a card.

if (!mfrc522.PICC_ReadCardSerial()) {
  return;
}

This checks if the card's UID is successfully read. If it fails, it exits and waits for the next card.

Serial.print("UID tag :");
String content = "";
for (byte i = 0; i < mfrc522.uid.size; i++) {
  content.concat(String(mfrc522.uid.uidByte[i], HEX));
}
content.toUpperCase();
Serial.println(content);

This section reads the UID of the RFID card byte-by-byte and concatenates them into a string, then prints it to the serial monitor.

# 4. Access Control Check

if (content.substring(1) == " ") { 
  Serial.println("Authorized access");
}
else {
  Serial.println("Access Denied");
}

Authorized Access Check: The UID is checked against a predefined authorized UID ("E9 21 0F E3"). If the UID matches, access is granted, and a corresponding message is printed.
If the UID is unauthorized, access is denied.

# 5. Feedback Mechanism
Authorized Access Feedback
digitalWrite(BUZZER_PIN, HIGH);
delay(200);
digitalWrite(BUZZER_PIN, LOW);
delay(200);

For authorized access, the system uses the buzzer to provide feedback by turning it on and off in intervals.
The buzzer blinks several times to indicate access is granted. The delay of 200 ms ensures the buzzer is on for a brief period before turning off.

Unauthorized Access Feedback

digitalWrite(PLAY_E, HIGH);
delay(50);
digitalWrite(PLAY_E, LOW);
delay(playTime);

When unauthorized access is detected, the ISD1820 voice recorder plays a "Denied" message. The PLAY_E pin is set HIGH to trigger edge playback on the ISD1820, and then set LOW to stop the playback.
The message plays for the time duration specified by playTime (5 seconds).

# 6. Component Interaction

RFID Reader:

When a card is placed near the MFRC522 RFID reader, it detects the card and reads its UID.
The UID is compared to the authorized UID list stored in the code.

Authorized Access:

If the card's UID matches the authorized UID (E9 21 0F E3), the buzzer blinks several times, signaling "Access Granted."
Optionally, a voice message saying "Access" can be triggered using the ISD1820 (this part is currently commented out in the code).

Unauthorized Access:

If the UID does not match, the ISD1820 plays a "Denied" message, and feedback is provided using the buzzer.

# 7. Loop Continuation
The loop() function repeats itself continuously, checking for new RFID cards and taking the appropriate action for each card's UID.
Execution Flow Summary

Startup:

The system initializes the RFID reader and sets up the communication.
The serial monitor displays a prompt: "Approximate your card to the reader..."

Card Detection:

When an RFID card is presented, the MFRC522 reads the UID.
The UID is displayed on the serial monitor for debugging.

Access Control Check:

The UID is checked against the predefined authorized UID. If it matches, the system grants access by blinking the buzzer. If not, access is denied, and a voice message is played.

Continuous Loop:

The system continues to check for RFID cards in the loop, granting or denying access as necessary.

# Conclusion
This system provides basic access control using RFID technology. It can be expanded further by adding more authorized UIDs, integrating a display, or improving the voice feedback. The code and hardware components interact seamlessly to provide a user-friendly access control solution.







