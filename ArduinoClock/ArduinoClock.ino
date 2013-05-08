// Simply change commonCathode to 1 for common cathode 7-seg display
#define commonCathode 1
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <EEPROM.h>
#include <Bounce.h>
#include "RTClib.h"

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

// Shift register pins
const int latchPin = 7;
const int clockPin = 6;
const int dataPin = 13;

// Select digit pins
const int digit1 = 12;
const int digit2 = 11;
const int digit3 = 10;
const int digit4 = 9;

// alarm pin
const int alarmTonePin = 14;
const int alarmControlPin = 3;
const int leftButtonPin = 4;
const int rightButtonPin = 5;

// EEPROM locations
const int minuteLocation = 0;
const int hourLocation = 1;
const int alarmOnLocation = 2;

// Instantiate a Bounce object with a 20 millisecond debounce time
Bounce alarmControlButton = Bounce(alarmControlPin,20); 
Bounce leftButton = Bounce(leftButtonPin,20); 
Bounce rightButton = Bounce(rightButtonPin,20); 
/*
10 digits:
 Each defines which segments should be on/off for that digit: A,B,C,D,E,F,G,P
 */

const byte numbers[10] = 
{
  B11111100, // 0
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110  // 9
};

byte alarmMin;
byte alarmHour;
boolean alarmOn = false;
RTC_DS1307 RTC;
boolean setupAlarm = false;

int noteIndex = 0;
const int noteCount = 2;
//int notes[noteCount] = {
  //NOTE_F7,NOTE_FS7, NOTE_A7, NOTE_AS7};
 // int notes[noteCount] = { NOTE_A4, NOTE_B4,NOTE_C3};
//int notes[noteCount] = {
//  NOTE_A7, NOTE_AS7};
int notes[noteCount] = {400, 1000};

void setup() {
  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);

  pinMode(alarmControlPin,INPUT);
  pinMode(rightButtonPin,INPUT);
  pinMode(leftButtonPin,INPUT);

  resetDigits();
  setupRTC();

  alarmMin = EEPROM.read(minuteLocation);
  alarmHour = EEPROM.read(hourLocation);
  alarmOn = EEPROM.read(alarmOnLocation);
}

void setupRTC () {
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    //Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}

void resetDigits(){
  digitalWrite(digit1, HIGH); // common cathode
  digitalWrite(digit2, HIGH); // common cathode
  digitalWrite(digit3, HIGH); // common cathode
  digitalWrite(digit4, HIGH); // common cathode
}

void sevenSegWrite(byte digit, int digitPosition) {
  byte pin = 2;
  int number =  numbers[digit];
  resetDigits();
  digitalWrite(digitPosition, LOW);
  updateShiftRegister(number);
}

boolean leftButtonPressed = false;
boolean rightButtonPressed = false;
boolean alarmControlButtonPressed = false;

boolean displayAlarm = true;
long alarmSwapTime = -1;

boolean playingAlarm = false;

void loop(){
  checkAlarmButtons();

  if(!setupAlarm){
    DateTime now = RTC.now();
    int hour = now.hour();
    int minute = now.minute();
    if(playAlarm(minute, hour)){ 
      playingAlarm = true;
      if(playTone(notes[noteIndex], 10)){
        noteIndex++;
        if(noteIndex >= noteCount){
          noteIndex=0; 
        }
      } 
      else {
        noTone(alarmTonePin);
      }
    } else {
       playingAlarm = false; 
    }
    displayTime(minute, hour); 
  } 
  else {
   if(displayAlarm){
      showAlarm(); 
    } 
    else {
      hideAlarm();
    }
    alarmCycle();
  }
}

void alarmCycle(){
  if(millis() - alarmSwapTime >= 500){
     alarmSwapTime = millis();
     displayAlarm = !displayAlarm;
     Serial.print("Alarm cycle:");
     Serial.println(alarmSwapTime);
  }
}

void showAlarm(){
  int alarmMin = EEPROM.read(0);
  int alarmHour = EEPROM.read(1);
  displayTime(alarmMin, alarmHour);
}

void hideAlarm(){
  resetDigits();
}

void checkAlarmButtons(){
  leftButton.update();
  int leftValue = leftButton.read();

  rightButton.update();
  int rightValue = rightButton.read();

  alarmControlButton.update();
  int alarmControlValue = alarmControlButton.read();

  if(alarmControlValue == HIGH){
    if(!alarmControlButtonPressed){
      alarmControlButtonPressed = true;
      if(playingAlarm){
         EEPROM.write(alarmOnLocation, 0);
        return; 
      }
      if(!setupAlarm){
        setupAlarm = true;
        Serial.println("setupAlarm = true");
      } 
      else {
        setupAlarm = false; 
        Serial.println("setupAlarm = false");
      }
    }
  } 
  else {
    alarmControlButtonPressed = false;
  }

  if ( leftValue == HIGH) {
    if(!leftButtonPressed){
      Serial.println("Left button pressed");
      leftButtonPressed = true;
      if(setupAlarm){
        alarmHour++;
        if(alarmHour >= 24){
          alarmHour = 0;
        }
        EEPROM.write(hourLocation, alarmHour);
      }
    } 
  }
  else {
    leftButtonPressed = false;
  }

  if ( rightValue == HIGH) {
    if(!rightButtonPressed){
      Serial.println("Right button pressed");
      rightButtonPressed = true;
      if(setupAlarm){
        alarmMin++;
        if(alarmMin >= 60){
          alarmMin = 0; 
        }
        EEPROM.write(minuteLocation, alarmMin);
      }
    }
  }
  else{
    rightButtonPressed = false;
  }
}

void displayTime(int minute, int hour){
  int minLSB = minute%10; 
  sevenSegWrite(minLSB, digit1); 
  updateShiftRegister(0);

  int minMSB = minute/10;
  sevenSegWrite(minMSB, digit2); 
  updateShiftRegister(0);

  int hourLSB = hour%10;
  sevenSegWrite(hourLSB, digit3); 
  updateShiftRegister(0);

  int hourMSB = hour/10;
  sevenSegWrite(hourMSB, digit4); 
  updateShiftRegister(0);
}

void updateShiftRegister(byte value)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  digitalWrite(latchPin, HIGH);
}

long noteStartTime = -1;

boolean playAlarm(int minute,int  hour){
  byte _minute = EEPROM.read(minuteLocation);
  byte _hour = EEPROM.read(hourLocation);
  return EEPROM.read(alarmOnLocation) == 1 && _minute == minute && _hour == hour;
}

boolean playTone(int note, int noteDuration){
  // to calculate the note duration, take one second 
  // divided by the note type.
  //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

  int duration = 1500/noteDuration;
  tone(alarmTonePin, note,duration);

  // to distinguish the notes, set a minimum time between them.
  // the note's duration + 30% seems to work well:
  if(noteStartTime == -1){
    noteStartTime = millis();
    return false;
  } 
  else if(noteStartTime !=-1 && millis() - noteStartTime >= duration * 1.8){
    noTone(alarmTonePin); 
    noteStartTime = -1;
  } 
  else {
    return false;
  }
  return true;
}



















