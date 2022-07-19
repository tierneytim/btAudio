#include <btAudio.h>

btAudio audio = btAudio("ESP_Speaker");
String command;

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
}


void loop() {
 delay(300);
 // check if data is available 
 if(Serial.available()){
  //read until a terminator. after this point there should only be numbers
  command = Serial.readStringUntil('#');
  if(command.equals("vol")){
   //read and set volume	  
   float vol =Serial.parseFloat();
   Serial.println("Changing Volume");
   audio.volume(vol);
  }
 }
}
