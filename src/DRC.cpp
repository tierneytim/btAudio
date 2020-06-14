#include "DRC.h"

#define pow10f(x) expf(2.302585092994046f*x)

DRC::DRC(float fs, float T,float tauAtt,float tauRel, float R,float w, float mu){
		_T= T;
		_alphAtt=exp(-1/(tauAtt*fs));
		_alphRel=exp(-1/(tauRel*fs));
		_slope=1/R-1;
		_yprev=0;
		_w = constrain(w,1,90);
		_mu = mu;
		_w4= 1/(4*_w);
	}

// thanks you ! https://github.com/chipaudette/OpenAudio_ArduinoLibrary/blob/master/AudioEffectCompressor_F32.h
float DRC::log10f_approx(float X){
		float Y, F;
		int E;
		F = frexpf(fabsf(X), &E);
		Y = 1.23149591368684f;
		Y *= F;
		Y += -4.11852516267426f;
		Y *= F;
		Y += 6.02197014179219f;
		Y *= F;
		Y += -3.13396450166353f;
		Y += E;
		Y *=0.3010299956639812f;
  return(Y);
}

int16_t DRC::softKnee(float x){
		// signal to dB
	 	float dBx = fabsf(x);
		dBx+=1;
		dBx= 20.0*log10f_approx(dBx);
		
		// how far above threshold are we?
		float delta = dBx-_T;
		
		// if not above do nothing 
		float dBy = dBx;	
		
		//if above soft kneee compress linearly 
		if((delta-_w) >= 0){
		dBy += _slope*delta;	
		} else if (delta> -_w && delta<_w){ //if within knee compress quadratically
		delta+= _w;
		dBy += _slope*(delta*delta)*_w4;
		}
		
		// smooth difference between compressed and uncompressed data
		dBy-=dBx;
		if(dBy<_yprev){
			dBy = _alphAtt*_yprev+ (1-_alphAtt)*dBy;
			
		}else{
			dBy = _alphRel*_yprev+ (1-_alphRel)*dBy;
		}
		// save current value for next iteration
		_yprev=dBy;
		
		// add in make up gain
		dBy+=_mu;
		
		/*polynomial approximation (newtons divided differences)
		for fractional part of pow10f(dB/20). The integral part is 
		determined from a lookup table(G). Precision > 16bits.
		*/
		if(dBy<0){
			int integ=  (int)dBy;
			integ-=1;
			float frac= dBy-integ;
			dBy= frac*(frac*(0.0003027786f*frac+0.006535915f)+0.115179760f)+1.0f;
			dBy*=G[integ+96];
			dBy*=x;
		}else{
			int integ=  (int)dBy;
			float frac= dBy-integ;
			//dBy= 1.0f+ (0.1146f+0.0074f*frac)*frac;
			dBy= frac*(frac*(0.0003027786f*frac+0.006535915f)+0.115179760f)+1.0f;
			
			dBy*=G[integ+96];
			dBy*=x;
		}

		// constrain to be in int16_t range
		if(dBy>32767){
			dBy=32767;
		}else if (dBy < -32767){
			dBy= -32767;
		}
		// and return 16-bit signal */
		return (int16_t)(dBy);
}
