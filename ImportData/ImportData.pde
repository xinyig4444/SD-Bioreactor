/* TextFile Sender: Written by Scott C on 5th April 2013
 using Processing Version 2.0b8 */

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
 //background(255,0,0); //Start with a Red background
 textFileLines =loadStrings("D:/4444/Study/UA/ECE/SeniorDesign/test/testIn.csv");
 numLines = textFileLines.length;
 //for(int i = 0; i < numLines; i++){
 //  println(textFileLines[i]);
 //}
 //println(numLines);
 
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
       //if(tempLine != 0){
       //inString = inString+" ";
       //}
       inString = inString + lineItems[i] + "," + inBuffer;
        String[] inS = split(inString, " ");
        saveStrings("D:/4444/Study/UA/ECE/SeniorDesign/test/testOut.csv", inS);
     }
     
   }
   tempLine++;
 }
 println("end");
 
 //PLX = new Serial(this, "COM6", 14400);
 
 //PLX.write("1000");
 
 //PLX.write("CLEARDATA"); //clears up any data left from previous projects
 //PLX.write("LABEL,Time,timer,V,S,..."); //always write LABEL, so excel knows the next things will be the names of the columns (instead of Acolumn you could write Time for instance)
 //PLX.write("RESETTIMER"); //resets timer to 0
 
 //while(true){
 //    String inBuffer = comPort.readStringUntil('\n');
 //    PLX.write(inBuffer);
 //}
   
}
