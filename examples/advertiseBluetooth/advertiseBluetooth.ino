#include <btAudio.h>

// Sets the name of the audio device
btAudio audio = btAudio("ESP_Speaker");

void setup() {
 
 // streams audio data to the ESP32   
 audio.begin();

}

void loop() {

}