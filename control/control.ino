/*
 * Wiring
 

*/
#include <avr/io.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <CD74HC4067.h>


/* NEED TO MODIFY BY HAND */
#define TOTAL_DATA 20               // Personalized for each file
#define GAIN 1                      // Adjust the gain to amplifier the force being applied to the scaffold
#define T 200000                    // T = 1*E6ms / Frequency
#define HVR 0.5                     // The ratio of the force applied to horizontal and vertical
/* Stepper wiring */
// Vertical
#define PulseV 8
#define DirV 9
// Horizontal
#define PulseH 2
#define DirH 3

/* Strain guages variable */
// NEED TEST
#define R 3.2                     // microstrain change per step
#define V_EX 5000                 //excitation voltage in mV
#define GF 2.17                   //gauge factor
const double MULTIPLIER = 0.03125F; // 4x gain   +/- 1.024V  1 bit = 0.03125mV = MULTIPLIER
// adc
Adafruit_ADS1115 ads;
// MUX        // s0 s1 s2 s3
CD74HC4067 mux0(4, 5, 6, 7);
CD74HC4067 mux1(10, 11, 12, 13);
// Variables
long iStrain[36]; //initial Strain unit microStrain
long realStrain[36]; 
int numStr = 0;

/* Read File variables */
int S[TOTAL_DATA]; //converted to steps
int Strain[TOTAL_DATA];
int numS = 0;
int curS = 0;

/* Stepper variables */
long delay_Micros = 10;
long currentMicros = 0; 
long previousMicros = 0;

int currentStepV = 0;
int currentStepH = 0;


double CalcTempStrain(double v){
  double tStrain = - 1000000*( v/V_EX - 0.5)*4/GF;
  return tStrain;
}

void ReadInitStrain(){
    for(int j = 0; j < 3;j++){
      mux0.channel(j);
      for (int i = 0; i < 12; i++) {
          mux1.channel(i);
          /* measure strain */
          int16_t results;  double v;          
          results = ads.readADC_Differential_0_1(); v = results * MULTIPLIER;
          iStrain[numStr%36] = CalcTempStrain(v);
          numStr = (numStr+1)%36;
      }
  }
}

void ReadRealStrain(){
    for(int j = 0; j < 3;j++){
      mux0.channel(j);
      for (int i = 0; i < 12; i++) {
          mux1.channel(i);
          /* measure strain */
          int16_t results;  double v;
          results = ads.readADC_Differential_0_1();  v = results * MULTIPLIER;       
          realStrain[numStr%36] = CalcTempStrain(v) - iStrain[numStr%36];
          numStr = (numStr+1)%36;
      }
  }
  Serial.print("DATA,TIME,TIMER,"); //writes the time in the first column A and the time since the measurements started in column B
  for (int i = 0; i < 36; i++){
    Serial.print(realStrain[i%36]);
    Serial.print(",");
    if(i==35)
      Serial.println(" ");
  }
}





void setup() {
  // initialize the serial communications:
  Serial.begin(9600);

  /* Read File */
  while(numS<TOTAL_DATA){
    if (Serial.available()) {
      delay(100);
      byte byteRead[5];
      int index = 0;
        while (Serial.available() > 0) {
          byteRead[index] = Serial.read() - '0';
          index++;
        }
        
        for(int k = 0; k < index;k ++){
          int b = byteRead[k];
          for(int j = 0; j < (index - k - 1); j++){
            b = b*10;
          }
          Strain[numS] = Strain[numS] + b;
        }
        S[numS] = (Strain[numS] * GAIN)/R;
        Serial.print("Steps:"); Serial.println(S[numS]);
        numS = numS+1;
    }
  }

  /* Stepper */
  pinMode(PulseV,OUTPUT); pinMode(DirV,OUTPUT); digitalWrite(DirV,HIGH);
  pinMode(PulseH,OUTPUT); pinMode(DirH,OUTPUT); digitalWrite(DirH,HIGH);

  /* Strain guages */
  ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.03125mV = MULTIPLIER
  ads.begin();
  ReadInitStrain();
}

void loop() {

  //TIME!!!!!!!!!!!!!!
  /* Reset */
  if(Serial.available()){
    char temp = Serial.read();
    if(temp == 't'){
        while(currentStepV > 0){
            currentMicros = micros();
            if(currentMicros - previousMicros >= delay_Micros){ //Protection
              previousMicros = currentMicros;
              digitalWrite(DirV,LOW);     delayMicroseconds(7);
              digitalWrite(PulseV,HIGH);  delayMicroseconds(13); //Set Value
              digitalWrite(PulseV,LOW);   currentStepV--;
            }
        }
        while(currentStepH > 0){
            currentMicros = micros();
            if(currentMicros - previousMicros >= delay_Micros){ //Protection
              previousMicros = currentMicros;
              digitalWrite(DirH,LOW);     delayMicroseconds(7);
              digitalWrite(PulseH,HIGH);  delayMicroseconds(13); //Set Value
              digitalWrite(PulseH,LOW);   currentStepH--;
            }
        }
        delay(1000);
        ReadInitStrain();
    }
  }

  long startMicros = micros();
  while((micros() - startMicros) < T ){
    
    /* Apply Force */
    //Vertical
    while(currentStepV != S[curS%TOTAL_DATA]){
      if(currentStepV < S[curS%TOTAL_DATA]){
        //take one step down
        currentMicros = micros();
          if(currentMicros - previousMicros >= delay_Micros){ //Protection
              previousMicros = currentMicros;
              digitalWrite(DirV,HIGH);     delayMicroseconds(7);
              digitalWrite(PulseV,HIGH);   delayMicroseconds(13); //Set Value
              digitalWrite(PulseV,LOW);    currentStepV++;
          }
      }
      if(currentStepV > S[curS%TOTAL_DATA]){
        //take one step down
        currentMicros = micros();
          if(currentMicros - previousMicros >= delay_Micros){ //Protection
              previousMicros = currentMicros;
              digitalWrite(DirV,LOW);     delayMicroseconds(7);
              digitalWrite(PulseV,HIGH);  delayMicroseconds(13); //Set Value
              digitalWrite(PulseV,LOW);   currentStepV--;
          }
      }
      if((micros()-startMicros) >  T )
        break;
    }
    //Horizontal
    while(currentStepH != S[curS%TOTAL_DATA]*HVR){
      if(currentStepH < S[curS%TOTAL_DATA]*HVR){
        //take one step down
        currentMicros = micros();
          if(currentMicros - previousMicros >= delay_Micros){ //Protection
              previousMicros = currentMicros;
              digitalWrite(DirH,HIGH);     delayMicroseconds(7);
              digitalWrite(PulseH,HIGH);   delayMicroseconds(13);
              digitalWrite(PulseH,LOW);    currentStepH++;
          }
      }
      if(currentStepH > S[curS%TOTAL_DATA]*HVR){
        //take one step down
        currentMicros = micros();
          if(currentMicros - previousMicros >= delay_Micros){ //Protection
              previousMicros = currentMicros;
              digitalWrite(DirH,LOW);     delayMicroseconds(7);
              digitalWrite(PulseH,HIGH);  delayMicroseconds(13);
              digitalWrite(PulseH,LOW);   currentStepH--;
          }
      }
      if((micros()-startMicros) > T )
        break;
    }
    delayMicroseconds(20);
  }

  /* Read from 36 strain guages */
  ReadRealStrain();

  curS = (curS+1)%TOTAL_DATA;
}



