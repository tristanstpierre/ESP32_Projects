#include <WiFi.h> //Wifi library
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <ArduinoMqttClient.h>
const char broker[] = "eduoracle.ugavel.com";
int        port     = 1883;
unsigned long previousMillis = 0; // stores the last time the LED was updated
const long interval = 60000; // interval at which to turn off the LED (10 minutes)



#define EAP_ANONYMOUS_IDENTITY "" 
#define EAP_IDENTITY "tvs60497@uga.edu" 
#define EAP_PASSWORD "Bolt050902" //password for eduroam account
#define WPA_PASSWORD "" //password for home wifi
#define USE_EAP
//SSID NAME
#ifdef USE_EAP
  const char* ssid = "eduroam"; // eduroam SSID
#else 
  const char* ssid = "shamrock"; // home SSID
#endif

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// Receives messages from motion sensor (led sign)
const char motion_status_to_relay_led_sign[] = "smart_room/motion_sensor/motion_status_to_relay_led_sign";

// Receives messages from motion sensor (main light)
const char motion_status_to_relay_main_light[] = "smart_room/motion_sensor/motion_status_to_relay_main_light";

const char motion_status[] = "smart_room/motion_sensor/motion_status";

int IR_pin = 4;
int button1 = 5;
int button2 = 21;
bool button_state = false;
bool motion_state = false;

void startWifi() {
  WiFi.disconnect(true); // disconnect from wifi
  WiFi.mode(WIFI_STA); //init wifi mode
  #ifdef USE_EAP
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wifi_sta_wpa2_ent_enable();
    WiFi.begin(ssid); //connect to wifi
  #else
    WiFi.begin(ssid,WPA_PASSWORD);
  #endif
  WiFi.setSleep(false);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Waiting for connection");
  }
  Serial.println("WiFi connected");  
}

void startMqtt() {
  //added after connected in setup
  mqttClient.onMessage(onMqttMessage);
  mqttClient.setUsernamePassword("giiuser","giipassword");
  mqttClient.connect(broker, port);
  mqttClient.subscribe(motion_status);
  mqttClient.subscribe(motion_status_to_relay_main_light);
  mqttClient.subscribe(motion_status_to_relay_led_sign);
}

void turnOn() {
	Serial.println("Motion Detected...");
}

void turnOff() {
	Serial.println("No Motion...");
}

void mainLight(){
  int motion = digitalRead(IR_pin);
  int button1_pressed = digitalRead(button1);
  if (motion == HIGH && button1_pressed == LOW) { // motion detected no button pressed
    Serial.println("Motion Detected...");
    sendStatus_main_light(1);
    delay(1000);
  }
  else if (motion == HIGH && button1_pressed == HIGH) { // motion detected, but button pressed so turn off
    Serial.println("Button detected turn off");
    sendStatus_main_light(0);
    delay(5000);
  } 
}

void LEDsign() {
  int button2_pressed = digitalRead(button2);
  if (button2_pressed == HIGH && button_state == false) {
    Serial.println("LED sign on...");
    sendStatus_led_sign(1);
    button_state = true;
    delay(1000);
  }
  else if (button2_pressed == HIGH && button_state == true) {
    Serial.println("LED sign off...");
    sendStatus_led_sign(0);
    button_state = false;
    delay(1000);
  }
}

void sendStatus_main_light(int value){
  mqttClient.beginMessage(motion_status_to_relay_main_light);
  mqttClient.write(value);
  mqttClient.endMessage();
}

void sendStatus_led_sign(int value){
  mqttClient.beginMessage(motion_status_to_relay_led_sign);
  mqttClient.write(value);
  mqttClient.endMessage();
}

void onMqttMessage(int messageSize) {
  if(mqttClient.messageTopic() == motion_status){
    Serial.println("got a message");
  }
}

void setup() {
  pinMode(IR_pin, INPUT);
	Serial.begin(115200);
  startWifi();
  startMqtt();
}

void loop() {
  mqttClient.poll();
  mainLight();
  LEDsign();
}
