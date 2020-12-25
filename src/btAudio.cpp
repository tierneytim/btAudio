#include "btAudio.h"
////////////////////////////////////////////////////////////////////
////////////// Nasty statics for i2sCallback ///////////////////////
////////////////////////////////////////////////////////////////////
 float btAudio::_vol=0.95;
 uint8_t btAudio::_address[6];
 
 String btAudio::title="";
 String btAudio::album="";
 String btAudio::genre="";
 String btAudio::artist="";
 
 int btAudio::_postprocess=0;
 filter btAudio::_filtLhp = filter(2,44100,3,highpass); 
 filter btAudio::_filtRhp = filter(2,44100,3,highpass);
 filter btAudio::_filtLlp = filter(20000,44100,3,lowpass); 
 filter btAudio::_filtRlp = filter(20000,44100,3,lowpass);  
 DRC btAudio::_DRCL = DRC(44100,60.0,0.001,0.2,4.0,10.0,0.0); 
 DRC btAudio::_DRCR = DRC(44100,60.0,0.001,0.2,4.0,10.0,0.0); 

////////////////////////////////////////////////////////////////////
////////////////////////// Constructor /////////////////////////////
////////////////////////////////////////////////////////////////////
btAudio::btAudio(const char *devName) {
  _devName=devName;  
}

////////////////////////////////////////////////////////////////////
////////////////// Bluetooth Functionality /////////////////////////
////////////////////////////////////////////////////////////////////
void btAudio::begin() {
	
  //Arduino bluetooth initialisation
  btStart();

  // bluedroid  allows for bluetooth classic
  esp_bluedroid_init();
  esp_bluedroid_enable();
   
  //set up device name
  esp_bt_dev_set_device_name(_devName);
  
  // initialize AVRCP controller
  esp_avrc_ct_init();
  esp_avrc_ct_register_callback(avrc_callback);
  
  // this sets up the audio receive
  esp_a2d_sink_init();
  
  esp_a2d_register_callback(getAddress);
  
  // set discoverable and connectable mode, wait to be connected
  esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);  	
}
void btAudio::end() {
  esp_a2d_sink_deinit();
  esp_bluedroid_disable();
  esp_bluedroid_deinit();
  btStop();  
}
void btAudio::getAddress(esp_a2d_cb_event_t event, esp_a2d_cb_param_t*param){
	
	 switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT:{
        esp_a2d_cb_param_t *a2d = (esp_a2d_cb_param_t *)(param);
       
        uint8_t* temp= a2d->conn_stat.remote_bda;
        _address[0]= *temp;
		_address[1]= *(temp+1);
		_address[2]= *(temp+2);
		_address[3]= *(temp+3);
		_address[4]= *(temp+4);
		_address[5]= *(temp+5);    
        break;
    }
    default:
        log_e("a2dp invalid cb event: %d", event);
        break;
    }
}
void btAudio::updateMeta() {
  uint8_t attr_mask = ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST | ESP_AVRC_MD_ATTR_ALBUM | ESP_AVRC_MD_ATTR_GENRE;
  esp_avrc_ct_send_metadata_cmd(1, attr_mask);
}
void btAudio::avrc_callback(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param) {
  esp_avrc_ct_cb_param_t *rc = (esp_avrc_ct_cb_param_t *)(param);
  char *attr_text;
  String mystr;

  switch (event) {
    case ESP_AVRC_CT_METADATA_RSP_EVT: {
      attr_text = (char *) malloc (rc->meta_rsp.attr_length + 1);
      memcpy(attr_text, rc->meta_rsp.attr_text, rc->meta_rsp.attr_length);
      attr_text[rc->meta_rsp.attr_length] = 0;
      mystr = String(attr_text);

      switch (rc->meta_rsp.attr_id) {
        case ESP_AVRC_MD_ATTR_TITLE:
          //Serial.print("Title: ");
          //Serial.println(mystr);
		  title= mystr;
          break;
        case ESP_AVRC_MD_ATTR_ARTIST:
          //Serial.print("Artist: ");
          //Serial.println(mystr);
          artist= mystr;
		  break;
        case ESP_AVRC_MD_ATTR_ALBUM:
          //Serial.print("Album: ");
          //Serial.println(mystr);
          album= mystr;
		  break;
        case ESP_AVRC_MD_ATTR_GENRE:
          //Serial.print("Genre: ");
          //Serial.println(mystr);
          genre= mystr;
		  break;
      }
      free(attr_text);
  }break;
    default:
      ESP_LOGE(BT_RC_CT_TAG, "unhandled AVRC event: %d", event);
      break;
  }
}

