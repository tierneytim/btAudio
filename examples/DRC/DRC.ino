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
float rel= 0.2;
float ratio = 10;
float kneeWidth= 1;
float gain=0;

void loop() {
 delay(300);
 if(Serial.available()){
  command = Serial.readStringUntil('#');
   if(command.equals("compress")){
    Serial.println("Applying Compression");
    audio.compress(thresh,attack,rel,ratio,kneeWidth,gain);
   }
   else if(command.equals("decompress")){
    Serial.println("Releasing Compression");
    audio.decompress();
   }
   else if(command.equals("gain")){
    gain =(int)Serial.parseInt();
    Serial.println(gain);
    audio.compress(thresh,attack,rel,ratio,kneeWidth,gain);
   }
   else if(command.equals("thresh")){
    thresh =(float)Serial.parseFloat();
    Serial.println(thresh);
    audio.compress(thresh,attack,rel,ratio,kneeWidth,gain);
   }
   else if(command.equals("attack")){
    attack =(float)Serial.parseFloat();
    Serial.println(attack);
   audio.compress(thresh,attack,rel,ratio,kneeWidth,gain); 
   }
   else if(command.equals("rel")){
    rel =(float)Serial.parseFloat();
    Serial.println(rel);
    audio.compress(thresh,attack,rel,ratio,kneeWidth,gain);
   }
   else if(command.equals("ratio")){
    ratio =(float)Serial.parseFloat();
    Serial.println(ratio);
    audio.compress(thresh,attack,rel,ratio,kneeWidth,gain);
   }
   else if(command.equals("w")){
    kneeWidth =(int)Serial.parseInt();
    Serial.println(kneeWidth);
   audio.compress(thresh,attack,rel,ratio,kneeWidth,gain);
   }
  }
}
