#include "filter.h"
const uint16_t sampleRate = 48000;
int16_t sig[sampleRate];

void setup() {
  delay(3000);

  //begin serial after clocks set
  Serial.begin(115200);
  Serial.println("hello");

  // signal to be modulated
  for (int i = 1; i < (sampleRate + 1); i++) {
    sig[i - 1] = (int16_t)(sin(2.0 * PI * 1000 * i / sampleRate) * 32767 * 1);
  }

  volatile int16_t result;
  uint32_t beg;


  filter filt = filter(8000, 48000, 1, lowpass);
  beg = micros();
  for (uint16_t i = 0; i < sampleRate; i++) {
    result = filt.process(sig[i]);
  }
  Serial.print((micros() - beg) * 100.0 / 1000000.0);
  Serial.println("% CPU used for 1 Biquad");
  Serial.print("Ignore this number: ");
  Serial.println(result);

  filter filt2 = filter(8000, 48000, 2, lowpass);
  beg = micros();
  for (uint16_t i = 0; i < sampleRate; i++) {
    result = filt2.process(sig[i]);
  }
  Serial.print((micros() - beg) * 100.0 / 1000000.0);
  Serial.println("% CPU used for 2 Biquads");
  Serial.print("Ignore this number: ");
  Serial.println(result);

  filter filt3 = filter(200, 48000, 3, highpass);
  beg = micros();
  for (uint16_t i = 0; i < sampleRate; i++) {
    result = filt3.process(sig[i]);
  }
  Serial.print((micros() - beg) * 100.0 / 1000000.0);
  Serial.println("% CPU used for 3 Biquads");
  Serial.print("Ignore this number: ");
  Serial.println(result);
}

void loop() {
}