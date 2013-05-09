// Simply change commonCathode to 1 for common cathode 7-seg display
#define commonCathode 1
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <EEPROM.h>
#include <Bounce.h>
#include "RTClib.h"
#include "pitches.h"
#include <dht11.h>

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

// DHT11 sensor pins
const int dht11Pin = 15;

// EEPROM locations
const int minuteLocation = 0;
const int hourLocation = 1;
const int alarmOnLocation = 2;

// Instantiate a Bounce object with a 20 millisecond debounce time
Bounce alarmControlButton = Bounce(alarmControlPin,20); 
Bounce leftButton = Bounce(leftButtonPin,20); 
Bounce rightButton = Bounce(rightButtonPin,20); 

dht11 DHT11;
int temperature;

int minute;
int hour;

/*
10 digits:
 Each defines which segments should be on/off for that digit: A,B,C,D,E,F,G,P
 */

const byte numbers[11] = 
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
  B11110110,  // 9
  B10011100  // C
};

byte alarmMin;
byte alarmHour;
boolean alarmOn = false;
RTC_DS1307 RTC;
boolean setupAlarm = false;

int noteIndex = 0;
const int noteCount = 8;

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };

boolean leftButtonPressed = false;
boolean rightButtonPressed = false;
boolean alarmControlButtonPressed = false;

boolean displayAlarm = true;
boolean showTime = true;
long alarmSwapTime = -1;
long timeSwapTime = -1;

boolean playingAlarm = false;

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
  DateTime now = RTC.now();
  hour = now.hour();
  minute = now.minute();
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

void loop(){
  checkAlarmButtons();

  if(!setupAlarm){
    if(playAlarm(minute, hour)){ 
      playingAlarm = true;
      if(playTone(melody[noteIndex],noteDurations[noteIndex])){
        noteIndex++;
        if(noteIndex >= noteCount){
          noteIndex=0; 
        }
      }
    } 
    else {
      playingAlarm = false; 
    }
    if(showTime){
      displayTime(minute, hour); 
    } 
    else {
      displayTemperature();
    }
    displayCycle();
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

void displayCycle(){
  boolean swap;
  if((!showTime && millis() - timeSwapTime >= 2000) || (showTime && millis() - timeSwapTime >= 5000)){
    timeSwapTime = millis();
    showTime = !showTime;
    if(!showTime){
         int chk = DHT11.read(dht11Pin);
         temperature = (int)DHT11.temperature;
         Serial.println(temperature); 
    } else {
        DateTime now = RTC.now();
        hour = now.hour();
        minute = now.minute();
    }
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
        EEPROM.write(alarmOnLocation, 1);
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

  int duration = 1000/noteDuration;
  tone(alarmTonePin, note,duration);

  // to distinguish the notes, set a minimum time between them.
  // the note's duration + 30% seems to work well:
  if(noteStartTime == -1){
    noteStartTime = millis();
    return false;
  } 
  else if(noteStartTime !=-1 && millis() - noteStartTime >= duration * 1.3){
    noTone(alarmTonePin); 
    noteStartTime = -1;
  } 
  else {
    return false;
  }
  return true;

}

void displayTemperature(){
  sevenSegWrite(10, digit1); 
  updateShiftRegister(0);

  sevenSegWrite(temperature%10, digit2); 
  updateShiftRegister(0);
  
  sevenSegWrite(temperature/10, digit3); 
  updateShiftRegister(0);

  digitalWrite(digit4, HIGH); // common cathode
}






















