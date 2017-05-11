//necessary includes to use the RTC module
#include <Wire.h>
#include <SparkFunDS1307RTC.h>

#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>

//define table for the digit geometry, MSB first
#define DIG_0 0x3F 
#define DIG_1 0x06 
#define DIG_2 0x5B 
#define DIG_3 0x4F 
#define DIG_4 0x66
#define DIG_5 0x6D
#define DIG_6 0x7D 
#define DIG_7 0x07
#define DIG_8 0x7F
#define DIG_9 0x67
#define DIG_A 0x77
#define DIG_B 0x7C
#define DIG_C 0x39
#define DIG_D 0x5E
#define DIG_E 0x79
#define DIG_F 0x71
#define DIG_DOT 0x80

//state defines
#define STARTs   0x00
#define MAINs    0x01
#define SETMODEs 0x02
#define SETTINGs 0x03
#define ALARMs   0x04
#define ERRORs   0x05
#define DAYHALFs 0x06
#define SETALARMs 0x07
#define AUPDATEs 0x08

//the digit geometry values put into an array so they are easier to call by an int value
int displayCodes[17] = { DIG_0, DIG_1, DIG_2, DIG_3, DIG_4, DIG_5, DIG_6, DIG_7, DIG_8, DIG_9, DIG_A, DIG_B, DIG_C, DIG_D, DIG_E, DIG_F, DIG_DOT } ;

//the initial states that the machine will take
int current_state = STARTs;
int previous_state = STARTs;
int next_state = MAINs;

//variable to count and change the dot signifying that the clock is working
int dotcounter = 0;

//variables to hold the current state of the alarm
int alarmHours = 1;
int alarmMinutes = 1;
bool alarmIsPM = false;

//bool to effectively detect a separate press event
bool switch1high = true;
bool switch2high = true;
bool switch3high = true;

bool notAlarmSuppress = true;

Adafruit_MMA8451 mma = Adafruit_MMA8451();

bool interrupted = false;

void setup() {

  attachInterrupt(digitalPinToInterrupt(2), mmaInterrupt, RISING);
  //LED 7-segment outputs
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(13, OUTPUT);
  
  //RTC intialization
  rtc.begin();
  rtc.autoTime();
  rtc.set12Hour();
  
  pinMode(6, OUTPUT);
  if (! mma.begin()) {
    Serial.println("Couldnt start");
    
    while (1);
  }
  Serial.println("MMA8451 found!");
}

void loop() {
  previous_state = current_state;
  current_state = next_state;
  next_state = ERRORs;

  switch(current_state){
    case STARTs:
    {
      next_state = MAINs;
      break;
    }
      
    case MAINs:
    {
      updateTime();

      analogWrite(6, 0);

      if(!setButtonPressed1()){
        switch1high = true;
      }
      if(!setButtonPressed2()){
        switch2high = true;
      }
      if(!setButtonPressed3()){
        switch3high = true;
      }

      if(!alarmTime()){
        notAlarmSuppress = true;
      }

      
      if(alarmTime() && notAlarmSuppress){
        next_state = ALARMs;
      }else if(setButtonPressed1() && switch1high){
        next_state = SETMODEs;
        switch1high = false;
      }else if(setButtonPressed2() && switch2high){
        next_state = DAYHALFs;
        switch2high = false;
      }else if(setButtonPressed3() && switch3high){
        next_state = SETALARMs;
        switch3high = false;
      }else{
        next_state = MAINs;
      }
      break;
    }
      
    case SETMODEs:
    {
      updateTime();
      next_state = SETMODEs;
      if(!setButtonPressed1()){
        switch1high = true;
      }
      if(!setButtonPressed2()){
        switch2high = true;
      }
      if(!setButtonPressed3()){
        switch3high = true;
      }
      
      if(setButtonPressed1() && switch1high){
        next_state = MAINs;
        switch1high = false;
      }
      if(setButtonPressed2() && switch2high){
        next_state = SETTINGs;
        switch2high = false;
      }
      if(setButtonPressed3() && switch3high){
        next_state = SETTINGs;
        switch3high = false;
      }

      
      
      break;
    }
      
    case SETTINGs:
    {

      if(!switch2high){
        rtc.set24Hour();
        rtc.setHour(((rtc.hour() + 1) % 25));
        rtc.set12Hour();
      }else{
        rtc.set24Hour();
        rtc.setMinute(((rtc.minute() + 1) % 60));
        rtc.set12Hour();
      }
      next_state = SETMODEs;
      break;
    }
      
    case ALARMs:
    {
      updateTime();
      notAlarmSuppress = false;
      analogWrite(6, 126);
      //timecheck(); not needed anymore
      next_state = ALARMs;
      if(interrupted){
        next_state = MAINs;
        interrupted = false;
      }
      break;
    }
      
    case DAYHALFs:
    {
      
      printDayHalf();
      next_state = DAYHALFs;
      if(!setButtonPressed1()){
        switch1high = true;
      }
      if(!setButtonPressed2()){
        switch2high = true;
      }
      if(!setButtonPressed3()){
        switch3high = true;
      }
      
      if(setButtonPressed2() && switch2high){
        next_state = MAINs;
        switch2high = false;
      }
      break;
    }
      
     case SETALARMs:
    {
      shiftAlarms();
      next_state = SETALARMs;
      if(!setButtonPressed1()){
        switch1high = true;
      }
      if(!setButtonPressed2()){
        switch2high = true;
      }
      if(!setButtonPressed3()){
        switch3high = true;
      }
      
      if(setButtonPressed1() && switch1high){
        next_state = AUPDATEs;
        switch1high = false;
      }
      if(setButtonPressed2() && switch2high){
        next_state = AUPDATEs;
        switch2high = false;
      }
      if(setButtonPressed3() && switch3high){
        next_state = MAINs;
        switch3high = false;
      }
      break;
    }

    
    case AUPDATEs:
    {
     next_state = SETALARMs;
     if(!switch1high){
      if(alarmHours == 11){
        alarmIsPM = !alarmIsPM;
      }
      alarmHours = (alarmHours + 1) % 13;
      
     }else{
      alarmMinutes = (alarmMinutes + 1) % 60; 
     }
     break;   
    }

    
    case ERRORs:
    {
      next_state = STARTs;
      shiftOut(13, 9, MSBFIRST, DIG_DOT);
      delay(333);
      shiftOut(13, 9, MSBFIRST, 0x00);
      delay(333);
      shiftOut(13, 9, MSBFIRST, 0x00);
      delay(333);
      shiftOut(13, 9, MSBFIRST, 0x00);

      break;
    }
      
    default:
    {
      next_state = ERRORs;
      break;
    }
      
  }

}

