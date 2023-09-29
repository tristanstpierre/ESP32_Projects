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
int red;
int gre;
int blu;
int BTNA = 37;

const char* broker = "info8000.ga";
const int port = 1883;
const char* mqtt_user = "giiuser";
const char* mqtt_pw = "giipassword";
const char* t_control_status = "tvs60497/light_control_status";
const char* t_control_color = "tvs60497/light_control_color";
const char* t_status = "tvs60497/light_status";

WiFiClient espClient;
MqttClient client(espClient);

void setup() {
  M5.begin();
  
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

void sendStatus() {
  client.beginMessage(t_status, 4, true);
  client.write(status);
  client.write(red);
  client.write(gre);
  client.write(blu);
  client.endMessage();
}

void updateStatusColor() {
  M5.Lcd.fillScreen(rgb565(red, gre, blu));
  Serial.println("Color changed");  
}

void updateStatusOnOff() {
  M5.Axp.ScreenBreath(status ? 15 : 0);
  Serial.println("Screen turned on/off");  
}

int waitingForRelease = 0;
void loop(){

  M5.update();
  client.poll();

  static unsigned long last_time = 0;
  if(millis()-last_time > 2000){
    sendStatus();
    last_time = millis();
  }

  if(digitalRead(37)==LOW && !waitingForRelease){
    status = !status;
    updateStatusOnOff();
    sendStatus();
    waitingForRelease = 1;
  }
  if(digitalRead(37)==HIGH){
    waitingForRelease = 0;
  }
}

uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) 
{
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

void onMqttMessage(int messageSize) {
  Serial.println("got a message");
  Serial.println(client.messageTopic());

  if (client.messageTopic() == "tvs60497/light_control_color") {
    // client.read
    for (int i = 0; i < messageSize; i++) {
      buffer[i] = client.read();      
    }
    red = buffer[0];
    gre = buffer[1];
    blu = buffer[2];
    Serial.println(red);
    Serial.println(gre);
    Serial.println(blu);

    updateStatusColor();
  } else if (client.messageTopic() == "tvs60497/light_control_status") {
      status = client.read();
      updateStatusOnOff();
    }
}
