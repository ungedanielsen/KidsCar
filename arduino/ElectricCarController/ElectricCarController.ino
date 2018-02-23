#include <Servo.h>

Servo servo1;      //Servo on pin 2


//int minPulse = 600;
//int maxPulse = 2400;
//int refreshTime = 20;

//Values

int centerServo;    //center servo position
//int pulseWidth;     //servo pulse width

int input[3];
int startbyte;
int device;          //the pin the servo is on
int val;            //commanded servo posistion, 0-180 degrees
int pulseRange;     //max pulse-min pulse
long lastPulse = 0; //recorded time (ms) of the last pulse
int i;

void setup()
{
  servo1.attach(2);
  Serial.begin(9600);
}

void loop()
{ 
  //Wait for serial input
  if (Serial.available() > 2)
  {
    // Read the first byte
    startbyte = Serial.read();
    //If it's 255 then continue
    if (startbyte == 255)
    {
      //then get the next two bytes
      for(i=0;i<2;i++)
      {
        input[i] = Serial.read();
      }
      
      device = input[0];
      
      //Second byte = which position?
      val = input[1];
      
      // Paket error checking and recovery
      if (val == 255)
      {
        val = 255;
      }
      Serial.println(val);
      
      switch (device)
      {
        case 1:  servo1.write(val); //move servo 1 to 'pos'
                 break;
      }
    }
   }
 }
 
