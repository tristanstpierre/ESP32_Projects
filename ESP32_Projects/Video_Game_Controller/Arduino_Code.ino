#include <WiFi.h>
#include <ArduinoMqttClient.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <M5StickCPlus.h>

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

char buffer[100];
int status;
float accX, accY, accZ;
int BTNA = 37;

const char* broker = "info8000.ga";
const int port = 1883;
const char* mqtt_user = "giiuser";
const char* mqtt_pw = "giipassword";
const char* t_status = "ugaelee2045sp23/tstpierre/BTcontrol";
const char* t_control_status = "ugaelee2045sp23/tstpierre/BTcontrol_shot_fired";
const char* t_control_color = "ugaelee2045sp23/tstpierre/BTcontrol_accelerometer";

WiFiClient espClient;
MqttClient client(espClient);

void onMqttMessage(int messageSize) {
  Serial.println("got a message");
  Serial.println(client.messageTopic());
  }

void setup() {
  M5.begin();
  M5.IMU.Init();
  
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

  client.onMessage(onMqttMessage);
  client.setUsernamePassword(mqtt_user, mqtt_pw );
  client.connect(broker, port);
  Serial.println("broker connected");
  M5.Lcd.print("connected");

  client.subscribe(t_control_status);
  client.subscribe(t_control_color);

  // print ip
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  M5.Lcd.print(ip);
}

void sendStatus(){
  client.beginMessage(t_status);
  client.write(status);
  client.write(accX);
  client.write(accY);
  client.write(accZ);
  client.endMessage();
}
int waitingForRelease = 0;
void loop(){

  M5.update();
  client.poll();

  M5.IMU.getAccelData(&accX, &accY, &accZ);
  M5.Lcd.setCursor(0, 45);
  M5.Lcd.printf("X:%5.2f/nY:%5.2f/nZ:%5.2f ", accX, accY, accZ);  

  static unsigned long last_time = 0;
  if(millis()-last_time > 2000){
    sendStatus();
    last_time = millis();
  }

  if(digitalRead(37)==LOW && !waitingForRelease){
    status = !status;
    sendStatus();
    waitingForRelease = 1;
  }
  if(digitalRead(37)==HIGH){
    waitingForRelease = 0;
  }
}
