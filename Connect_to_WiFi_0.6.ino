/*
 *     M      M     AA    TTTTTTT  TTTTTTT  EEEEEEE M      M     AA     NN    N
 *     MM    MM    A  A      T        T     E       MM    MM    A  A    NN    N
 *     M M  M M   A    A     T        T     E       M M  M M   A    A   N  N  N
 *     M  MM  M   AAAAAA     T        T     EEEE    M  MM  M   AAAAAA   N  N  N - AUTOMATION
 *     M      M  A      A    T        T     E       M      M  A      A  N   N N 
 *     M      M  A      A    T        T     E       M      M  A      A  N    NN  
 *     M      M  A      A    T        T     EEEEEEE M      M  A      A  N    NN  
 *     
 *     
 *     Project    : Connect to WiFi, the Right Way
  *    Version    : 0.6
 *     Date       : 01-2021
 *     Written by : Ap Matteman
 */    

// #include "WiFi.h"      //FOR ESP32
#include <ESP8266WiFi.h>  // FOR ESP8266

const char* YourWiFiSSID = "YourSSID";
const char* YourWiFiPassword = "YourWiFiPassword";

int LEDRed = 5;
int LEDGreen = 2;

int WiFiTry = 0;

void setup() {
  
  Serial.begin(115200);
  delay(100);
  pinMode(LEDRed, OUTPUT);
  pinMode(LEDGreen, OUTPUT);
  ConnectWiFi();
}

void ConnectWiFi() { 
  //Connect with WiFi network
  
  WiFi.begin(YourWiFiSSID, YourWiFiPassword);
  Serial.println("Connecting to WiFi");
  //Try to connect to WiFi for 5 times
  while (WiFi.status() != WL_CONNECTED && WiFiTry < 5) {
    digitalWrite(LEDRed, HIGH);
    ++WiFiTry;
    Serial.print("WiFi status: ");; Serial.println(WiFi.status());
    Serial.print("WiFiTry: ");; Serial.println(WiFiTry);
    delay(500);
    digitalWrite(LEDRed, LOW);
    delay(500);
  }
  Serial.println("");
  Serial.print("WiFiTry: ");; Serial.println(WiFiTry);
  WiFiTry = 0;
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
}

void loop() {
  //  Your Code here
  digitalWrite(LEDGreen, HIGH);
  delay(500);
  digitalWrite(LEDGreen, LOW);
  delay(500);
  Serial.println();
  Serial.println("Loop is working");

  
  // Check the WiFi connection and reconnect if lost.
  if (WiFi.status() != WL_CONNECTED) { 
    Serial.println("No WiFi Connection!");  
    ConnectWiFi();
  } else {
    Serial.println("WiFi Connection!");  
  }
 
  
}
