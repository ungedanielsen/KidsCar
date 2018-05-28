
#include <math.h>
#include <SPI.h>

#define RC_CH1  0
#define RC_CH2  1
#define RC_CH3  2
#define RC_CH4  3
#define RC_NUM_CHANNELS  4

//Relayes
#define RelayOn 3
#define EnableGas 2
#define rfThrottlePin 4
#define rfStearingPin 5
#define rfOverridePin 6
#define motorPWM 7
#define motorIN1 8
#define motorIN2 9

#define rfReversePin 21
#define manualOverridePin 8


//SPI
int test;
byte address = 0x11;
int CS = 10;
bool radioPresent = true;
bool radioOverride = true;
int manualOverride = false;
int rfOverride = 0;
int rfThrottle = 0;
int rfStearing = 0;
int rfReverse = 0;
int Radiospeed = 0;
int input[3];
int gear = 1;
int startbyte;
int device;          
int val = 130;            // 0-254
int speed = 130;
int setSpeed = 0;
int breakPedal = 0;
int steering;
int stearTo = 67;
int i;
bool enableMotor = false;
unsigned long previousMillis = 0;
const long interval = 500;

//steering
int steeringMaxLeft = 0;
int steeringMaxRight = 985;
int steeringCurrentPos = 493;

//start change
const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;
char messageFromPC[buffSize] = {0};
int newFlashInterval = 0;
float servoFraction = 0.0; // fraction of servo range to move
unsigned long curMillis;
unsigned long prevReplyToPCmillis = 0;
unsigned long replyToPCinterval = 10;

void getDataFromRaspberry() {

    // receive data from PC and save it into inputBuffer
    
  if(Serial.available() > 0) {

    char x = Serial.read();

      // the order of these IF clauses is significant
      
    if (x == endMarker) {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }
    
    if(readInProgress) {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) {
        bytesRecvd = buffSize - 1;
      }
    }

    if (x == startMarker) { 
      bytesRecvd = 0; 
      readInProgress = true;
    }
  }
}

void parseData() {
    // split the data into its parts
    
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(inputBuffer,",");      // get the first part - the string
  speed = atoi(strtokIndx); // copy it to messageFromPC
  
  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  steering = atoi(strtokIndx);     // convert this part to an integer
  //Serial.println(steering);
  
  strtokIndx = strtok(NULL, ","); 
  gear = atoi(strtokIndx);     // convert this part to a float

}


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
  pinMode(rfReversePin, INPUT);
  pinMode(manualOverride, INPUT);
  Serial.begin(57600);
  digitalWrite(RelayOn, HIGH);
  digitalWrite(EnableGas, HIGH);
  pinMode(A0, INPUT);
  pinMode(motorPWM, OUTPUT);
  pinMode(motorIN1, OUTPUT);
  pinMode(motorIN2, OUTPUT);
}
void steer() {
    //Serial.print("CurrentPos: ");
    //Serial.print(steeringCurrentPos);
    //Serial.print(" Target:");
    //Serial.println(stearTo);
    steeringCurrentPos = map(analogRead(A0), 10, 950,0,254);
    if (abs(stearTo-steeringCurrentPos) < 10){
      analogWrite(motorPWM, 127);
    }
    else {
      analogWrite(motorPWM, 255);
    }

  
  if (abs(stearTo-steeringCurrentPos) < 2) {
    //stop
    digitalWrite(motorIN1,LOW);
    digitalWrite(motorIN2,LOW);
    analogWrite(motorPWM, 0);
  }
  else if (stearTo > steeringCurrentPos) {
    //Right
    digitalWrite(motorIN1,HIGH);
    digitalWrite(motorIN2,LOW);
    
  }
  else if (stearTo < steeringCurrentPos) {
    //Left
    digitalWrite(motorIN1,LOW);
    digitalWrite(motorIN2,HIGH);
  }


  

}
void replyToRaspberry() {

  if (newDataFromPC) {
    newDataFromPC = false;
    Serial.print("<");
    Serial.print(speed);
    Serial.print(",");
    Serial.print(steering);
    Serial.print(",");
    Serial.print(gear);
    Serial.println(">");
  }
}

void loop()
{
  
  curMillis = millis();
  getDataFromRaspberry();
  steer();
  if (curMillis-previousMillis > interval) {
    previousMillis=curMillis;
    
    
    ReadRadio();
  }
  //radioPresent = true;
  controlCar();
} //end main loop
 

 int digitalPotWrite(int pin,int value)
{
  digitalWrite(pin, LOW);
  SPI.transfer(address);
  SPI.transfer(value);
  digitalWrite(pin, HIGH);
}

int controlCar() {
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

    stearTo = (int)map(steering, 0, 254, 254, 0);
    //Serial.println(stearTo);

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
      digitalWrite(EnableGas, LOW);
      digitalPotWrite(CS, setSpeed);
    }
    else {
      digitalWrite(RelayOn, HIGH);
      digitalWrite(EnableGas, HIGH);
    }
    
    
}
int ReadRadio() {
  
  //rfOverride = pulseIn(rfOverridePin, HIGH, 3000);
  //rfThrottle = pulseIn(rfThrottlePin, HIGH, 3000);
  //rfStearing = pulseIn(rfStearingPin, HIGH, 3000);
  //rfReverse = pulseIn(rfReversePin, HIGH, 3000);
  Serial.print("rfOverride: ");
  Serial.print(rfOverride);
  Serial.print(", rfThrottle: ");
  Serial.print(rfThrottle);
  Serial.print(", rfStearing: ");
  Serial.print(rfStearing);
  Serial.print(", rfReverse: ");
  Serial.println(rfReverse);
  
  //manualOverride = digitalRead(manualOverridePin);
  manualOverride = 1;


  //Check if radio is present and that we get steady values. /Noice happens
 
  
  if (rfOverride > 1000 && rfThrottle > 1000 && rfThrottle < 1730 && rfStearing > 1000 && rfStearing < 1750) {
    radioPresent = true;
    if (rfOverride > 1200) {
      radioOverride = true;
    }
    else {
      radioOverride = false;
    }
  }
  else {
    radioPresent = false;
    
  }
  radioPresent = true;
  radioOverride = false;
  


    if (rfReverse > 1100) {
      Radiospeed = (int)map(rfThrottle, 1200,1720,188,253);
    }
    else {
      Radiospeed = (int)map(rfThrottle, 1200,1720,130,70);
    }


    
}
 
