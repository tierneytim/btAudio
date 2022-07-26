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
}

void loop() {

}
