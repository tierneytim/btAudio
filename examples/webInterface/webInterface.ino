#include<webDSP.h>
#include<btAudio.h>

// create audio object 
btAudio audio = btAudio("ESP_Speaker");

// create webserver object
webDSP web;

void setup() {

 // Streams audio data to the ESP32   
 audio.begin();

 // Re-connects to last connected device
 audio.reconnect();

 // Outputs the received data to an I2S DAC, e.g. https://www.adafruit.com/product/3678
 int bck = 26; 
 int ws = 27;
 int dout = 25;
 audio.I2S(bck, dout, ws);

 // Opens the serial port
 Serial.begin(115200);
  
 // Replace ssid and password with your details
 const char* ssid = "";
 const char* password = "";
 web.begin(ssid, password, &audio); 
}

void loop() {
  // continually check on client 
  web._server.handleClient();
}
