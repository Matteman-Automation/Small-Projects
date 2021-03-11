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
const char* Device_ID = "Dimmer";

int LEDStrip = 2;     // D4

byte OnOff = 0;
int Brightness = 0;

int WiFiTry = 0;          // times the WiFi is not available
int MQTTTry = 0;          // times the MQTT server is not available. 

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(LEDStrip, OUTPUT);
 
  ConnectWiFi();
  client.setServer(mqtt_server, 1883);
  ConnectMQTT();


  client.subscribe("office/light/switch");
  client.subscribe("office/light/brightness/set");
  client.setCallback(callback);

  // Send the Turn Off to Home Assistant
  client.publish("office/light/brightness/status", "0");
  client.publish("office/light/status", "OFF");
}

void ConnectWiFi() { 
  //Connect with WiFi network
  WiFi.begin(YourWiFiSSID, YourWiFiPassword);
  Serial.println("Connecting to WiFi");
  //Try to connect to WiFi for 5 times
  while (WiFi.status() != WL_CONNECTED && WiFiTry < 5) {
    digitalWrite(LEDStrip, HIGH);
    ++WiFiTry;
    Serial.print("WiFi status: ");; Serial.println(WiFi.status());
    Serial.print("WiFiTry: ");; Serial.println(WiFiTry);
    delay(500);
    digitalWrite(LEDStrip, LOW);
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

  
  // Recconnect to the MQTT server
  Serial.println("Attempting Reconnect MQTT connection...");
  // Attempt to connect
  //  client.setClient(Device_ID);
  // while (!client.connect(mqtt_server, mqtt_user, mqtt_password) && MQTTTry < 5 ) {
  /// while (!client.connect(Device_ID) && MQTTTry < 5 ) {
  //   ++MQTTTry;
  //   Serial.println("No MQTT, Retry");
  //   delay(1000);   
  // } 
  MQTTTry = 0;
  Serial.println("MQTT end loop.");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT Message: ");
  Serial.println(topic);
  
  if (strcmp(topic,"office/light/switch")==0){
    Serial.println("Signal for Switch");
    String Status = "";
    for (int i=0;i<length;i++) {
      Status+= (char)payload[i];
    }
    client.publish("office/light/status", Status.c_str());
    if (Status == "ON") {
      OnOff = 1;
    } else {
      OnOff = 0;
    }
    Serial.print("OnOff: ");
    Serial.println(OnOff);
  }
   
  if (strcmp(topic,"office/light/brightness/set")==0){
    Serial.println("Signal for Brightness");

    String Status = "";
    for (int i=0;i<length;i++) {
      Status+= (char)payload[i];
    }
    client.publish("office/light/brightness/status", Status.c_str());

    Brightness = Status.toInt();
    Serial.print("Brightness: ");
    Serial.println(Brightness);
    
  }
  int Strip = 0;
  
  if (OnOff == 1){
    Strip = 2.55 * Brightness;  // Brightness 1-100, AnaloogWrite 0-255, 
  } 
  analogWrite(LEDStrip, Strip);
  Serial.print("Strip: ");
  Serial.println(Strip);
}


void loop() {
  //  Your Code here
  delay(100);
  client.loop();  

 
}
