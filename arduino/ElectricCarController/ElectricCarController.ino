
#include <math.h>

//Relayes
#define RelayOn 2
#define RelayForward 3
#define RelayBackward 4


int input[3];
int gear = 0;
int startbyte;
int device;          //the pin the servo is on
int val;            // 0-254
int speed = 0;
int lastVal = -100;
int i;

void setup()
{
  pinMode(RelayForward, OUTPUT);
  pinMode(RelayBackward, OUTPUT);
  
  digitalWrite(RelayForward, HIGH);
  digitalWrite(RelayBackward, HIGH);
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
        case 1:
                 break;
        case 2:  speed = val;
                 break;
        case 12: if (gear < 5) gear++;
                 break;
        case 13: if (gear > -1) gear--;
                 break;
      }

      switch (gear) {
        case -1:
          speed = (int)map(speed,0,254,0,51);
          digitalWrite(RelayForward, HIGH);
          digitalWrite(RelayBackward, LOW);
        case 0:
          speed = 0;
          digitalWrite(RelayBackward, HIGH);
          digitalWrite(RelayForward, HIGH);
          break;
        case 1:
          speed = (int)map(speed,0,254,0,51);
          digitalWrite(RelayBackward, HIGH);
          digitalWrite(RelayForward, LOW);
          break;
        case 2:
          speed = (int)map(speed,0,254,0,102);
          digitalWrite(RelayBackward, HIGH);
          digitalWrite(RelayForward, LOW);
          break;
        case 3:
          speed = (int)map(speed,0,254,0,153);
          digitalWrite(RelayBackward, HIGH);
          digitalWrite(RelayForward, LOW);
          break;
        case 4:
          speed = (int)map(speed,0,254,0,205);
          digitalWrite(RelayBackward, HIGH);
          digitalWrite(RelayForward, LOW);
          break;
        case 5:
          speed = (int)map(speed,0,254,0,255);
          digitalWrite(RelayBackward, HIGH);
          digitalWrite(RelayForward, LOW);
          break;
      }
      Serial.print("Device = ");
      Serial.print(device);
      Serial.print(" - Value = ");
      Serial.print(val);
      Serial.print(" - Speed = ");
      Serial.print(speed);
      Serial.print(" - Gear = ");
      Serial.println(gear);
     
        
      
    }
   }
 }
 
