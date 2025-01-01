#include <NTPClient.h>               // Include NTPClient library to get time from NTP servers
#include <ESP8266WiFi.h>              // Include ESP8266 WiFi library to connect to WiFi
#include <WiFiUdp.h>                  // Include WiFi UDP library for NTP communication
#include <Wire.h>                     // Include Wire library for I2C communication
#include <LiquidCrystal_I2C.h>        // Include LiquidCrystal_I2C library to control the LCD screen
#include <TimeLib.h>                  // Include TimeLib library for easier date handling
#include <BlynkSimpleEsp8266.h>       // Include Blynk library to connect to the Blynk cloud
#include <Servo.h>                    // Include Servo library to control the servo motor

const char *ssid = " ";    // Wi-Fi SSID (network name)
const char *password = " "; // Wi-Fi password
#define BLYNK_AUTH_TOKEN " " // Blynk Auth Token for connecting to Blynk app

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Create an object for the LCD with I2C address 0x27 and 16x2 size
WiFiUDP ntpUDP;                      // Create a UDP object for NTP communication
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // Create NTP client object with UTC+5:30 time zone
Servo myServo;                       // Create Servo object to control the servo motor

#define BUZZER_PIN2  14              // Define GPIO pin 14 for Buzzer 2
#define MOTION_PIN   12              // Define GPIO pin 12 for IR motion sensor
#define SERVO_PIN    13              // Define GPIO pin 13 for Servo motor

unsigned long lastMotionTime = 0;    // Variable to store last motion detection time
const unsigned long debounceDelay = 500; // Set debounce delay to 500ms for motion sensor
bool irActive = false;               // Boolean to track the state of the IR sensor (active or not)
bool buzzerState2 = LOW;             // Boolean to store the state of Buzzer 2

BlynkTimer timer;                    // Create Blynk timer for periodic tasks
String arr_days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; // Array for days of the week

// Blynk virtual pins to control the devices from the Blynk app
BLYNK_WRITE(V2) {                   // Blynk virtual pin V2 is for controlling Buzzer 2
  buzzerState2 = param.asInt();     // Get buzzer state from Blynk app
  digitalWrite(BUZZER_PIN2, buzzerState2); // Set Buzzer 2 state based on received value
}

BLYNK_WRITE(V3) {                   // Blynk virtual pin V3 is for controlling the Servo motor
  int servoPosition = param.asInt(); // Get servo position from Blynk app
  myServo.write(servoPosition);      // Set servo position based on received value
}

BLYNK_WRITE(V4) {                   // Blynk virtual pin V4 controls both LED and Buzzer 2 (not used for LED)
  int state = param.asInt();         // Get state value from Blynk app
  digitalWrite(BUZZER_PIN2, buzzerState2); // Set Buzzer 2 state based on received value
}

BLYNK_WRITE(V5) {                   // Blynk virtual pin V5 updates the IR sensor status
  if (irActive) {                   // If motion is detected
    Blynk.virtualWrite(V5, 1);       // Send "1" to Blynk app to indicate motion detected
  } else {
    Blynk.virtualWrite(V5, 0);       // Send "0" to Blynk app if no motion detected
  }
}

void updateLCD(String line1, String line2) {  // Function to update the LCD screen
  lcd.clear();                             // Clear the LCD screen
  lcd.setCursor(0, 0);                     // Set cursor to the beginning of the first line
  lcd.print(line1);                        // Print first line on LCD
  lcd.setCursor(0, 1);                     // Set cursor to the beginning of the second line
  lcd.print(line2);                        // Print second line on LCD
  // Blynk.virtualWrite(V6, line1);           // Update Blynk LCD widget with first line
  // Blynk.virtualWrite(V7, line2);           // Update Blynk LCD widget with second line
}

unsigned long lastNotificationTime = 0;     // Variable to track the time of last notification
const unsigned long notificationCooldown = 5000; // Cooldown time between notifications (5 seconds)

int motionDetectionCount = 0;              // Counter to track the number of motion detections

void handleMotionDetection() {            // Function to handle motion detection from the IR sensor
  int motionState = digitalRead(MOTION_PIN); // Read motion sensor input
  unsigned long currentTime = millis();    // Get the current time for debouncing

  if (motionState == LOW && (currentTime - lastMotionTime > debounceDelay)) {  // Motion detected
    if (!buzzerState2) {
      digitalWrite(BUZZER_PIN2, HIGH);    // Turn on Buzzer 2 when motion is detected
    }

    updateLCD(" Motion Detected", " Remainder");  // Update LCD with motion detected message
    irActive = true;                        // Set IR sensor status to active

    motionDetectionCount++;                 // Increment motion detection counter

    // Move servo based on motion detection count
    if (motionDetectionCount == 1) {
      myServo.write(90);                  // Move servo to 90 degrees
      Blynk.virtualWrite(V3, 90);          // Update servo position in Blynk app
    } else if (motionDetectionCount == 2) {
      myServo.write(180);                 // Move servo to 180 degrees
      Blynk.virtualWrite(V3, 180);         // Update servo position in Blynk app
    } else if (motionDetectionCount == 3) {
      myServo.write(270);                 // Move servo to 270 degrees
      Blynk.virtualWrite(V3, 270);         // Update servo position in Blynk app
    }

    if (motionDetectionCount >= 3) {       // Reset servo position after 3 detections
      motionDetectionCount = 0;            // Reset motion detection counter
      myServo.write(0);                    // Move servo back to 0 degrees
      Blynk.virtualWrite(V3, 0);            // Update servo position in Blynk app
    }

    Blynk.virtualWrite(V5, 1);              // Send "1" to Blynk to indicate motion detected

    lastMotionTime = currentTime;          // Update last motion detection time
  } else if (motionState == HIGH && irActive) { // If no motion detected
    irActive = false;                      // Set IR sensor status to inactive
  }
}

