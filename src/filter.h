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
	T process(T in){
		float fout;
		float fprev= (float)in;
		for(int i=0; i<_ncascs;i++){
			fout= fprev * _b0[i] + _z1[i];
			_z1[i] = fprev * _b1[i] + _z2[i] - _a1[i] * fout;
			_z2[i] = fprev * _b2[i] - _a2[i] * fout;
			fprev=fout;
		}
		return (T)fout;
	}
	

private:
	float _b0[5], _b1[5], _b2[5], _a1[5], _a2[5], _z1[5], _z2[5];
	int _ncascs;
}; 
#endif