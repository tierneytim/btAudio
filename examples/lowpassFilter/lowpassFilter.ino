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

int fo=3;
float fc;

void loop() {
 delay(300);
 if(Serial.available()){
  command = Serial.readStringUntil('#');
  if(command.equals("hp")){
   Serial.println("Setting a High-Pass Filter...");
   fc =Serial.parseFloat();
   audio.createFilter(fo, fc, highpass);
  }
  else if(command.equals("stopFilt")){
   audio.stopFilter();
   Serial.println("Stopping Filter...");
  } 
  else if(command.equals("lp")){
   Serial.println("Setting a Low-Pass Filter...");
   fc =Serial.parseFloat();
   audio.createFilter(fo, fc, lowpass);
  }  
 }
}
