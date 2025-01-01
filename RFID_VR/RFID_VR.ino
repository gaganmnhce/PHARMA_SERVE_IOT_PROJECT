#include <SPI.h>            // Include the SPI library for communication with the MFRC522 RFID module.
#include <MFRC522.h>        // Include the MFRC522 library to work with the RFID reader.

#define SS_PIN 10           // Define the Slave Select pin for the RFID module (used for SPI communication).
#define RST_PIN 9           // Define the Reset pin for the MFRC522 RFID module.
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create an instance of the MFRC522 class with the defined SS and RST pins.

#define BUZZER_PIN A0       // Define the pin for the buzzer (used for feedback on access).
#define REC 2               // Define the pin for recording (connected to the ISD1820 voice recorder).
#define PLAY_E 3            // Define the pin for edge-triggered playback (connected to ISD1820).
#define PLAY_L 4            // Define the pin for looped playback (connected to ISD1820).
#define FT 5                // Define the pin for feed-through (disables microphone recording during playback).

#define playTime 5000       // Define the playback duration (5 seconds).
#define recordTime 3000     // Define the recording duration (3 seconds).
#define playLTime 900       // Define the loop playback duration (0.9 seconds).

void setup() 
{
  pinMode(BUZZER_PIN, OUTPUT);  // Set the buzzer pin as output to control the buzzer.
  pinMode(REC, OUTPUT);         // Set the REC pin as output for controlling the ISD1820 recording function.
  pinMode(PLAY_L, OUTPUT);      // Set the PLAY_L pin as output for controlling looped playback on ISD1820.
  pinMode(PLAY_E, OUTPUT);      // Set the PLAY_E pin as output for controlling edge-triggered playback.
  pinMode(FT, OUTPUT);          // Set the FT pin as output for controlling the feed-through function (disables mic recording).

  Serial.begin(9600);           // Start serial communication at 9600 baud rate to communicate with the serial monitor.
  SPI.begin();                  // Initialize SPI communication for the MFRC522 module.
  mfrc522.PCD_Init();           // Initialize the MFRC522 module (starts RFID reader).

  Serial.println("Approximate your card to the reader...");  // Display a message to indicate that the reader is ready.
  Serial.println();  // Print a blank line for readability.
}

void loop() 
{
  if (!mfrc522.PICC_IsNewCardPresent()) {  // Check if a new RFID card is present.
    return;  // If no card is detected, exit the loop and wait for the next card.
  }
  
  if (!mfrc522.PICC_ReadCardSerial()) {  // Try to read the card's serial number.
    return;  // If the card serial number is not read successfully, exit the loop and wait for the next card.
  }
  
  Serial.print("UID tag :");  // Print the "UID tag" message to indicate the UID of the detected RFID card will be printed.
  String content = "";  // Create an empty string to hold the card's UID.

  // Loop through the UID bytes of the card and convert them to a hexadecimal string for display.
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));  // Add leading zeros to the UID bytes for formatting.
    content.concat(String(mfrc522.uid.uidByte[i], HEX));  // Convert each byte to hexadecimal and append it to the content string.
  }
  
  content.toUpperCase();  // Convert the content string to uppercase (for uniformity).
  Serial.println(content);  // Print the final UID to the serial monitor.

  if (content.substring(1) == "  ") {  // Check if the detected UID matches the authorized UID.
    Serial.println("Authorized access");  // Print a message indicating authorized access.
    Serial.println();  // Print a blank line for readability.
    
    // Blink the buzzer to indicate successful access (authorized user).
    digitalWrite(BUZZER_PIN, HIGH);  // Turn the buzzer ON.
    delay(200);  // Wait for 200 milliseconds.
    digitalWrite(BUZZER_PIN, LOW);   // Turn the buzzer OFF.
    delay(200);  // Wait for 200 milliseconds.
    digitalWrite(BUZZER_PIN, HIGH);  // Turn the buzzer ON again.
    delay(200);  // Wait for 200 milliseconds.
    digitalWrite(BUZZER_PIN, LOW);   // Turn the buzzer OFF.
    delay(200);  // Wait for 200 milliseconds.
    digitalWrite(BUZZER_PIN, HIGH);  // Turn the buzzer ON again.
    delay(200);  // Wait for 200 milliseconds.
    digitalWrite(BUZZER_PIN, LOW);   // Turn the buzzer OFF.
    delay(200);  // Wait for 200 milliseconds.
    digitalWrite(BUZZER_PIN, HIGH);  // Turn the buzzer ON one more time.
    delay(200);  // Wait for 200 milliseconds.
    digitalWrite(BUZZER_PIN, LOW);   // Turn the buzzer OFF.
    delay(1000);  // Wait for 1 second before the next action.
  }
  else {  // If the detected UID does not match the authorized UID (i.e., unauthorized access).
    Serial.println("Access Denied");  // Print a message indicating denied access.
    Serial.println();  // Print a blank line for readability.
    
    // Play a "Denied" voice message using edge-triggered playback on the ISD1820.
    digitalWrite(PLAY_E, HIGH);  // Start the playback.
    delay(50);  // Wait for 50 milliseconds to allow playback to start.
    digitalWrite(PLAY_E, LOW);   // Stop the playback.
    delay(playTime);  // Wait for the duration of the "Denied" message playback.
  }
}