////////////////////////////////////////////////////////////////////
////////////////// I2S Audio Functionality /////////////////////////
////////////////////////////////////////////////////////////////////
void btAudio::I2S(int bck, int dout, int ws) {
   // i2s configuration
  static const i2s_config_t i2s_config = {
    .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = static_cast<i2s_comm_format_t>(I2S_COMM_FORMAT_I2S|I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // default interrupt priority
    .dma_buf_count = 3,
    .dma_buf_len = 600,
    .use_apll = false,
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
void btAudio::i2sCallback(const uint8_t *data, uint32_t len){
  size_t i2s_bytes_write = 0; 
  int16_t* data16=(int16_t*)data; //playData doesnt want const
  int16_t fy[2];
  float temp;
  
  int jump =4; //how many bytes at a time get sent to buffer
  int  n = len/jump; // number of byte chunks	
	switch (_postprocess) {
   case NOTHING:
        for(int i=0;i<n;i++){
		 //process left channel
		 fy[0] = (int16_t)((*data16)*_vol);
		 data16++;
		 
		 // process right channel
		 fy[1] = (int16_t)((*data16)*_vol);
		 data16++;
		 i2s_write(I2S_NUM_0, fy, jump, &i2s_bytes_write,  100 ); 
		}
		break;
   case FILTER:
		for(int i=0;i<n;i++){
		 //process left channel
		 temp = _filtLlp.process(_filtLhp.process((*data16)*_vol));
		 
		 // overflow check
		 if(temp>32767){
		 temp=32767;
		 }
		 if(temp < -32767){
			temp= -32767;
		 }
		 fy[0] = (int16_t)(temp);
	     data16++;
		 
		 // process right channel
		 temp = _filtRlp.process(_filtRhp.process((*data16)*_vol));
		 if(temp>32767){
		 temp=32767;
		 }
		 if(temp < -32767){
			temp= -32767;
		 }
		 fy[1] =(int16_t) (temp);
		 data16++; 
		 i2s_write(I2S_NUM_0, fy, jump, &i2s_bytes_write,  100 );
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
		 i2s_write(I2S_NUM_0, fy, jump, &i2s_bytes_write,  100 );
		}
		break;
   case FILTER_COMPRESS:
      for(int i=0;i<n;i++){
		 //process left channel(overflow check built into DRC)
		 fy[0] = _DRCL.softKnee(_filtLhp.process(_filtLlp.process((*data16)*_vol)));
		 data16++;
		 
		 //process right channel(overflow check built into DRC)
		 fy[1] = _DRCR.softKnee(_filtRhp.process(_filtRlp.process((*data16)*_vol)));
		 data16++;
		 i2s_write(I2S_NUM_0, fy, jump, &i2s_bytes_write,  100 );
		}
		break;	
  }

}
void btAudio::volume(float vol){
	_vol = constrain(vol,0,1);	
}

////////////////////////////////////////////////////////////////////
////////////////// Filtering Functionality /////////////////////////
////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////
////////////////// Compression Functionality ///////////////////////
////////////////////////////////////////////////////////////////////
void btAudio::compress(float T,float alphAtt,float alphRel, float R,float w,float mu){
	_T=T;
	_alphAtt=alphAtt;
	_alphRel=alphRel;
	_R=R;
	_w=w;
	_mu=mu;
	
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

