#include<webDSP.h>
#include<btAudio.h>

// create audio object 
btAudio audio = btAudio("ESP_Speaker");

// create webserver object
webDSP web;

void setup() {
  Serial.begin(115200);  
  
  //start streaming audio
  audio.begin();
  
  //transmit data to DAC
  int bck = 26; 
  int ws = 27;
  int dout = 25;  
  audio.I2S(bck, dout, ws);
  
  // replace ssid and password with your details
  const char* ssid = "";
  const char* password = "";
  web.begin(ssid,password ,&audio); 
}

void loop() {
  // continually check on client 
  web._server.handleClient();
}