void setup() {
  Serial.begin(115200);                   // Start serial communication at 115200 baud rate
  pinMode(BUZZER_PIN2, OUTPUT);           // Set Buzzer 2 pin as output
  pinMode(MOTION_PIN, INPUT);             // Set motion sensor pin as input

  myServo.attach(SERVO_PIN);              // Attach the servo motor to the defined pin
  myServo.write(0);                       // Set servo to initial position (0 degrees)

  lcd.init();                             // Initialize the LCD screen
  lcd.backlight();                        // Turn on the LCD backlight
  lcd.setCursor(0, 0);                    // Set cursor to the first line
  lcd.print(" PHARMA SERVE");             // Print welcome message on LCD
  lcd.setCursor(0, 1);                    // Set cursor to the second line
  lcd.print(" G&H M&P");              // Print additional information on LCD

  WiFi.begin(ssid, password);             // Connect to Wi-Fi using the SSID and password
  while (WiFi.status() != WL_CONNECTED) { // Wait until Wi-Fi is connected
    delay(500);                           // Wait 500ms before checking again
    Serial.print(".");                    // Print dot to show Wi-Fi connection status
  }
  Serial.println("Connected to Wi-Fi.");  // Print connection success message

  timeClient.begin();                     // Start the NTP client to get the current time
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password, "blynk.cloud", 80);  // Start Blynk connection

  timer.setInterval(1000L, updateIRStatus);  // Set a timer to update IR status every second
}

void loop() {
  Blynk.run();                            // Run the Blynk loop to handle virtual pin events

  timeClient.update();                    // Update the NTP client time
  unsigned long epochTime = timeClient.getEpochTime(); // Get the current epoch time
  int hh = hour(epochTime);               // Extract hours from the epoch time
  int mm = minute(epochTime);             // Extract minutes from the epoch time
  int ss = second(epochTime);             // Extract seconds from the epoch time
  int dayOfWeek = weekday(epochTime);     // Extract the day of the week
  int currentDay = day(epochTime);        // Extract the current day of the month
  int currentMonth = month(epochTime);    // Extract the current month
  int currentYear = year(epochTime);      // Extract the current year

  handleMotionDetection();                // Call function to handle motion detection

  if (!irActive) {                        // If no motion is detected
    lcd.setCursor(1, 0);                  // Set cursor to the first line to display time
    if (hh >= 12) {                       // Check if it's PM
      if (hh > 12) hh -= 12;              // Convert to 12-hour format
      lcd.print(hh < 10 ? "0" : "");      // Print hour (with leading zero if necessary)
      lcd.print(hh);
      lcd.print(":");
      lcd.print(mm < 10 ? "0" : "");      // Print minutes (with leading zero if necessary)
      lcd.print(mm);
      lcd.print(":");
      lcd.print(ss < 10 ? "0" : "");      // Print seconds (with leading zero if necessary)
      lcd.print(ss);
      lcd.print(" PM");                   // Display "PM"
      lcd.print(" ");
      lcd.print(arr_days[dayOfWeek - 1]);  // Display day of the week
    } else {                              // If it's AM
      lcd.print(hh < 10 ? "0" : "");
      lcd.print(hh);
      lcd.print(":");
      lcd.print(mm < 10 ? "0" : "");
      lcd.print(mm);
      lcd.print(":");
      lcd.print(ss < 10 ? "0" : "");
      lcd.print(ss);
      lcd.print(" AM");                   // Display "AM"
      lcd.print(" ");
      lcd.print(arr_days[dayOfWeek - 1]);  // Display day of the week
    }

    lcd.setCursor(0, 1);                  // Set cursor to the second line to display date
    lcd.print(" ");
    lcd.print(currentDay < 10 ? "0" : ""); // Print the current day (with leading zero if necessary)
    lcd.print(currentDay);
    lcd.print("/");
    lcd.print(currentMonth < 10 ? "0" : ""); // Print the current month (with leading zero if necessary)
    lcd.print(currentMonth);
    lcd.print("/");
    lcd.print(currentYear);               // Print the current year
  }

  digitalWrite(BUZZER_PIN2, buzzerState2); // Control the buzzer based on its state
}

void updateIRStatus() {
  if (irActive) {
    Blynk.virtualWrite(V5, 1);              // Send "1" to Blynk if motion is detected
  } else {
    Blynk.virtualWrite(V5, 0);              // Send "0" to Blynk if no motion is detected
  }
}
