#include<webDSP.h>
webDSP::webDSP() {

}
 void webDSP::begin(const char* ssid, const char* password, btAudio* audio){
  WiFi.mode(WIFI_STA);
  while(WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(1000);
    Serial.print(".");
  }
  _audio=audio;
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
 _server.on("/", [this]() { this->handleRoot(); });
 _server.on("/workStyle.css", [this]() { this->handleCss(); });
 _server.on("/get", [this]() { this->handleGet(); });
 _server.begin();
 }
 
  void webDSP::handleRoot() {
  _server.send(200, "text/html", html2);
}
  void webDSP::handleCss() {
 _server.send_P(200, "text/css", css2);
}
  void webDSP::handleGet(){
   String inputName = _server.argName(0);
   String inputVal = _server.arg(0);
   
   char sw = inputName.charAt(0);
   float cT= _audio->_T;
   float caa= _audio->_alphAtt;
   float car= _audio->_alphRel;
   float cR= _audio->_R;
   float cw= _audio->_w;
   float cm= _audio->_mu;
  
  _server.send_P(200, "text/html", html2);
  Serial.println(inputName);
  switch(sw) {
    case 'h': 
    _audio->createFilter(3, inputVal.toFloat(), highpass);
      break;
    case 'l': 
      _audio->createFilter(3, inputVal.toFloat(), lowpass);
      break;
    case 'c': 
      _audio->decompress();
      break;
    case 'g':   
      _audio->compress(cT,caa,car,cR,cw,inputVal.toFloat());
      break;
    case 't': 
       _audio->compress(inputVal.toFloat(),caa,car,cR,cw,cm);
      break;
    case 'a': 
      _audio->compress(cT,inputVal.toFloat(),car,cR,cw,cm);
       break;
    case 'r': 
     _audio->compress(cT,caa,inputVal.toFloat(),cR,cw,cm);
       break;
    case 'w': 
     _audio->compress(cT,caa,car,cR,inputVal.toFloat(),cm);
        break;
    case 'f': 
      _audio->stopFilter(); 
      break;
    case 'R':
     _audio->compress(cT,caa,car,inputVal.toFloat(),cw,cm);
        break;
    case 'v':
      _audio->volume(inputVal.toFloat());
      break;
    default:
      break;
  }
  
  }