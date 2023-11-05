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

// Sends messages to relay
const char motion_status_to_relay[] = "smart_room/motion_sensor/motion_status_to_relay";

const char motion_status[] = "smart_room/motion_sensor/motion_status";

int IR_pin = 4;
int button_led = 5;
int m_status;

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
  mqttClient.subscribe(motion_status_to_relay);
}

void turnOn() {
	Serial.println("Motion Detected...");
}

void turnOff() {
	Serial.println("No Motion...");
}

void mainLight(){
  int motion = digitalRead(IR_pin);
  if (motion == HIGH) { // motion detected
    Serial.println("Motion Detected...");
    sendStatus(1);
    currentMillis = millis(); // store the current time
    if (currentMillis >= interval) {
      break;
    }
  }
  else if (motion == LOW) {
    Serial.println("No Motion...");
    sendStatus(0);
  }  
}

// Problem: Need to find a way to incorporate the button presses to interupt the delay of the motion sensor when on
// void checkSensor() {
// 	int motion = digitalRead(IR_pin);
//   int button1_pressed = digitalRead(button1);
//   bool button_state = false;
// 	if (motion == LOW && !sensorTriggered && !button1_pressed) {
// 		return;
// 	} else if (motion == LOW && sensorTriggered && !button1_pressed) {
// 		sensorTriggered = 0;
// 		sendStatus(0);
//     turnOff();
// 	} else if (motion == LOW && button_state && button1_pressed) {
// 		sensorTriggered = 0;
//     button_state = false;
// 		sendStatus(0);
//     turnOff();
//    } else if (motion == HIGH && !sensorTriggered && !button1_pressed) {
//     sensorTriggered = 1;
// 		sendStatus(1);
//     turnOn();
//   } else if (motion == LOW && !sensorTriggered && button1_pressed && !button_state) {
//     sensorTriggered = 1;
//     button_state = true;
// 		sendStatus(1);
//     turnOn();
// 	} else if (motion == HIGH && sensorTriggered) {
// 		return;
// 	}
// }

void sendStatus(int value){
  mqttClient.beginMessage(motion_status_to_relay);
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
}
