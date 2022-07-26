#include <btAudio.h>

// Sets the name of the audio device
btAudio audio = btAudio("ESP_Speaker");

void setup() {
 
 // Streams audio data to the ESP32   
 audio.begin();
 
 // Re-connects to last connected device
 audio.reconnect();

}

void loop() {

}
