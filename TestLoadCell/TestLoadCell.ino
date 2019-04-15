#include <avr/io.h>
#include "HX711.h"

/* Stepper wiring */
#define Pulse 8
#define Dir 9

#define TOTAL_DATA 88

long S[TOTAL_DATA];
int i = 0;

/*  HX711 circuit wiring */
const int DOUT = 2;
const int CLK = 3;

/* HX711 load cell variables */
HX711 scale;
float calibration_factor = -28150; //29130
long zero_factor;
int tTare = 0;

/* Stepper variables */
long delay_Micros = 1800;
long currentMicros = 0; 
long previousMicros = 0;

long numStep = 200;

long currentStep = 0;


int Out = 0;





void setup() {
  Serial.begin(9600);
  

  /* Stepper */
  pinMode(Pulse,OUTPUT);
  pinMode(Dir,OUTPUT);
  digitalWrite(Dir,HIGH);

  /* HX711 */
  scale.begin(DOUT,CLK,128);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  zero_factor = scale.read_average(); //Get a baseline reading

S[0]=564;
S[1]=568;
S[2]=560;
S[3]=590;
S[4]=536;
S[5]=522;
S[6]=524;
S[7]=530;
S[8]=566;
S[9]=572;
S[10]=530;
S[11]=512;
S[12]=514;
S[13]=558;
S[14]=514;
S[15]=512;
S[16]=550;
S[17]=510;
S[18]=564;
S[19]=520;
S[20]=512;
S[21]=550;
S[22]=572;
S[23]=672;
S[24]=606;
S[25]=520;
S[26]=550;
S[27]=554;
S[28]=526;
S[29]=404;
S[30]=412;
S[31]=396;
S[32]=422;
S[33]=398;
S[34]=464;
S[35]=384;
S[36]=466;
S[37]=518;
S[38]=562;
S[39]=510;
S[40]=548;
S[41]=512;
S[42]=572;
S[43]=490;
S[44]=414;
S[45]=392;
S[46]=434;
S[47]=392;
S[48]=418;
S[49]=492;
S[50]=510;
S[51]=570;
S[52]=394;
S[53]=450;
S[54]=392;
S[55]=450;
S[56]=440;
S[57]=422;
S[58]=388;
S[59]=452;
S[60]=404;
S[61]=448;
S[62]=418;
S[63]=434;
S[64]=404;
S[65]=570;
S[66]=544;
S[67]=558;
S[68]=560;
S[69]=522;
S[70]=530;
S[71]=514;
S[72]=564;
S[73]=512;
S[74]=570;
S[75]=528;
S[76]=566;
S[77]=510;
S[78]=518;
S[79]=610;
S[80]=804;
S[81]=816;
S[82]=712;
S[83]=562;
S[84]=526;
S[85]=414;
S[86]=416;
S[87]=382;

for(int a = 0; a < TOTAL_DATA; a ++){
  S[a%TOTAL_DATA] = S[a%TOTAL_DATA]/4;
}

}

void loop() {

  double weight = 0.453592*scale.get_units();

  while(Out == 0){

  /* HX711 */
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Reading: ");
  weight = 0.453592*scale.get_units();
  Serial.print(weight, 3);
  Serial.print(" kg"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
  Serial.println(numStep);
  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a') //calibrate
      calibration_factor += 20;
    else if(temp == '-' || temp == 'z'){
      calibration_factor -= 20;
    }
    else if(temp == 't'){ // zero factor
      scale.tare();
      zero_factor = scale.read_average(); //Get a baseline reading
    }
    else if( temp == 'o'){
      Out = 1;
    }
    else if(temp == 'c'){ //clear the step number
      numStep = 0;
    }
    /* Motor */
    else if(temp == 'd'){
      currentMicros = micros();
      if(currentMicros - previousMicros >= delay_Micros){ //Protection
        previousMicros = currentMicros;
        digitalWrite(Dir,LOW);
        delayMicroseconds(7);
        digitalWrite(Pulse,HIGH);
        delayMicroseconds(50); //Set Value fro 500 to 50
        digitalWrite(Pulse,LOW);
        numStep++;
      }
    }
    else if((temp == 'u')&& (numStep > 0)){
      currentMicros = micros();
      if(currentMicros - previousMicros >= delay_Micros){ //Protection
        previousMicros = currentMicros;
        digitalWrite(Dir,HIGH);
        delayMicroseconds(7);
        digitalWrite(Pulse,HIGH);
        delayMicroseconds(50); //Set Value
        digitalWrite(Pulse,LOW);
        numStep--;
      }
    }
  }
  }


  /* test */
  long startMicros = micros();
  weight = 0.453592*scale.get_units();
  while((micros() - startMicros)<200000){
    
      while(currentStep != S[i%TOTAL_DATA]){
        if(currentStep < S[i%TOTAL_DATA]){
          //take one step down
          currentMicros = micros();
            if(currentMicros - previousMicros >= delay_Micros){ //Protection
                previousMicros = currentMicros;
                digitalWrite(Dir,LOW);
                delayMicroseconds(7);
                digitalWrite(Pulse,HIGH);
                delayMicroseconds(50); //Set Value
                digitalWrite(Pulse,LOW);
                currentStep++;
            }
        }
        if(currentStep > S[i%TOTAL_DATA]){
          //take one step up
          currentMicros = micros();
            if(currentMicros - previousMicros >= delay_Micros){ //Protection
                previousMicros = currentMicros;
                digitalWrite(Dir,HIGH);
                delayMicroseconds(7);
                digitalWrite(Pulse,HIGH);
                delayMicroseconds(50); //Set Value
                digitalWrite(Pulse,LOW);
                currentStep--;
            }
        }
        if((micros()-startMicros)>200000)
        break;
      }
      delayMicroseconds(20);
  }
  weight = 0.453592*scale.get_units();
  Serial.print("DATA,TIME,TIMER,"); //writes the time in the first column A and the time since the measurements started in column B
  //Serial.print(micros());
  Serial.println(weight,3);
  i= (i+1)%TOTAL_DATA;

}
