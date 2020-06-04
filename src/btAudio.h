#ifndef BTAUDIO_H
#define BTAUDIO_H

#include "Arduino.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "driver/i2s.h"
#include "filter.h"
#include "DRC.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// postprocessing 
enum {
    NOTHING = 0,
    FILTER,
	COMPRESS,
	FILTER_COMPRESS,
};
class btAudio {
  public:
	//Constructor
	btAudio(const char *devName);
	
	// Bluetooth functionality
	void begin();  
	void end();
	
	// I2S Audio
	void I2S(int bck, int dout, int ws);
	void volume(float vol);
    
	// Filtering
	void createFilter(int n, float hp,int type);
	void stopFilter();
	
	// Compression
	void compress(float T,float alphAtt,float alphRel, float R,float w,float mu );
	void decompress();
	
	// Web interface
	void webDSP(const char* ssid, const char* password);
	
	
  private:
    const char *_devName;
	bool _filtering=false;
	bool _compressing=false;
	float _T=60.0;
	float _alphAtt=0.001;
	float _alphRel=0.1; 
	float _R=4.0;
	float _w=10.0;
	float _mu=0.0; 
	static int _postprocess;
	AsyncWebServer _server= AsyncWebServer(80);
	
	// static function causes a static infection of variables
	static void i2sCallback(const uint8_t *data, uint32_t len);
	static void getAddress(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);
	
	// bluetooth address of connected device
	static uint8_t _address[6];
	static DRC _DRCR;
	static DRC _DRCL;
	static float _vol;
	static filter _filtLlp;
    static filter _filtRlp;
    static filter _filtLhp;
    static filter _filtRhp;		
	
   // static File _file;	
};

// The Webpage...
const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML><html><style>input[type=text], select{width: 40%; padding: 12px 20px; margin: 8px 0; display: inline-block; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box;}#stop{width: 80%; padding: 12px 20px; margin: 8px 0; display: inline-block; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box;}input[type=submit]{width: 40%; background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; border-radius: 4px; cursor: pointer;}input[type=submit]:hover{background-color: #45a049;}.collapsible{background-color: #555; color: white; cursor: pointer; padding: 18px; width: 100%; border: none; text-align: left; outline: none; font-size: 15px;}.active, .collapsible:hover{background-color: #555;}.collapsible:after{content: '\002B'; color: white; font-weight: bold; float: right; margin-left: 0px;}.active:after{content: "\2212";}.rcorners1{border-radius: 25px; border: 2px solid #555; width: 80%; margin: 30px 30px;}.content{padding: 0 0px; max-height: 0; width: 100%; overflow: hidden; transition: max-height 0.2s ease-out; background-color: #f1f1f1;}div{border-radius: 5px; background-color: #f2f2f2; padding: 0px;}header{background-color: #222; padding: 30px 0px; text-align: center; width: 100%; font-size: 35px; color: white;}</style> <head> <title>DSP Web Control</title> <meta name="viewport" content="width=device-width, initial-scale=1"> </head> <body> <center> <header>WiFi DSP</header> <button class="collapsible">Volume</button> <div class="content"> <form action="/get"> <div class="rcorners1"> <h3>Volume</h3><p> Value between 0 and 1 to set volume</p><input type="text" name="v" placeholder="e.g. 0.42"> <input type="submit" value="Submit"> </div></form><br></div><button class="collapsible">Filters</button><div class="content"> <form action="/get"> <input type="submit" value="Stop Filters" name="f" id="stop"> </form><br><form action="/get"> <div class="rcorners1"> <h3>High-Pass Filter (Hz)</h3><p>Frequencies lower than this are suppressed</p><input type="text" name="h" placeholder="e.g. 100"> <input type="submit" value="Submit"> </div></form><br><form action="/get"> <div class="rcorners1"> <h3>Low-Pass Filter (Hz)</h3> <p>Frequencies higher than this are suppressed</p><input type="text" name="l" placeholder="e.g. 20000"> <input type="submit" value="Submit"> </div></form><br></div><button class="collapsible">Dynamic Range Compression</button><div class="content"> <form action="/get"> <input type="submit" value="Turn off Compressor" name="c" id="stop"> </form><br><form action="/get"> <div class="rcorners1"> <h3>Threshold (dB)</h3> <p>Threshold at which audio is compressed</p><input type="text" name="t" align="right" placeholder="e.g. 30"> <input type="submit" value="Submit"></div></form> <form action="/get"> <div class="rcorners1"> <h3>Attack Time (s): </h3> <p>Time for compressor to start working</p><input type="text" name="a" placeholder="e.g. 0.1"> <input type="submit" value="Submit"></div></form> <form action="/get"> <div class="rcorners1"> <h3>Release Time (s): </h3> <p>Time it takes compressor to stop working</p><input type="text" name="r" placeholder="e.g. 0.1"> <input type="submit" value="Submit" ></div></form> <form action="/get"> <div class="rcorners1"> <h3>Ratio: </h3> <p>Reduction ratio of audio </p><input type="text" name="R" placeholder="e.g. 10"> <input type="submit" value="Submit"></div></form> <form action="/get"> <div class="rcorners1"> <h3>Knee Width(dB)</h3> <p>Smoothness of compressor</p><input type="text" name="w" placeholder="e.g. 10"> <input type="submit" value="Submit"> </div></form> <form action="/get"> <div class="rcorners1"> <h3>Gain (dB)</h3> <p>Gain to add to data after compression</p><input type="text" id="gain" name="g" align="right" placeholder="e.g. 0"> <input type="submit" value="Submit"></div></form> </div><script>var coll=document.getElementsByClassName("collapsible");var i;for (i=0; i < coll.length; i++){coll[i].addEventListener("click", function(){this.classList.toggle("active"); var content=this.nextElementSibling; if (content.style.maxHeight){content.style.maxHeight=null;}else{content.style.maxHeight=content.scrollHeight + "px";}});}</script></body></center></html>)rawliteral";


#endif