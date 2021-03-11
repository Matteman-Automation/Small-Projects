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

int LEDRed = 2;     // D4
int LEDGreen = 14;  // D5
int LEDBlue = 12;   // D6

byte RGBOnOff = 0;
int RGBBrightness = 0;
int RGBred = 255;
int RGBgreen = 255;
int RGBblue = 255;

int WiFiTry = 0;          // times the WiFi is not available
int MQTTTry = 0;          // times the MQTT server is not available. 

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(LEDRed, OUTPUT);
  pinMode(LEDGreen, OUTPUT);
  pinMode(LEDBlue, OUTPUT);
 
  ConnectWiFi();
  client.setServer(mqtt_server, 1883);
  ConnectMQTT();


  client.subscribe("Office/rgb/light/switch");
  client.subscribe("Office/rgb/brightness/set");
  client.subscribe("Office/rgb/rgb/set");
  client.setCallback(callback);

  // Send the Turn Off to Home Assistant
  client.publish("Office/rgb/brightness/status", "0");
  client.publish("Office/rgb/rgb/status", "255,255,255");
  client.publish("Office/rgb/light/status", "OFF");
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
  while (!client.connected() && MQTTTry < 5 ) {
      Serial.print("Attempting MQTT connection...");
      ++MQTTTry;
      // Create a random client ID
      String clientId = "ESP8266Client-";
      clientId += String(random(0xffff), HEX);
      // Attempt to connect
      if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
        Serial.println("connected");
        // Once connected, publish an announcement...
        client.publish("outTopic", "hello world");
        // ... and resubscribe
        client.subscribe("inTopic");
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
    }
  }
  MQTTTry = 0;
  Serial.println("MQTT end loop.");
}

void callback(char* topic, byte* payload, unsigned int length) {
  int Red;
  int Green;
  int Blue;
  
  Serial.print("MQTT Message: ");
  Serial.println(topic);
  
  if (strcmp(topic,"Office/rgb/light/switch")==0){
    Serial.println("Signal for RGB Switch");
    String Status = "";
    for (int i=0;i<length;i++) {
      Status+= (char)payload[i];
    }
    client.publish("Office/rgb/light/status", Status.c_str());
    if (Status == "ON") {
      RGBOnOff = 1;
    } else {
      RGBOnOff = 0;
    }
    Serial.print("RGBOnOff: ");
    Serial.println(RGBOnOff);
  }
   
  if (strcmp(topic,"Office/rgb/brightness/set")==0){
    Serial.println("Signal for RGB Brightness");

    String Status = "";
    for (int i=0;i<length;i++) {
      Status+= (char)payload[i];
    }
    client.publish("Office/rgb/brightness/status", Status.c_str());

    RGBBrightness = Status.toInt();
    Serial.print("RGBBrightness: ");
    Serial.println(RGBBrightness);
    
  }
  if (strcmp(topic,"Office/rgb/rgb/set")==0){
    Serial.println("Signal for RGB Set");

    String Status = "";
    for (int i=0;i<length;i++) {
      Status+= (char)payload[i];
    }
    client.publish("Office/rgb/rgb/status", Status.c_str());
    Serial.print("Status: ");
    Serial.println(Status);
    // Get RGB from xxx,xxx,xxx
    int FirstIndex = Status.indexOf(',');
    int LastIndex = Status.lastIndexOf(",");
    RGBred = Status.substring(0,FirstIndex).toInt();
    RGBgreen = Status.substring(FirstIndex + 1, LastIndex).toInt();
    RGBblue = Status.substring(LastIndex+1).toInt(); 
    Serial.print("RGBred: "); Serial.print(RGBred);
    Serial.print(" RGBgreen: "); Serial.print(RGBgreen);
    Serial.print(" RGBblue: "); Serial.println(RGBblue);
  }
  if (RGBOnOff = 1){
    Red = RGBred * RGBBrightness / 100;
    Green = RGBgreen * RGBBrightness / 100;
    Blue = RGBblue * RGBBrightness / 100;
  } else {
    // Lights Out
    Red = 0;
    Green = 0;
    Blue = 0;
  }
  analogWrite(LEDRed, Red * 2.55);
  analogWrite(LEDGreen, Green * 2.55);
  analogWrite(LEDBlue, Blue * 2.55);
  
}


void loop() {
  //  Your Code here
  delay(100);
  client.loop();  

 
}
