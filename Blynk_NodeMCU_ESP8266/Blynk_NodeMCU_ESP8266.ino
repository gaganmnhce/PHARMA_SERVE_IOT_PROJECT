#include <NTPClient.h>                          // Library for using NTP (Network Time Protocol) to get the current time
#include <ESP8266WiFi.h>                         // Library to connect the ESP8266 to WiFi
#include <WiFiUdp.h>                             // Library for using UDP communication (used by NTP)
#include <Wire.h>                                // I2C communication library (needed for LCD display)
#include <LiquidCrystal_I2C.h>                   // Library for controlling an I2C-based LCD screen
#include <TimeLib.h>                             // Library for working with time (converts time to a readable format)
#include <BlynkSimpleEsp8266.h>                  // Library for Blynk app communication using the ESP8266
#include <Servo.h>                               // Library for controlling a Servo motor

// Wi-Fi credentials
const char *ssid = " ";               // Wi-Fi network name
const char *password = " ";           // Wi-Fi network password

// #define BLYNK_AUTH_TOKEN " " // Blynk Auth Token for connecting to Blynk app ours
#define BLYNK_AUTH_TOKEN " " // Blynk authentication token (used to connect the device to Blynk app)

LiquidCrystal_I2C lcd(0x27, 16, 2);             // Create an LCD object with the I2C address 0x27 (16x2 LCD)
WiFiUDP ntpUDP;                                // Create an object for UDP communication (for NTP)
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // Create a client object to request time from an NTP server (with time zone offset)
Servo myServo;                                 // Create a servo object to control a servo motor

// Pin definitions
#define BUZZER_PIN2  14                          // Buzzer connected to GPIO14 (D5)
#define MOTION_PIN   12                          // Motion sensor connected to GPIO12 (D6)
#define SERVO_PIN    13                          // Servo motor connected to GPIO13 (D7)
#define LED_PIN      0                           // LED connected to GPIO0 (D3) on ESP8266

// Variables for motion detection and debounce
unsigned long lastMotionTime = 0;               // Last time motion was detected
const unsigned long debounceDelay = 500;        // Delay time to debounce motion sensor
bool irActive = false;                          // Variable to track the motion sensor status
bool buzzerState2 = LOW;                        // Variable to control the buzzer state

BlynkTimer timer;                               // Timer object to manage periodic tasks
String arr_days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; // Array to convert day numbers to names

// Function to handle buzzer control via Blynk app
BLYNK_WRITE(V2) {                              // When V2 is updated in the Blynk app
  buzzerState2 = param.asInt();                 // Get the buzzer state from the app
  if (buzzerState2 == 1) {                      // If buzzer state is 1 (on)
    digitalWrite(BUZZER_PIN2, HIGH);            // Turn the buzzer on
    Blynk.logEvent("buzzer_on", "\nWarning!!! Critical Alert"); // Log an event to Blynk
  } else {                                      // If buzzer state is 0 (off)
    digitalWrite(BUZZER_PIN2, LOW);             // Turn the buzzer off
  }
}

// Function to control the servo motor via Blynk app
BLYNK_WRITE(V3) {                              // When V3 is updated in the Blynk app
  int servoPosition = param.asInt();            // Get the servo position from the app
  myServo.write(servoPosition);                 // Move the servo to the given position
  Blynk.virtualWrite(V3, servoPosition);        // Update the Blynk app with the new servo position
}

// Function to handle motion status display via Blynk app
BLYNK_WRITE(V5) {                              // When V5 is updated in the Blynk app
  if (irActive) {                               // If motion is detected
    Blynk.virtualWrite(V5, 1);                  // Send '1' to Blynk (motion active)
  } else {                                      // If no motion is detected
    Blynk.virtualWrite(V5, 0);                  // Send '0' to Blynk (no motion)
  }
}

// Function to control the LED via Blynk app (LED connected to D3 pin)
BLYNK_WRITE(V1) {                              // When V1 is updated in the Blynk app
  int ledState = param.asInt();                 // Get the LED state from the app
  if (ledState == 1) {                          // If LED state is 1 (on)
    digitalWrite(LED_PIN, HIGH);                // Turn the LED on
  } else {                                      // If LED state is 0 (off)
    digitalWrite(LED_PIN, LOW);                 // Turn the LED off
  }
}

