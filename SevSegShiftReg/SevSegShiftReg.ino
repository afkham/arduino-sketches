// Simply change commonCathode to 1 for common cathode 7-seg display
#define commonCathode 1
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

// Shift register pins
int latchPin = 4;
int clockPin = 3;
int dataPin = 2;

// Select digit pins
int digit1 = 5;
int digit2 = 6;
int digit3 = 7;
int digit4 = 8;

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
void setupRTC () {
  Serial.begin(57600);
  pinMode(A3, OUTPUT); 
  pinMode(A2, OUTPUT); 
  
  digitalWrite(A3, HIGH);
  digitalWrite(A2, LOW);
  
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
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
  
  sevenSegWrite(minute%10, digit1); 
  updateShiftRegister(0);
  
  
  sevenSegWrite(minute/10, digit2); 
  updateShiftRegister(0);
  
  
  sevenSegWrite(hour%10, digit3); 
  updateShiftRegister(0);
  

  sevenSegWrite(hour/10, digit4); 
  updateShiftRegister(0);
}

void updateShiftRegister(byte value)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  digitalWrite(latchPin, HIGH);
}



