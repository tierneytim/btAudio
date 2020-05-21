// adapted from https://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
#ifndef FILTER_H
#define FILTER_H
#include <Arduino.h>
enum {
    lowpass = 0,
    highpass,
	bandpass,
};
class filter {
public:
    filter(float Fc,float fs, int ncascs,int type);
	
	template <typename T> 
	int16_t process(T in){
		float fout;
		float fprev= (float)in;
		for(int i=0; i<_ncascs;i++){
			fout= fprev * b0[i] + z1[i];
			z1[i] = fprev * b1[i] + z2[i] - a1[i] * fout;
			z2[i] = fprev * b2[i] - a2[i] * fout;
			fprev=fout;
		}
		return (int16_t)fout;
	}
	

private:
	float b0[5], b1[5], b2[5], a1[5], a2[5],z1[5], z2[5];
	int _ncascs;
}; 
#endif