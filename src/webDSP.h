#ifndef WEBDSP_H
#define WEBDSP_H

#include "Arduino.h"
#include<WebServer.h>
#include<btAudio.h>
#include<WiFi.h>


class webDSP {
  public:
  webDSP();
  void begin(const char* ssid, const char* password, btAudio* audio);
  WebServer _server;
  
  private:
  btAudio* _audio;
  void handleRoot();
  void handleCss();
  void handleGet();
  

};

 const char PROGMEM html2[2866]= R"rawliteral(<!DOCTYPE HTML><html> <head> <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
 <link rel="stylesheet" type="text/css" href="workStyle.css"> <title>DSP</title> </head> <body> <center> <header>WiFi DSP</header> <button class="co">Volume</button> <div class="c"> <form action="/get"> <div class="r"> <h3>Volume</h3><p> Value between 0 and 1 to set volume</p><input type="text" name="v" placeholder="e.g. 0.42"> <input type="submit" value="Submit"> </div></form><br></div><button class="co">Filters</button><div class="c"> <form action="/get"> <input type="submit" value="Stop Filters" name="f" id="stop"> </form><br><form action="/get"> <div class="r"> <h3>High-Pass Filter (Hz)</h3><p>Frequencies lower than this are suppressed</p><input type="text" name="h" placeholder="e.g. 100"> <input type="submit" value="Submit"> </div></form><br><form action="/get"> <div class="r"> <h3>Low-Pass Filter (Hz)</h3> <p>Frequencies higher than this are suppressed</p><input type="text" name="l" placeholder="e.g. 20000"> <input type="submit" value="Submit"> </div></form><br></div><button class="co">Dynamic Range Compression</button><div class="c"> <form action="/get"> <input type="submit" value="Turn off Compressor" name="c" id="stop"> </form><br><form action="/get"> <div class="r"> <h3>Threshold (dB)</h3> <p>Threshold at which audio is compressed</p><input type="text" name="t" align="right" placeholder="e.g. 30"> <input type="submit" value="Submit"></div></form> <form action="/get"> <div class="r"> <h3>Attack Time (s): </h3> <p>Time for compressor to start working</p><input type="text" name="a" placeholder="e.g. 0.1"> <input type="submit" value="Submit"></div></form> <form action="/get"> <div class="r"> <h3>Release Time (s): </h3> <p>Time it takes compressor to stop working</p><input type="text" name="r" placeholder="e.g. 0.1"> <input type="submit" value="Submit" ></div></form> <form action="/get"> <div class="r"> <h3>Ratio: </h3> <p>Reduction ratio of audio </p><input type="text" name="R" placeholder="e.g. 10"> <input type="submit" value="Submit"></div></form> <form action="/get"> <div class="r"> <h3>Knee Width(dB)</h3> <p>Smoothness of compressor</p><input type="text" name="w" placeholder="e.g. 10"> <input type="submit" value="Submit"> </div></form> <form action="/get"> <div class="r"> <h3>Gain (dB)</h3> <p>Gain to add to data after compression</p><input type="text" id="gain" name="g" align="right" placeholder="e.g. 0"> <input type="submit" value="Submit"></div></form> </div><script>var coll=document.getElementsByClassName("co");var i;for (i=0; i < coll.length; i++){coll[i].addEventListener("click", function(){this.classList.toggle("active"); var c=this.nextElementSibling; if (c.style.maxHeight){c.style.maxHeight=null;}else{c.style.maxHeight=c.scrollHeight + "px";}});}</script></body></center></html>)rawliteral";

const char PROGMEM css2[825]= R"rawliteral(input[type=text], select{width: 40%; padding: 12px 20px; margin: 8px 0; border: 1px solid #ccc; border-radius: 4px;}#stop{width: 80%; padding: 12px 20px; margin: 8px 0; border-radius: 4px;}input[type=submit]{width: 40%; background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; border-radius: 4px;}.co{background-color: #555;color: white; padding: 18px; width: 100%; border: none; outline: none; font-size: 15px;}.co:after{content: '\002B'; color: white; float: right;}.r{border-radius: 25px; border: 2px solid #555; width: 80%; margin: 30px 30px;}.c{max-height: 0; width: 100%; overflow: hidden; transition: max-height 0.2s ease-out;background-color: #f1f1f1;}header{background-color: #222; padding: 30px 0px;font-size: 35px; color: white;})rawliteral";
#endif