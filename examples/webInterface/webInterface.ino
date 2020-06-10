#include<webDSP.h>
#include<btAudio.h>

btAudio audio = btAudio("ESP_Speaker");
webDSP web;

void setup() {
  Serial.begin(115200);  
  audio.begin();
  int bck = 26; 
  int ws = 27;
  int dout = 25;  
  audio.I2S(bck, dout, ws);
  const char* ssid = "";
  const char* password = "";
  web.begin(ssid,password ,&audio); 
}

void loop() {
  web._server.handleClient();
}