// Function to update the LCD with custom messages
void updateLCD(String line1, String line2) {
  lcd.setCursor(0, 0);                         // Set cursor to the first line
  lcd.print(line1);                             // Print the first line on the LCD
  lcd.setCursor(0, 1);                         // Set cursor to the second line
  lcd.print(line2);                             // Print the second line on the LCD
}

// Variables for managing notifications and motion detection
unsigned long lastNotificationTime = 0;         // Last time a notification was sent
const unsigned long notificationCooldown = 5000; // Cooldown time between notifications
int motionDetectionCount = 0;                   // Counter for detected motion

// Function to handle motion detection
void handleMotionDetection() {
  int motionState = digitalRead(MOTION_PIN);    // Read the motion sensor state (HIGH or LOW)
  unsigned long currentTime = millis();         // Get the current time in milliseconds

  // If motion is detected (LOW) and debounce time has passed
  if (motionState == LOW && (currentTime - lastMotionTime > debounceDelay)) {
    if (!buzzerState2) {                        // If the buzzer is not active
      digitalWrite(BUZZER_PIN2, HIGH);          // Turn on the buzzer
    }

    updateLCD(" Motion Detected", " Remainder"); // Update the LCD with a message
    irActive = true;                            // Set the motion detected flag to true

    motionDetectionCount++;                     // Increment the motion detection counter

    // Move the servo motor based on motion detection count
    if (motionDetectionCount == 1) {
      myServo.write(90);                        // Move servo to 90 degrees
      Blynk.virtualWrite(V3, 90);                // Update Blynk with servo position
      Blynk.logEvent("tablet_1", "\nPill 1 is taken by patient \nServo moved to 90 degrees.");
    } else if (motionDetectionCount == 2) {
      myServo.write(180);                       // Move servo to 180 degrees
      Blynk.virtualWrite(V3, 180);               // Update Blynk with servo position
      Blynk.logEvent("tablet_2", "\nPill 2 is taken by patient \nServo moved to 180 degrees.");
    } else if (motionDetectionCount == 3) {
      myServo.write(270);                       // Move servo to 270 degrees
      Blynk.virtualWrite(V3, 270);               // Update Blynk with servo position
    }

    if (motionDetectionCount >= 3) {            // If 3 motions are detected
      motionDetectionCount = 0;                  // Reset motion count
      myServo.write(0);                          // Reset servo to initial position
      Blynk.virtualWrite(V3, 0);                  // Update Blynk with servo position
      Blynk.logEvent("system_ready", "\n2 Pills taken by patient \nMedicine is Empty!!! \nServo moved to 0 (Initial Position).");
    }

    Blynk.virtualWrite(V5, 1);                   // Update Blynk to show motion detected
    lastMotionTime = currentTime;                // Update last motion detection time
  } else if (motionState == HIGH && irActive) {
    irActive = false;                           // If motion stops, reset the motion detected flag
  }
}

// Setup function, runs once when the program starts
void setup() {
  Serial.begin(115200);                         // Start serial communication at 115200 baud
  pinMode(BUZZER_PIN2, OUTPUT);                 // Set the buzzer pin as an output
  pinMode(MOTION_PIN, INPUT);                   // Set the motion sensor pin as an input
  pinMode(LED_PIN, OUTPUT);                     // Set the LED pin as an output

  myServo.attach(SERVO_PIN);                    // Attach the servo motor to the specified pin
  myServo.write(0);                             // Set the servo position to 0 (initial)

  lcd.init();                                   // Initialize the LCD screen
  lcd.backlight();                              // Turn on the backlight
  lcd.setCursor(0, 0);                          // Set cursor to the first line
  lcd.print(" PHARMA SERVE");                   // Print the title on the LCD
  lcd.setCursor(0, 1);                          // Set cursor to the second line
  lcd.print(" G&H M&P");                        // Print the subtitle on the LCD

  WiFi.begin(ssid, password);                   // Connect to Wi-Fi using the provided credentials
  while (WiFi.status() != WL_CONNECTED) {       // Wait until the Wi-Fi is connected
    delay(500);                                 // Wait 500 ms between checks
    Serial.print(".");                          // Print a dot while connecting
  }
  Serial.println("Connected to Wi-Fi.");        // Print Wi-Fi connection status

  timeClient.begin();                           // Start the NTP client to get time from NTP server
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password, "blynk.cloud", 80); // Start the Blynk app connection

  timer.setInterval(1000L, updateIRStatus);     // Set an interval to update the IR status every 1 second
}

