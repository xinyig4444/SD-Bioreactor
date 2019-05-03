/*
 * Wiring
 

*/
#include <avr/io.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <CD74HC4067.h>


/* NEED TO MODIFY BY HAND */
#define TOTAL_DATA 30                // Personalized for each file
#define GAIN 1                      // Adjust the gain to amplifier the force being applied to the scaffold
#define T 200000                    // T = 1*E6ms / Frequency
#define HVR 1                     // The ratio of the force applied to horizontal and vertical
/* Stepper wiring */
#define PulseV 8
#define DirV 9

/* Strain guages variable */
// NEED TEST
#define R 3.2                     // microstrain change per step
#define V_EX 4000                 //excitation voltage in mV
#define GF 2.17                   //gauge factor
const double MULTIPLIER = 0.0078125F;
// adc
Adafruit_ADS1115 ads;
long iStrain = 0;
// MUX        // s0 s1 s2 s3
CD74HC4067 mux0(4, 5, 6, 7);
CD74HC4067 mux1(10, 11, 12, 50);
// Variables
//long iStrain[16]; //initial Strain unit microStrain
//long realStrain[16]; 
int numStr = 0; 

/* Read File variables */
int S[TOTAL_DATA]; //converted to steps
int Strain[TOTAL_DATA];
int numS = 0;
int curS = 0;

/* Stepper variables */
long delay_Micros = 1000;
long currentMicros = 0; 
long previousMicros = 0;

int currentStepV = 0;
int currentStepH = 0;


double CalcTempStrain(double v){
  double tStrain = - 1000000*( v/V_EX - 0.5)*4/GF;
  return tStrain;
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

  /* Strain guages */
  //ads.setGain(GAIN_SIXTEEN);
  ads.begin();
//  ReadInitStrain();

Serial.begin(9600);  
  
  Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");

   ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  
  ads.begin();

  int16_t results;
  double v;
  
  float multiplier = 0.0625F;  /* ADS1115  @ +/- 2.048V gain (16-bit results) */
  //float multiplier = 0.0078125F;  /* ADS1115  @ +/- 0.256V gain (16-bit results) */


          /* measure strain */
          results = ads.readADC_Differential_0_1();  
          v = results * multiplier;

  iStrain = CalcTempStrain(v);
  
}

void loop() {
  /* Reset */
  if(Serial.available()){
    char temp = Serial.read();
    if(temp == 't'){
        while(currentStepV > 0){
            currentMicros = micros();
            if(currentMicros - previousMicros >= delay_Micros){ //Protection
              previousMicros = currentMicros;
              digitalWrite(DirV,LOW);     delayMicroseconds(20);
              digitalWrite(PulseV,HIGH);  delayMicroseconds(100); //Set Value
              digitalWrite(PulseV,LOW);   currentStepV--;
            }
        }
        delay(1000);
//        ReadInitStrain();
    }
  }

  long startMicros = micros();
  while((micros() - startMicros) < T ){
    
    /* Apply Force */
    while(currentStepV != S[curS%TOTAL_DATA]*HVR){
      if(currentStepV < S[curS%TOTAL_DATA]*HVR){
        //take one step down
        currentMicros = micros();
          if(currentMicros - previousMicros >= delay_Micros){ //Protection
              previousMicros = currentMicros;
              digitalWrite(DirV,HIGH);     delayMicroseconds(20);
              digitalWrite(PulseV,HIGH);   delayMicroseconds(100); //Set Value
              digitalWrite(PulseV,LOW);    currentStepV++;
          }
      }
      if(currentStepV > S[curS%TOTAL_DATA]*HVR){
        //take one step down
        currentMicros = micros();
          if(currentMicros - previousMicros >= delay_Micros){ //Protection
              previousMicros = currentMicros;
              digitalWrite(DirV,LOW);     delayMicroseconds(20);
              digitalWrite(PulseV,HIGH);  delayMicroseconds(100); //Set Value
              digitalWrite(PulseV,LOW);   currentStepV--;
          }
      }
//      if((micros()-startMicros) >  T )
//        break;
    }
    delayMicroseconds(20);
  }

  /* Read from 16 strain guages */
//  ReadRealStrain();
  int16_t results;
  double v;
  long actualStrain = 0;

  float multiplier = 0.0625F;  /* ADS1115  @ +/- 2.048V gain (16-bit results) */
  //float multiplier = 0.0078125F;  /* ADS1115  @ +/- 0.256V gain (16-bit results) */

  /* measure strain */
  results = ads.readADC_Differential_0_1();  
  v = results * multiplier;



   if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == 't')
        iStrain = CalcTempStrain(v);
  }
  actualStrain = CalcTempStrain(v) - iStrain;

    /* Monitor how many steps we go */
    /* Linear to the strain value */
    
    Serial.print("DATA,TIME,TIMER,"); //writes the time in the first column A and the time since the measurements started in column B
    Serial.println(currentStepV); //print the step value

    /* Monitor the Strain value */
    /* Real-Time fdbk */
//  Serial.print("DATA,TIME,TIMER,");
//  Serial.print("Strain value:"); 
//  Serial.println(actualStrain);
//  delay(100);



  curS = (curS+1)%TOTAL_DATA;
}



