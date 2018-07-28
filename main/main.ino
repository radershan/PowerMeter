#include "PubSubClient.h"
#include <ESP8266WiFi.h> //ESP8266WiFi.h
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_INA219.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  


//OLED Display
#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!"); // only required for NodeMCU
#endif

//INA219
Adafruit_INA219 ina219;

//Readings
unsigned long previousMillis = 0;
unsigned long previousMillisData = 0;
unsigned long interval = 100;
unsigned long intervalData = 30000;
float shuntVoltage = 0;
float busVoltage = 0;
float current_mA = 0;
float loadVoltage = 0;
float energy = 0;

//mqtt topic url
char* topic = "channels/530407/publish/VAIDHYXEK4MSOA5A"; //channels/<channelID>/publish/API
char* server = "mqtt.thingspeak.com";


WiFiClient wifiClient;
PubSubClient client(server, 1883, wifiClient);

//Reconnect to server when disconnects due to timeout
void reconnect(){
   String clientName="ESP-Thingspeak";
  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);
  
   if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }
  }

//Acces device from server
void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}


//push date to the MQTT server
void PushData(){
  String payload="field1=";
  payload+=String(loadVoltage);
  payload+="&field2=";
  payload+=String(current_mA);
  payload+="&field3=";
  payload+=String(energy);
  payload+="&tatus=MQTTPUBLISH";
  
  if (client.connected()){
    Serial.print("Sending payload: ");
    Serial.println(payload);
    
    if (client.publish(topic, (char*) payload.c_str())) {
      Serial.println("Publish ok");
    }
    else {
      Serial.println("Publish failed");
    }
  }
  {
    reconnect();
    }
  }

//show readings in display
void Display() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(loadVoltage);
  display.setCursor(35, 0);
  display.println("V");
  display.setCursor(50, 10);
  display.println(current_mA);
  display.setCursor(95, 10);
  display.println("mA");
  display.setCursor(0, 20);
  display.println(loadVoltage * current_mA);
  display.setCursor(65, 20);
  display.println("mW");
  display.setCursor(0, 30);
  display.println(energy);
  display.setCursor(65, 30);
  display.println("mWh");
  display.display();
}

void GetReadings() {
  shuntVoltage = ina219.getShuntVoltage_mV();
  busVoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  loadVoltage = busVoltage + (shuntVoltage / 1000);
  energy = energy + loadVoltage * current_mA / 3600;

  Serial.print("Amp : ");
  Serial.println(current_mA);
  Serial.print("Volt : ");
  Serial.println(loadVoltage);
  Serial.print("Energy : ");
  Serial.println(energy);
}

void setup() {
  Serial.begin(115200);

  ina219.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  

  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("Connected to Wifi!");

  
String clientName="ESP-Thingspeak";
  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);
  
  if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }

}

void loop() {
 unsigned long currentMillis = millis();
 if (currentMillis - previousMillis >= interval) // get readings from every 100ms
  {
    previousMillis = currentMillis;
    GetReadings();
    Display();
    if (currentMillis - previousMillisData >= intervalData){ // push data to cloud in every 30sec
      previousMillisData = currentMillis;
      PushData();
    }
 }
 

  
}


