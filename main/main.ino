er#include "PubSubClient.h"
#include <ESP8266WiFi.h> //ESP8266WiFi.h
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET LED_BUILTIN  //4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//Readings
float volt;
float amp;
int count;
int pushCount = 30;

//Wifi settings
const char* ssid = "10-05";
const char* password = "82918889";

//mqtt topic url
char* topic = "channels/530407/publish/VAIDHYXEK4MSOA5A"; //channels/<channelID>/publish/API
char* server = "mqtt.thingspeak.com";


WiFiClient wifiClient;
PubSubClient client(server, 1883, wifiClient);

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

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

void ReadVolt(){
  volt=  random(300);
  Serial.print("V= ");
  Serial.println(volt);
  }

void ReadAmp(){
  amp=  random(300);
  Serial.print("A= ");
  Serial.println(amp);
  }

void PushData(){
  String payload="field1=";
  payload+=String(volt);
  payload+="&field2=";
  payload+=String(amp);
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

void Display() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(volt);
  display.setCursor(35, 0);
  display.println("V");
  display.setCursor(50, 0);
  display.println(amp);
  display.setCursor(95, 0);
  display.println("mA");
  display.setCursor(0, 10);
  display.println(volt * amp);
  display.display();
}

void setup() {
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

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
 
 ReadAmp();
 ReadVolt();
 Display();
 if (count>=pushCount){
   count =0;
   PushData();
 }
 count++;
 delay(1000);
  
}