void shiftAlarms(){
  int thirdDigit = displayCodes[alarmMinutes % 10];
  int secondDigit = displayCodes[alarmMinutes / 10];
  int firstDigit = displayCodes[alarmHours];

  thirdDigit = thirdDigit | DIG_DOT;

  digitalWrite(8, LOW);
  shiftOut(13, 9, MSBFIRST, thirdDigit);
  shiftOut(13, 9, MSBFIRST, secondDigit);
  shiftOut(13, 9, MSBFIRST, firstDigit);
  digitalWrite(8, HIGH);
}

void updateTime(){
  dotcounter = (dotcounter + 1) % 666;
  rtc.update();

  int thirdDigit  = displayCodes[rtc.minute() % 10];

  int secondDigit = displayCodes[rtc.minute() / 10];

  int firstDigit  = displayCodes[rtc.hour()];

  if(dotcounter < 333){
    firstDigit = firstDigit | DIG_DOT;
  }
  
  digitalWrite(8, LOW);
  shiftOut(13, 9, MSBFIRST, thirdDigit);
  shiftOut(13, 9, MSBFIRST, secondDigit);
  shiftOut(13, 9, MSBFIRST, firstDigit);
  digitalWrite(8, HIGH);
  
}

bool alarmTime(){
  if(true){
    if((rtc.hour() == alarmHours) && (rtc.minute() == alarmMinutes)){
      return true;
    }
  }
  return false;
}

void printDayHalf(){

  
  int firstDigit = 0x00;
  int secondDigit = DIG_A;
  int thirdDigit = 0x00;

  if(rtc.pm()){
    secondDigit = secondDigit & 0xFB;
  }
  
  digitalWrite(8, LOW);
  shiftOut(13, 9, MSBFIRST, thirdDigit);
  shiftOut(13, 9, MSBFIRST, secondDigit);
  shiftOut(13, 9, MSBFIRST, firstDigit);
  digitalWrite(8, HIGH);

}

bool setButtonPressed1(){
  if(digitalRead(5)){
    return true;
  }
  return false;
}

bool setButtonPressed2(){
  if(digitalRead(3)){
    return true;
  }
  return false;
}

bool setButtonPressed3(){
  if(digitalRead(4)){
    return true;
  }
  return false;
}


bool isClkPM(){
  rtc.set24Hour();
  if(rtc.hour() > 12){
    rtc.set12Hour();
    return true ;
  }
  rtc.set12Hour();
  return false;
}

void mmaInterrupt(){
  interrupted = true;
}