// Loop function, runs repeatedly
void loop() {
  Blynk.run();                                  // Run the Blynk app
  timeClient.update();                          // Update the time from the NTP server
  unsigned long epochTime = timeClient.getEpochTime();  // Get the current time in epoch format
  int hh = hour(epochTime);                     // Extract the hour from the epoch time
  int mm = minute(epochTime);                   // Extract the minute from the epoch time
  int ss = second(epochTime);                   // Extract the second from the epoch time
  int dayOfWeek = weekday(epochTime);           // Extract the day of the week from the epoch time
  int currentDay = day(epochTime);              // Extract the day of the month from the epoch time
  int currentMonth = month(epochTime);          // Extract the month from the epoch time
  int currentYear = year(epochTime);            // Extract the year from the epoch time

  handleMotionDetection();                      // Call the function to handle motion detection

  if (!irActive) {                              // If no motion is detected
    lcd.setCursor(1, 0);                        // Set cursor to the first line
    if (hh >= 12) {                             // If it is PM (after 12:00)
      if (hh > 12) hh -= 12;                    // Convert hours to 12-hour format
      lcd.print(hh < 10 ? "0" : "");            // Print hour with leading zero if needed
      lcd.print(hh);                            // Print hour
      lcd.print(":");                           // Print colon
      lcd.print(mm < 10 ? "0" : "");            // Print minute with leading zero if needed
      lcd.print(mm);                            // Print minute
      lcd.print(":");                           // Print colon
      lcd.print(ss < 10 ? "0" : "");            // Print second with leading zero if needed
      lcd.print(ss);                            // Print second
      lcd.print(" PM");                        // Print PM
      lcd.print(" ");                           // Add space
      lcd.print(arr_days[dayOfWeek - 1]);       // Print the day of the week
    } else {                                    // If it is AM (before 12:00)
      lcd.print(hh < 10 ? "0" : "");            // Print hour with leading zero if needed
      lcd.print(hh);                            // Print hour
      lcd.print(":");                           // Print colon
      lcd.print(mm < 10 ? "0" : "");            // Print minute with leading zero if needed
      lcd.print(mm);                            // Print minute
      lcd.print(":");                           // Print colon
      lcd.print(ss < 10 ? "0" : "");            // Print second with leading zero if needed
      lcd.print(ss);                            // Print second
      lcd.print(" AM");                        // Print AM
      lcd.print(" ");                           // Add space
      lcd.print(arr_days[dayOfWeek - 1]);       // Print the day of the week
    }

    lcd.setCursor(0, 1);                        // Set cursor to the second line
    lcd.print(" ");                             // Print space
    lcd.print(currentDay < 10 ? "0" : "");      // Print day with leading zero if needed
    lcd.print(currentDay);                      // Print the day
    lcd.print("/");                             // Print slash
    lcd.print(currentMonth < 10 ? "0" : "");    // Print month with leading zero if needed
    lcd.print(currentMonth);                    // Print the month
    lcd.print("/");                             // Print slash
    lcd.print(currentYear);                     // Print the year
  }

  digitalWrite(BUZZER_PIN2, buzzerState2);      // Update the buzzer state based on buzzerState2 variable
}

// Function to update the IR status in Blynk app
void updateIRStatus() {
  if (irActive) {                               // If motion is detected
    Blynk.virtualWrite(V5, 1);                   // Send '1' to Blynk (motion active)
  } else {                                      // If no motion is detected
    Blynk.virtualWrite(V5, 0);                   // Send '0' to Blynk (no motion)
  }
}
