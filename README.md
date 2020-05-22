# Bluetooth Audio for Arduino
The code exposes the A2DP profile(Bluetooth Audio) available in ESP32 boards using the arduino interface. It assumes you have installed the [ESP32 core](https://github.com/espressif/arduino-esp32) for arduino and have an ESP32 board. I quite like the [TinyPico](https://www.tinypico.com/) and the [ESP32-PICO-KIT](https://www.mouser.co.uk/ProductDetail/Espressif-Systems/ESP32-PICO-KIT?qs=MLItCLRbWsyoLrlknFRqcQ%3D%3D&vip=1&gclid=EAIaIQobChMImN2EgKTG6QIVWbvVCh0zcAPBEAQYASABEgK0kfD_BwE)


## Table of contents
1. [Installation](#a)
2. [Advertising the Connection](#b)
3. [Simple Audio](#c)
	1. [I2S](#c1)
	2. [Hardware Connections](#c2)
	3. [Arduiono Code](#c3)
4. [Changing Volume](#d)
5. [Serial Control](#e)
6. [High-Pass Filtering](#f)
7. [Low-Pass Filtering](#g)
8. [Dynamic Range Compression](#h)
9. [Not so simple audio](#i)


<a name="a"></a>
## Installation
Download respoitory. Within the arduino IDE under Sketch>>Include Library>> select "Add .ZIP library". Then select the downloaded zip file. This should add the library. To use the library you'll have to include the relevant header in the arduino sketch. You'll see this in the following sketches

<a name="b"></a>
## Advertising the Connection
The first step to getting some bluetooth audio up and running is to advertise your ESP32 board.  You will need to include the btAudio header and declare a `btAudio` object.
```cpp
#include <btAudio.h>

// Sets the name of the audio device
btAudio audio = btAudio("ESP_Speaker");
```
The string that you supply to the `btAudio` object becomes the name of the ESP32 bluetooth connection. But this only initialises the object. It doesn't actually start the bluetooth. for that you'll need to use the `btAudio::begin` method.
 
 
```cpp
void setup() {
 
 // streams audio data to the ESP32   
 audio.begin();

}

void loop() {

}
```

Yay, now you can connect to your esp32 board and stream audio to it.  You can connect with your phone, laptop, MP3 player, whatever you want. Sadly, this data is stuck on the ESP32 unless you have a DAC (Digital to Analogue Converter) that can actually send the audio somewhere (speaker, Hi-Fi system). I'll cover that in the next section. Anywho the whole script is below.

```cpp
#include <btAudio.h>

// Sets the name of the audio device
btAudio audio = btAudio("ESP_Speaker");

void setup() {
 
 // streams audio data to the ESP32   
 audio.begin();

}

void loop() {

}
```
<a name="c"></a>
## Simple Audio
Now that we have mastered the bluetooth component of "Bluetooth Audio" let's turn to the audio part. This requires some extra hardware. I like the adafruit [I2S Stereo decoder](https://www.adafruit.com/product/3678). It takes data from the ESP32 and sends converts it to a line out signal  which can be pluged into a stereo or Hi-Fi system (instantly adding wireless audio to your audio system). But what is I2s????

<a name="c1"></a>
### I2S

<a name="c2"></a>
### Hardware

<a name="c3"></a>
### Arduino Code


```cpp
#include <btAudio.h>

// Sets the name of the audio device
btAudio audio = btAudio("ESP_Speaker");

void setup() {
 
 // streams audio data to the ESP32   
 audio.begin();
 
 //  outputs the received data to an I2S DAC https://www.adafruit.com/product/3678
 int bck = 26; 
 int ws = 27;
 int dout = 25;
 audio.I2S(bck, dout, ws);
}

void loop() {

}
```
<a name="d"></a>
## Changing Volume

<a name="e"></a>
## Serial Control

<a name="f"></a>
## High-Pass Filtering

<a name="g"></a>
## Low-Pass Filtering

<a name="h"></a>
## Dynamic Range Compression

<a name="i"></a>
## Not so simple audio