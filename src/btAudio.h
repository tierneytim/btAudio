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
/* #include "FS.h"
#include "SD.h"
#include "SPI.h" */

enum {
    NOTHING = 0,
    FILTER,
	COMPRESS,
	FILTER_COMPRESS,
	RECORD
};
class btAudio {
  public:
	btAudio(const char *devName);
	void begin();  
	void I2S(int bck, int dout, int ws);
	void end();
	void createFilter(int n, float hp,int type);
	void stopFilter();
	void compress(float T,float alphAtt,float alphRel, float R,int w,int mu);
	void decompress();
    void volume(float vol);
    void record(const char * path);
	void stopRecord();
	int  postProc();
	static uint8_t _address[6];
  private:
    const char *_devName;
	bool _filtering=false;
	bool _compressing=false;
	
	// static function that causes a static infection of variables it needs
	static void i2sCallback(const uint8_t *data, uint32_t len);
	static void getAddress(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);
	static DRC _DRCR;
	static DRC _DRCL;
	static float _vol;
	static filter _filtLlp;
    static filter _filtRlp;
    static filter _filtLhp;
    static filter _filtRhp;		
	static int _postprocess;
   // static File _file;	
};

#endif