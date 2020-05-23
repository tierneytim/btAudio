#include <btAudio.h>

btAudio audio = btAudio("ESP_Speaker");
String command;

void setup() { 
 audio.begin();
 int bck = 26; 
 int ws = 27;
 int dout = 25;
 audio.I2S(bck, dout, ws);
 Serial.begin(115200);
 
}

float thresh=30;
float attack=0.1;
float release= 0.2;
float ratio = 10;
float width= 10;
float gain=0;

void loop() {
 delay(300);
 if(Serial.available()){
  command = Serial.readStringUntil('#');
   if(command.equals("compress")){
    Serial.println("Applying Compression");
    audio.compress(thresh,attack,release,ratio,width,gain);
   }
   else if(command.equals("decompress")){
    Serial.println("Releasing Compression");
    audio.decompress();
   }
  }
}
