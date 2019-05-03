
import processing.serial.*;
import java.io.*;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;


Serial comPort;
//Serial PLX;
//int counter=0; // Helps to keep track of values sent.
int numItems=0; //Keep track of the number of values in text file
boolean sendStrings=false; //Turns sending on and off
int tempLine = 0;
String inString = "";
int indS = 0;

String textFileLines[];

int numLines = 0;
int p = 0;

void setup(){
 comPort = new Serial(this, Serial.list()[0], 9600);
 textFileLines =loadStrings("DATA PATH!!!! NEED TO BE CHANGE eg. C:/user/file.csv");
 numLines = textFileLines.length;

 
 comPort.write("100000");
     
     delay(500);
     delay(500);
  
 String a = comPort.readString();
 
 while(tempLine < numLines){
   String lineItems[]=splitTokens(textFileLines[tempLine%numLines], ",");
   numItems=lineItems.length;
   print("tempLine:");
   print(tempLine);
   print(",");
   for(int i = 0; i<numItems; i++){
     comPort.write(lineItems[i]);
     
     println(lineItems[i]);
     delay(500);
     delay(500);
  
     //String inBuffer = comPort.readString();
     String inBuffer = comPort.readStringUntil('\n');
     if(inBuffer != null){
       print(inBuffer);

     }
     
   }
   tempLine++;
 }
 println("end");
   
}
