#ifndef DRC_H
#define DRC_H

#include "Arduino.h"
class DRC {
	public:
	DRC(float fs, float T,float tauAtt,float tauRel, float R,float w, float mu);
	int16_t softKnee(float x);
	
	private:
	float log10f_approx(float X);
	float _T;
    float _alphAtt;
	float _alphRel;
	float _slope;
	float _yprev;
	float _w; 
	float _mu;
	float _w4;
 	
};
#endif