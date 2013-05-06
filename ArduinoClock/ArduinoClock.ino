// Simply change commonCathode to 1 for common cathode 7-seg display
#define commonCathode 1
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include <EEPROM.h>
#include "RTClib.h"

#define NOTE_A7  3520
#define NOTE_AS7 3729

// Shift register pins
int latchPin = 7;
int clockPin = 6;
int dataPin = 13;

// Select digit pins
int digit1 = 12;
int digit2 = 11;
int digit3 = 10;
int digit4 = 9;

// alarm pin
int alarmPin = 14;

/*
10 digits:
 Each defines which segments should be on/off for that digit: A,B,C,D,E,F,G,P
 */

byte numbers[10] = 
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


/*byte numbers[10] = 
 {
 B11000000, // 0
 B11111001, // 1
 B10100100, // 2
 B10110000, // 3
 B10011001, // 4
 B10010010, // 5
 B10000010, // 6
 B11111000, // 7
 B10000000, // 8
 B10010000  // 9
 };*/

/*
byte numbers[10] = 
 {
 B00111111, // 0
 B00000110, // 1
 B01011011, // 2
 B01001111, // 3
 B01100110, // 4
 B01101101, // 5
 B01111101, // 6
 B00000111, // 7
 B01111111, // 8
 B01101111  // 9
 };*/

void setup() {

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  pinMode(digit4, OUTPUT);

  resetDigits();
  setupRTC();
}

RTC_DS1307 RTC;

int noteIndex = 0;
const int noteCount = 2;
//int notes[noteCount] = {NOTE_F7,NOTE_FS7, NOTE_A7, NOTE_AS7};
int notes[noteCount] = {NOTE_A7, NOTE_AS7};

void setupRTC () {
  Serial.begin(9600);

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

void writeDot(byte dot) {
  // digitalWrite(segmentDP, dot);
}

void sevenSegWrite(byte digit, int digitPosition) {
  byte pin = 2;
  int number =  numbers[digit];
  resetDigits();
  digitalWrite(digitPosition, LOW);
  updateShiftRegister(number);
  /*for (int bitIndex = 0; bitIndex < 7; bitIndex++){
   if(commonCathode){
   digitalWrite(pin, 1-bitRead(number, bitIndex));
   } 
   else {
   digitalWrite(pin, bitRead(number, bitIndex));
   }
   ++pin;
   }*/
}

void loop(){

  DateTime now = RTC.now();
  int hour = now.hour();
  int minute = now.minute();

  Serial.println(hour);

  sevenSegWrite(minute%10, digit1); 
  updateShiftRegister(0);


  sevenSegWrite(minute/10, digit2); 
  updateShiftRegister(0);


  sevenSegWrite(hour%10, digit3); 
  updateShiftRegister(0);


  sevenSegWrite(hour/10, digit4); 
  updateShiftRegister(0);

  if(soundAlarm){ 
  if(playTone(notes[noteIndex], 10)){
    noteIndex++;
    if(noteIndex >= noteCount){
      noteIndex=0; 
    }
  } else {
    
  }
  }
}

void updateShiftRegister(byte value)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  digitalWrite(latchPin, HIGH);
}

long noteStartTime = -1;

boolean soundAlarm(){
  return false;
}

boolean playTone(int note, int noteDuration){
  // to calculate the note duration, take one second 
  // divided by the note type.
  //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

  int duration = 1000/noteDuration;
  tone(alarmPin, note,duration);

  // to distinguish the notes, set a minimum time between them.
  // the note's duration + 30% seems to work well:
  if(noteStartTime == -1){
    noteStartTime = millis();
    return false;
  } 
  else if(noteStartTime !=-1 && millis() - noteStartTime >= duration * 1.3){
    noTone(alarmPin); 
    noteStartTime = -1;
  } 
  else {
    Serial.println(false);
    return false;
  }
  return true;
}



