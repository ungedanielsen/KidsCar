
#include <math.h>
#include <SPI.h>

//Relayes
#define RelayOn 3
#define EnableGas 2
#define rfThrottlePin 4
#define rfStearingPin 5
#define rfOverridePin 6

//SPI
byte address = 0x11;
int CS = 10;
bool radioPresent = false;
bool radioOverride = true;
int rfOverride = 0;
int rfThrottle = 0;
int input[3];
int gear = 1;
int startbyte;
int device;          
int val = 130;            // 0-254
int speed = 130;
int setSpeed = 0;
int breakPedal = 0;
int steering;
int i;
bool enableMotor = false;
unsigned long previousMillis = 0;
const long interval = 100000;

void setup()
{
  pinMode(CS, OUTPUT);
  SPI.begin();
  digitalPotWrite(CS, val);
  delay(1000);
  pinMode(EnableGas, OUTPUT);
  pinMode(RelayOn, OUTPUT);
  pinMode(rfOverridePin, INPUT);
  pinMode(rfThrottlePin, INPUT);
  pinMode(rfStearingPin, INPUT);
  Serial.begin(57600);
  digitalWrite(RelayOn, HIGH);
  digitalWrite(EnableGas, HIGH);

}

void loop()
{
  rfOverride = pulseIn(rfOverridePin, HIGH, 25000);
  rfThrottle = pulseIn(rfThrottlePin, HIGH, 25000);
  //Check if radio is present and that we get steady values. /Noice happens
  if (rfOverride < 1010 && rfThrottle > 1010) {
    radioPresent = false;
  }
  else {
    radioPresent = true;
  }
  if (rfOverride > 1200) {
    radioOverride = true;
  }
  else {
    radioOverride = false;
  }


  unsigned long currentMillis = millis();


  //Check if we have recieved a command the last x ms. If not, disable the car.
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(RelayOn, HIGH);
  }

  //Wait for serial input
  if (Serial.available() > 4)
  {
    // Read the first byte
    startbyte = Serial.read();
    speed = Serial.read();
    steering = Serial.read();
    gear = Serial.read();

    //If it's 255 then continue
    
    if (startbyte == 255)
    {
      char str[5];
      str[0] = speed;
      str[1] = steering;
      str[3] = gear;
      str[4] = '\0';
      Serial.print(str);
      Serial.print(speed);
      Serial.print(steering);
      Serial.print(gear);
      Serial.println("\n");
      Serial.print("OverRide: ");
      Serial.println(rfOverride);
      Serial.print("rfTrottle: ");
      Serial.println(rfThrottle);

      if (radioOverride) {
        //Read radio to override
      }

      switch (gear) {
        case 0:
          setSpeed = (int)map(speed,0,254,130,70);
          break;
        case 1:
          setSpeed = 130;
          break;
        case 2:
          setSpeed = (int)map(speed,0,254,188,201);
          break;
        case 3:
          setSpeed = (int)map(speed,0,254,188,214);
          break;
        case 4:
          setSpeed = (int)map(speed,0,254,188,227);
          break;
        case 5:
          setSpeed = (int)map(speed,0,254,188,240);
          break;
        case 6:
          setSpeed = (int)map(speed,0,254,188,253);
          break;
      }
      if (setSpeed == 188) {
        setSpeed = 130;
      }

      if (setSpeed == 130) {
        digitalWrite(EnableGas, HIGH);
      }
      else {
        digitalWrite(EnableGas, LOW);
      }
      
      if (radioPresent) {
        digitalWrite(RelayOn, LOW);
        digitalPotWrite(CS,setSpeed);
      }
      else {
        digitalWrite(RelayOn, HIGH);
        digitalWrite(EnableGas, HIGH);
      }
      /*
      Serial.print("Device = ");
      Serial.print(device);
      Serial.print(" - Value = ");
      Serial.print(val);
      Serial.print(" - Speed = ");
      Serial.print(setSpeed);
      Serial.print(" - Gear = ");
      Serial.print(gear);
      Serial.print(" - Steering = ");
      Serial.println(steering);
      */
    }
   }
 }

 int digitalPotWrite(int pin,int value)
{
  digitalWrite(pin, LOW);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(pin, HIGH);
}
 
