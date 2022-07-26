#include <btAudio.h>

// Sets the name of the audio device
btAudio audio = btAudio("ESP_Speaker");

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
  delay(5000);
  // updates metadata
  audio.updateMeta();
  Serial.print("Title: ");
  Serial.println(audio.title);
  Serial.print("Artist: ");
  Serial.println(audio.artist);
  Serial.print("Album: ");
  Serial.println(audio.album);
  Serial.print("Genre: ");
  Serial.println(audio.genre);
    
}
