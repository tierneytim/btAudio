void setup() {
 Serial.begin(115200);
 
// Create lookup table
float lu[199];
 for(int i = -99; i <100;i++){
  lu[i+99]=pow10f(i/20.0);
}

// test data ranges between -10 and 10 dB
float temp=-10.0;
float val[200];
for(int i = 0; i <200;i++){
  temp+=.1;
  val[i]= temp;
}


// exact solution 
float outvalEx[200];
unsigned long t1 =micros();
for(int i = 0; i <200;i++){
  outvalEx[i]= pow10f(val[i]/20.0f);
}
unsigned long t2 =micros();  
Serial.print("Exact Method: ");
Serial.print((t2-t1)/200.0);
Serial.println(" Microseconds per transform");


// approximate method 

float outvalAppi[200];

t1 =micros();
for(int i = 0; i <200;i++){
    outvalAppi[i]= lu[(int)val[i]+99];
}
t2=micros();
Serial.print("Approximate Method(lookup): ");
Serial.print((t2-t1)/200.0);
Serial.println(" Microseconds per transform");


// approximate method 
float frac;
float outvalApp[200];

t1 =micros();
for(int i = 0; i <200;i++){
  float input =val[i];
  if(input<0){
    int integ=  (int)input;
    integ-=1;
    frac= input-integ;
    outvalApp[i]= 1.0f+ (0.1146f+0.0074f*frac)*frac;
    outvalApp[i]*=lu[integ+99];
  }else{
    int integ=  (int)input;
    frac= input-integ;
    outvalApp[i]= 1.0f+ (0.1146f+0.0074f*frac)*frac;
    outvalApp[i]*=lu[integ+99];
  }
}
t2=micros();
Serial.print("Approximate Method: ");
Serial.print((t2-t1)/200.0);
Serial.println(" Microseconds per transform");

float err=0.0;
float maxerr=0.0;


// Approximation error(lookup)
for(int i = 0; i <200;i++){
  err= abs((outvalAppi[i]-outvalEx[i])/outvalEx[i]*100);
  if(err>maxerr){
    maxerr=err;
  }
}
Serial.print("Approximate method(lookup) max error is: ");
Serial.print(maxerr);
Serial.println("%");

maxerr=0;
// Approximation error(polynomial)
for(int i = 0; i <200;i++){
  err= abs((outvalApp[i]-outvalEx[i])/outvalEx[i]*100);
  if(err>maxerr){
    maxerr=err;
  }
}
Serial.print("Approximate method (polynomial) max error is: ");
Serial.print(maxerr);
Serial.println("%");

}

void loop() {
  // put your main code here, to run repeatedly:
delay(1000);
}
