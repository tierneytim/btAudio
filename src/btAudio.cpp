#include "btAudio.h"

 float btAudio::_vol=1.0;
 int btAudio::_postprocess=0;
 filter btAudio::_filtLhp = filter(20,44100,3,highpass); 
 filter btAudio::_filtRhp = filter(20,44100,3,highpass);
 filter btAudio::_filtLlp = filter(20000,44100,3,lowpass); 
 filter btAudio::_filtRlp = filter(20000,44100,3,lowpass);  
 
 DRC btAudio::_DRCL = DRC(44100,30,0.01,0.2,10,10,0); 
 DRC btAudio::_DRCR = DRC(44100,30,0.01,0.2,10,10,0); 
 //File btAudio::_file = SD.open("init.txt",FILE_WRITE);

btAudio::btAudio(const char *devName) {
  _devName=devName;  
}

void btAudio::begin() {
	
  //Arduino bluetooth initialisation
  btStart();

  // bluedroid  allows for bluetooth classic
  esp_bluedroid_init();
  esp_bluedroid_enable();
   
    //set up device name
  esp_bt_dev_set_device_name(_devName);
  
  // this sets up the audio receive
  esp_a2d_sink_init();

  // set discoverable and connectable mode, wait to be connected
  esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);  	
}

void btAudio::end() {
  esp_a2d_sink_deinit();
  esp_bluedroid_disable();
  esp_bluedroid_deinit();
  btStop();  
}

void btAudio::I2S(int bck, int dout, int ws) {
   // i2s configuration
  static const i2s_config_t i2s_config = {
    .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = static_cast<i2s_comm_format_t>(I2S_COMM_FORMAT_I2S|I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // default interrupt priority
    .dma_buf_count = 6,
    .dma_buf_len = 100,
    .use_apll = true,
    .tx_desc_auto_clear = true
  };
  
  // i2s pinout
  static const i2s_pin_config_t pin_config = {
    .bck_io_num =bck,//26
    .ws_io_num = ws, //27
    .data_out_num = dout, //25
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  
  // now configure i2s with constructed pinout and config
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  
  // Sets the function that will handle data (i2sCallback)
   esp_a2d_sink_register_data_callback(i2sCallback);
}

void btAudio::createFilter(int n, float fc, int type){
   fc=constrain(fc,2,20000);
   switch (type) {
   case lowpass:
	_filtLlp= filter(fc,44100,n,type);
    _filtRlp= filter(fc,44100,n,type);
   break;
   case highpass:
	_filtLhp= filter(fc,44100,n,type);
    _filtRhp= filter(fc,44100,n,type);
   break;
   }
	_filtering=true;
	
	if(_filtering & _compressing){
	 _postprocess=3;	
	}else{
	 _postprocess=1;
	}
}

void btAudio::stopFilter(){
	_filtering=false;
	if(_compressing){
	  _postprocess = 2;
	}else{
	 _postprocess = 0;
	}
}

void btAudio::compress(float T,float alphAtt,float alphRel, float R,int w,int mu){
	_DRCL = DRC(44100,T,alphAtt,alphRel,R,w,mu);
	_DRCR = DRC(44100,T,alphAtt,alphRel,R,w,mu);
	_compressing=true;
	
	if(_filtering & _compressing){
	 _postprocess=3;	
	}else{
	 _postprocess=2;
	}	
}

void btAudio::decompress(){
	_compressing=false;
	
	if(_filtering){
	  _postprocess = 1;
	}else{
	 _postprocess = 0;
	}
}

void btAudio::volume(float vol){
	_vol = constrain(vol,0,1);	
}

void btAudio::i2sCallback(const uint8_t *data, uint32_t len){
  size_t i2s_bytes_write = 0; 
  int16_t* data16=(int16_t*)data; //playData doesnt want const
  int jump =4; //how many bytes at a time get sent to buffer
  int  n = len/jump; // number of byte chunks
  int16_t fy[2];    // array to send via i2s(Left and Right channel)

  switch (_postprocess) {
   case NOTHING:
        for(int i=0;i<n;i++){
    	 //process left channel
		 fy[0] = (int16_t)((*data16)*_vol);
		 data16++;
		 // process right channel
		 fy[1] = (int16_t)((*data16)*_vol);
		 data16++;
		 i2s_write(I2S_NUM_0, fy, jump, &i2s_bytes_write,  10 ); 
		}
		break;
   case FILTER:
		for(int i=0;i<n;i++){
		 //process left channel
		 fy[0] = (*data16);
		 fy[0] = (int16_t)_filtLlp.process(fy[0]*_vol);
		 fy[0] = _filtLhp.process(fy[0]);
		 data16++;
		 // process right channel
		 fy[1] = (*data16);
		 fy[1] = (int16_t)_filtRlp.process(fy[1]*_vol);
		 fy[1] = _filtRhp.process(fy[1]);
		 data16++; 
		 i2s_write(I2S_NUM_0, fy, jump, &i2s_bytes_write,  10 );
		} 
		break;
   case COMPRESS:
	    for(int i=0;i<n;i++){
		 //process left channel
		 fy[0] = (*data16);
		 
		 fy[0]=_DRCL.softKnee(fy[0]*_vol);
		 data16++;
		 // process right channel
		 fy[1] = (*data16);
		 fy[1]=_DRCR.softKnee(fy[1]*_vol);
		 data16++;
		 i2s_write(I2S_NUM_0, fy, jump, &i2s_bytes_write,  10 );
		}
		break;
   case FILTER_COMPRESS:
		for(int i=0;i<n;i++){
		 //process left channel
		 fy[0] = (*data16);
		 fy[0] = _filtLlp.process(fy[0]);
		 fy[0] = _filtLhp.process(fy[0]);
		 fy[0] = _DRCL.softKnee(fy[0]*_vol);
		 data16++;
		 // process right channel
		 fy[1] = (*data16);
		 fy[1] = _filtRlp.process(fy[1]);
		 fy[1] = _filtRhp.process(fy[1]);
		 fy[1] = _DRCR.softKnee(fy[1]*_vol);
		 data16++;
		 i2s_write(I2S_NUM_0, fy, jump, &i2s_bytes_write,  10 );
		}
		break;
   case RECORD:
		// _file.write(data, len);
		break;		
  }
}


void btAudio::stopRecord(){
	//_postprocess=0;
	//_file.close();
	//enableCore0WDT(); 
}

void btAudio::record(const char * path){
   /*disableCore0WDT();
     if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
	uint8_t header[44]={82,73,70,70,86,237,221,3,87,65,86,69,102,109,116,32,16,0,0,0,1,0,2,0,68,172,0,0,16,177,2,0,4,0,16,0,100,97,116,97,255,255,255,255};
    _file = SD.open(path,FILE_WRITE);
	_file.write(header, 44);
	_postprocess = 4;
	//_file.close(); */
}

int  btAudio::postProc(){
		return _postprocess;
	} 