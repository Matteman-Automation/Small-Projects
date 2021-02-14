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

int LEDGreen1 = 14; // D5
int LEDGreen2 = 15; // D8

byte Light_On = 0;
long LastRead = 0;
long Time_Light_On = 5000;

int WiFiTry = 0;          // times the WiFi is not available
int MQTTTry = 0;          // times the MQTT server is not available. 

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(LEDGreen1, OUTPUT);
  pinMode(LEDGreen2, OUTPUT);
  ConnectWiFi();
  client.setServer(mqtt_server, 1883);
  ConnectMQTT();

  client.subscribe("Office/test/Green1");
  client.subscribe("Office/test/Green2");
  
  client.setCallback(callback);
}

void ConnectWiFi() { 
  //Connect with WiFi network
  WiFi.begin(YourWiFiSSID, YourWiFiPassword);
  Serial.println("Connecting to WiFi");
  //Try to connect to WiFi for 5 times
  while (WiFi.status() != WL_CONNECTED && WiFiTry < 5) {
    ++WiFiTry;
    Serial.print("WiFi status: ");; Serial.println(WiFi.status());
    Serial.print("WiFiTry: ");; Serial.println(WiFiTry);
    delay(1000);
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
    ++MQTTTry;
    Serial.println("No MQTT, Retry");
    delay(1000);    
  } 
  MQTTTry = 0;
  Serial.println("MQTT end loop.");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT Message: ");
  Serial.println(topic);
  
  if (strcmp(topic,"Office/test/Green1")==0){
    // Green LED 1
    Serial.println("Signal for Green1");
    for (int i=0;i<length;i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    if(payload[0] == '1'){
      digitalWrite(LEDGreen1, HIGH);
      client.publish("Office/test/Green1/set", "1"); 
    } else {
      digitalWrite(LEDGreen1, LOW);
      client.publish("Office/test/Green1/set", "0"); 
    }
      
    
  }
  if (strcmp(topic,"Office/test/Green2")==0){
    // Green LED 2
    Serial.println("Signal for Green2");
    
    String Status = "";
    for (int i=0;i<length;i++) {
      Status+= (char)payload[i];
    }
    Serial.print("Status: ");
    Serial.println(Status);
    if(Status == "aan") {
      Light_On = 1;
      LastRead = millis();
    } else {
      Light_On = 0;
      LastRead = Time_Light_On + millis();
    }
  }  
}


void loop() {

  client.loop();  
  

  long now = millis();
  if (now - LastRead > Time_Light_On && Light_On == 1)  {
    // Time to turn the lights off
    client.publish("Office/test/Green2", "uit"); 
  } 


  if (Light_On == 1) {
    // GreenLED2 On
    digitalWrite(LEDGreen2, HIGH);
  } else {
    // GreenLED2 Off
    digitalWrite(LEDGreen2, LOW);
  }
  

  // Check the WiFi connection and reconnect if lost.
  if (WiFi.status() != WL_CONNECTED) { 
    Serial.println("No WiFi Connection!");  
    ConnectWiFi();
  } else {
    // Serial.println("WiFi Connected");
    if (!client.connected()) {
      Serial.println("No MQTT Connection!");  
      digitalWrite(LEDGreen2, HIGH);
      ConnectMQTT();
      digitalWrite(LEDGreen2, LOW);
    }
    else {
      // Send Data to MQTT Server
      // Serial.print("Connected status: "); Serial.println(client.connected());   
    }
  }
}
