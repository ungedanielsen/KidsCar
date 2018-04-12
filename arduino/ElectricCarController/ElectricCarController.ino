
#include <math.h>
#include <SPI.h>

//Relayes
#define RelayOn 3
#define EnableGas 2


//SPI
byte address = 0x11;

int CS = 10;

int input[3];
int gear = 0;
int startbyte;
int device;          
int val = 130;            // 0-254
int speed = 0;
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
  Serial.begin(115200);
  digitalWrite(RelayOn, HIGH);
  digitalWrite(EnableGas, HIGH);

}

void loop()
{
  unsigned long currentMillis = millis();

  //Check if we have recieved a command the last x ms. If not, disable the car.
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(RelayOn, HIGH);
  }

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

      
            
      switch (device)
      {
        case 1:
          steering = val;
          previousMillis = currentMillis;
          break;

        case 2: //Throttle
          Serial.println("WHY???");
          speed = val;
          breakPedal = 0;
          previousMillis = currentMillis;
          break;

        case 3: //Break
          breakPedal = val;
          breakPedal = (int)map(breakPedal,0,254,140,70);
          break;

        case 4:
          if (enableMotor) {
            enableMotor = false;
            digitalWrite(RelayOn, HIGH);
          }
          else {
            enableMotor = true;
            digitalWrite(RelayOn, LOW);
          }
          break;

        case 12: //Gear up
          if (gear < 5) gear++;
          //speed = 0;
          break;
        case 13: //Gear down
          if (gear > -1) gear--;
          //speed = 0;
          break;
        case 254: //Initiate car
          if (val == 254) {}
            digitalWrite(RelayOn, LOW);

          break;
      }

      switch (gear) {
        case -1:
          setSpeed = (int)map(speed,0,254,130,70);
          break;
        case 0:
          setSpeed = 130;
          break;
        case 1:
          setSpeed = (int)map(speed,0,254,188,201);
          break;
        case 2:
          setSpeed = (int)map(speed,0,254,188,214);
          break;
        case 3:
          setSpeed = (int)map(speed,0,254,188,227);
          break;
        case 4:
          setSpeed = (int)map(speed,0,254,188,240);
          break;
        case 5:
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
      
      digitalPotWrite(CS,setSpeed);
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
 
