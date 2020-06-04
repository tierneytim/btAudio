#include <btAudio.h> 

 btAudio audio = btAudio("ESP_Speaker");

void setup() {
  Serial.begin(115200);

  audio.begin();
  int bck = 26; 
  int ws = 27;
  int dout = 25;  
  audio.I2S(bck, dout, ws);
  
  // REPLACE WITH YOUR NETWORK CREDENTIALS
  const char* ssid = "";
  const char* password = "";
  audio.webDSP(ssid,password);
}



void loop() {
}
