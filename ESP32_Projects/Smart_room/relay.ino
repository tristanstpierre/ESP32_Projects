#include <WiFi.h> //Wifi library
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <ArduinoMqttClient.h>
const char broker[] = "eduoracle.ugavel.com";
int        port     = 1883;

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

// Sends relay status to UI
const char relay_status_to_UI[] = "smart_room/smart_plug/relay_status_to_UI";

int relay_pin1 = 18;
int relay_pin2 = 5;
int status;

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
  mqttClient.subscribe(relay_status_to_UI);
  mqttClient.subscribe(motion_status_to_relay_led_sign);
  mqttClient.subscribe(motion_status_to_relay_main_light);
}

void sendStatus(){
  mqttClient.beginMessage(relay_status_to_UI);
  mqttClient.write(status);
  mqttClient.endMessage();
}

void onMqttMessage(int messageSize) {
  if (mqttClient.messageTopic() == motion_status_to_relay_main_light) {
    int status_main_light = mqttClient.read();

    if (status_main_light == 1) {
      Serial.printf("ON...\n");
	    digitalWrite(relay_pin1, HIGH);
    }
    if (status_main_light == 0) {
      Serial.printf("OFF...\n");
	    digitalWrite(relay_pin1, LOW);
    }
  }
  if (mqttClient.messageTopic() == motion_status_to_relay_led_sign) {
    int status_led_sign = mqttClient.read();

    if (status_led_sign == 1) {
      Serial.printf("ON...\n");
	    digitalWrite(relay_pin2, HIGH);
    }
    if (status_led_sign == 0) {
      Serial.printf("OFF...\n");
	    digitalWrite(relay_pin2, LOW);
    }
  }
}

void setup() {
  pinMode(relay_pin1, OUTPUT);
  pinMode(relay_pin2, OUTPUT);
	Serial.begin(115200);
  startWifi();
  startMqtt();
}

void loop() {
  mqttClient.poll();
}
