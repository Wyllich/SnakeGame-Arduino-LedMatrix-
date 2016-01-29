#include <SoftwareSerial.h>
#include <Wire.h>

SoftwareSerial mySerial =  SoftwareSerial(10, 11);

#define upButton A5
#define downButton A4
#define leftButton A3 
#define rightButton A2

void setup(){
  mySerial.begin(4800);
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void loop(){
   /*Controlling the data given by the diffrent pins 
   
   Serial.print(analogRead(upButton));
   Serial.print(' ');
   Serial.print(analogRead(downButton));
   Serial.print(' ');
   Serial.print(analogRead(leftButton));
   Serial.print(' ');
   Serial.println(analogRead(rightButton)); */
   
   
   /*Calibrating the thresholds in order to send to send the correct keys. 
   Each settings require its own set of thresholds values. Use the previous control commands to find the ones that fit */
  
  if(analogRead(upButton)<10){
     mySerial.write(1);
     Serial.println(1);
  }
  else if(analogRead(downButton)<10){
     mySerial.write(2);
     Serial.println(2);
  }
  else if(analogRead(rightButton)<10){
      mySerial.write(4);
      Serial.print(4);
  }
  else if(analogRead(leftButton)<10){
      mySerial.write(3);
      Serial.print(3);
  }
    delay(50);
}
