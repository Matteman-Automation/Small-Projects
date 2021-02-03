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
 *     Project    : Connect to MQTT, the Right Way
  *    Version    : 0.5
 *     Date       : 01-2021
 *     Written by : Ap Matteman
 */    

// #include "WiFi.h"      //FOR ESP32
#include <ESP8266WiFi.h>  // FOR ESP8266
#include <PubSubClient.h> // FOR MQTT

WiFiClient espClient;
PubSubClient client(espClient); // MQTT Client

const char* YourWiFiSSID = "YourSSID";
const char* YourWiFiPassword = "YourWiFiPassword";
const char* mqtt_server = "192.168.10.10";
const char* mqtt_user = "MQTTHome";
const char* mqtt_password = "Home@48";

int LEDRed = 16;    // D0
int LEDBlue = 5;    // D1
int LEDGreen1 = 2;  // D4
int LEDGreen2 = 14; // D5
int LEDGreen3 = 15; // D8

int WiFiTry = 0;          // times the WiFi is not available
int MQTTTry = 0;          // times the MQTT server is not available. 

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(LEDRed, OUTPUT);
  pinMode(LEDBlue, OUTPUT);
  pinMode(LEDGreen1, OUTPUT);
  pinMode(LEDGreen2, OUTPUT);
  pinMode(LEDGreen3, OUTPUT);
  ConnectWiFi();
  client.setServer(mqtt_server, 1883);
  ConnectMQTT();
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

void ConnectMQTT() {
  // Recconnect to the MQTT server
  Serial.println("Attempting Reconnect MQTT connection...");
  // Attempt to connect
  while (!client.connect(mqtt_server, mqtt_user, mqtt_password) && MQTTTry < 5 ) {
    digitalWrite(LEDBlue, HIGH);
    ++MQTTTry;
    Serial.println("No MQTT, Retry");
    delay(500);
    digitalWrite(LEDBlue, LOW);
    delay(500);    
  } 
  MQTTTry = 0;
  Serial.println("MQTT end loop.");
}

void loop() {
  //  Your Code here
   
  digitalWrite(LEDGreen1, HIGH);
  client.publish("Office/test/number", String(1).c_str()); 
  client.publish("Office/test/text", "één"); 
  client.publish("Office/test/switch", "aan");
  delay(2000);
  digitalWrite(LEDGreen2, HIGH);
  client.publish("Office/test/number", String(2).c_str()); 
  client.publish("Office/test/text", "twee"); 
  client.publish("Office/test/switch", "uit");
  delay(2000);
  digitalWrite(LEDGreen3, HIGH);
  client.publish("Office/test/number", String(3).c_str()); 
  client.publish("Office/test/text", "drie"); 
  client.publish("Office/test/switch", "aan");
  delay(2000);
  digitalWrite(LEDGreen1, LOW);
  digitalWrite(LEDGreen2, LOW);
  digitalWrite(LEDGreen3, LOW);
  client.publish("Office/test/number", String(0).c_str()); 
  client.publish("Office/test/text", "nul"); 
  client.publish("Office/test/switch", "uit"); 
  delay(2000);
  
  Serial.println();
  Serial.println("Loop is working");

  client.loop();  
  // Check the WiFi connection and reconnect if lost.
  if (WiFi.status() != WL_CONNECTED) { 
    Serial.println("No WiFi Connection!");  
    ConnectWiFi();
  } else {
    Serial.println("WiFi Connected");
    if (!client.connected()) {
      Serial.println("No MQTT Connection!");  
      digitalWrite(LEDGreen3, HIGH);
      ConnectMQTT();
      digitalWrite(LEDGreen3, LOW);
    }
    else {
      // Send Data to MQTT Server
      Serial.print("Connected status: "); Serial.println(client.connected());
      
      
    }
  }
}
