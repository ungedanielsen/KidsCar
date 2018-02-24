#include <Servo.h>

Servo servo1;      //Servo on pin 2


//int minPulse = 600;
//int maxPulse = 2400;
//int refreshTime = 20;

//Values

int centerServo;    //center servo position
//int pulseWidth;     //servo pulse width

int input[3];
int gear = 0;
int startbyte;
int device;          //the pin the servo is on
int val;            //commanded servo posistion, 0-180 degrees
int lastVal = -100;
int pulseRange;     //max pulse-min pulse
long lastPulse = 0; //recorded time (ms) of the last pulse
int i;

void setup()
{
  servo1.attach(2);
  servo1.write(180);
  Serial.begin(115200);
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

      if (lastVal = -100) lastVal = val;

      if (abs(lastVal-val)>2) {
        val = lastVal;
        Serial.print("bigger jump than 2");
      }
      // Paket error checking and recovery
      if (val == 255)
      {
        val = 255;
      }
      lastVal = val;
            
      switch (device)
      {
        case 1:  if (gear > 0) servo1.write(val); //move servo 1 to 'pos'
                 break;
        case 12: if (gear < 5) gear++;
                 break;
        case 13: if (gear > -1) gear--;
                 break;
      }
      Serial.print("Device = ");
      Serial.print(device);
      Serial.print(" - Value = ");
      Serial.print(val);
      Serial.print(" - Gear = ");
      Serial.println(gear);;

    }
   }
 }
 
