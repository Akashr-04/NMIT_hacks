#include <ThingSpeak.h>

#include <WiFi.h>


const int piezoPin = 34; // Analog input pin (GPIO 34, doesn't support pull-up/down)
const int ledPin = 19; // Alert LED
const int buzzerPin = 18; // Alert Buzzer
const int motorPin = 5; // Vibration motor (via NPN transistor)
const int vibrationThreshold = 1000; // Adjust based on sensor output
const int detectionWindow = 3000; // Time window (ms) to check vibration

unsigned long lastCheck = 0;
bool sleepDetected = false;

WiFiClient client;

// Wi-Fi Credentials
const char* ssid = "Akash";
const char* password = "Akash@123";

// ThingSpeak Settings
const unsigned long channelID = 2965647;
const char* writeAPIKey = "R9XPTE5LPDQIZPTL";

void setup() {
Serial.begin(115200);

pinMode(ledPin, OUTPUT);
pinMode(buzzerPin, OUTPUT);
pinMode(motorPin, OUTPUT);

connectWiFi();

ThingSpeak.begin(client);
}

void loop() {
unsigned long currentMillis = millis();

int vibration = analogRead(piezoPin);
Serial.print("Vibration: ");
Serial.println(vibration);

if (vibration < vibrationThreshold) {
if (!sleepDetected && currentMillis - lastCheck > detectionWindow) {
triggerAlert();
sendToThingSpeak(vibration);
lastCheck = currentMillis;
sleepDetected = true;
}
} else {
sleepDetected = false;
}

delay(200);
}

void triggerAlert() {
digitalWrite(ledPin, HIGH);
digitalWrite(buzzerPin, HIGH);
digitalWrite(motorPin, HIGH);
delay(800);
digitalWrite(ledPin, LOW);
digitalWrite(buzzerPin, LOW);
digitalWrite(motorPin, LOW);
}

void sendToThingSpeak(int vibration) {
ThingSpeak.setField(1, vibration);
ThingSpeak.setField(2, millis() / 1000); // Time since power-on
int status = ThingSpeak.writeFields(channelID, writeAPIKey);
if (status == 200) {
Serial.println("Data sent to ThingSpeak");
} else {
Serial.print("ThingSpeak Error: ");
Serial.println(status);
}
}

void connectWiFi() {
Serial.print("Connecting to WiFi: ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("\nWiFi Connected!");
}