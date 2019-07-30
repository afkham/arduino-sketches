
/*
   Copyright (c) 2019, Afkham Azeez (http://me.afkham.org) All Rights Reserved.

   WSO2 Inc. licenses this file to you under the Apache License,
   Version 2.0 (the "License"); you may not use this file except
   in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing,
  software distributed under the License is distributed on an
  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  KIND, either express or implied.  See the License for the
  specific language governing permissions and limitations
  under the License.
*/
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>  // for type definitions

/* 
 *  The following template is used for reading from PROGMEM
 *  See: https://arduino.stackexchange.com/questions/13545/using-progmem-to-store-array-of-structs
 */
template <typename T> void PROGMEM_readAnything (const T * sce, T& dest) {
  memcpy_P (&dest, sce, sizeof (T));
}

/* 
 * Number of items in an array
 */
template< typename T, size_t N > size_t ArraySize (T (&) [N]){ return N; }

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int SPEED_ENC_PIN_A = 13;
const int SPEED_ENC_PIN_B = 12;
const int DOT_LEN_ADDR = 0; 
const int ENCODER_PIN_LAST_ADDR = 1;
const int TONE_PIN = 8;      // output audio on pin 8
const int KEY_PIN = 2;       // Morse key pin
const int MODE_SELECT_PIN = 4; // Pin for switching between Morse key input and serial input
const int SPEED_PIN = A0;    // select the input pin for the potentiometer
const int TONE_HZ = 1800;      // music TONE_HZ/pitch in Hertz

// Character to Morse code mapping
typedef struct {
  char ch[6];
  char morseSeq[10];
} MorseMapping;

// Morse Alphabet
const MorseMapping morseMappings[47] PROGMEM = {
  {"a", "._"}, {"b", "_..."}, {"c", "_._."}, {"d", "_.."}, {"e", "."}, {"f", ".._."}, {"g", "__."},
  {"h", "...."}, {"i", ".."}, {"j", ".___"}, {"k", "_._"}, {"l", "._.."}, {"m", "__"}, {"n", "_."},
  {"o", "___"}, {"p", ".__."}, {"q", "__._"}, {"r", "._."}, {"s", "..."}, {"t", "_"}, {"u", "_."},
  {"v", "..._"}, {"w", ".__"}, {"x", "_.._"}, {"y", "_.__"}, {"z", "__.."},
  {"1", ".____"}, {"2", "..___"}, {"3", "...__"}, {"4", "...._"}, {"5", "....."}, {"6", "_...."},
  {"7", "__..."}, {"8", "___.."}, {"9", "____."}, {"0", "_____"},
  {".", "._._._"}, {",", "__..__"}, {"?", "..__.."}, {"=", "_..._"}, {"+", "._._."}, {"-", "_...._"},
  {"<SOS>", "...___..."}, {"<KA>", "_._._"}, {"<AS>", "._..."}, {"<AR>", "._._."}, {"<SK>", "..._._"},
};

void setup() {
  pinMode(KEY_PIN, INPUT);
  pinMode(MODE_SELECT_PIN, INPUT);
  pinMode(SPEED_ENC_PIN_A, INPUT);
  pinMode(SPEED_ENC_PIN_B, INPUT);
  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  _init();
}

int morseKeyState; // the current reading from the Morse key

void loop() {
  setSpeed();
  digitalRead(MODE_SELECT_PIN) ? playCodeFromSerial() : playOscillator();
}

void showText(String text) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println(text);
  display.display();      // Show initial text
//  delay(100);
//
//  // Scroll in various directions, pausing in-between:
//  display.startscrollright(0x00, 0x0F);
//  delay(2000);
//  display.stopscroll();
//  delay(1000);
//  display.startscrollleft(0x00, 0x0F);
//  delay(2000);
//  display.stopscroll();
//  delay(1000);
//  display.startscrolldiagright(0x00, 0x07);
//  delay(2000);
//  display.startscrolldiagleft(0x00, 0x07);
//  delay(2000);
//  display.stopscroll();
//  delay(1000);
}

/*
  Set the speed of your morse code
  Adjust the dotLen length to speed up or slow down your morse code
    (all of the other lengths are based on the dotLen)

  There are rules to help people distinguish dots from dashes in Morse code.

  1. The length of a dot is 1 time unit.
  2. A dash is 3 time units.
  3. The space between symbols (dots and dashes) of the same letter is 1 time unit.
  4. The space between letters is 3 time units.
  5. The space between words is 7 time units.
*/
int dotLen;     // length of the morse code 'dot'
int dashLen;    // length of the morse code 'dash'
int symbolSpacing; // length of the pause between elements of a character
int charSpacing; // length of the spaces between characters
int wordSpacing; // length of the pause between words

int encoderPinALast = LOW;

void _init() {
  dotLen = EEPROM.read(DOT_LEN_ADDR);
  encoderPinALast = EEPROM.read(ENCODER_PIN_LAST_ADDR);
  setSpeedDefaults();
  configureWordSpeed();
}

void setSpeed() {
  int n = digitalRead(SPEED_ENC_PIN_A);
  if (encoderPinALast == LOW && n == HIGH) {
    if (digitalRead(SPEED_ENC_PIN_B) == LOW) {
      dotLen -= dotLen/20;
    } else {
      dotLen += dotLen/20;
    }
    setSpeedDefaults();
    configureWordSpeed();
  }
  encoderPinALast = n;
  EEPROM.write(ENCODER_PIN_LAST_ADDR, encoderPinALast);
}

void setSpeedDefaults() {
  if (dotLen >= 150) dotLen = 150; else if (dotLen <= 20) dotLen = 20;
}

void configureWordSpeed() {
  dashLen = dotLen * 3;
  symbolSpacing = dotLen;
  charSpacing = dotLen * 3;
  wordSpacing = dotLen * 7;
  EEPROM.write(DOT_LEN_ADDR, dotLen);
  printSpeed();
}

void printSpeed() {
  int wpm = round((float)1000 / dotLen);
  String wpmText = "Speed: ";
  wpmText.concat(wpm);
  Serial.println(wpmText); 
  showText(wpmText);
}

// ------------ functions for oscillator mode -----------

// The time at which a dot or dash started. This is used for identifying dots and dashes
int symbolStartedAt = -1;

// The time at which the last dot or dash was received. This is used to identify character boundaries.
int lastSymbolReceivedAt = -1;

// The time at which the last character was received. This is used for identifying word boundaries.
int lastCharReceivedAt = -1;

// Holds the currently active symbol (dots and dashes) buffer.
// This will be used later for identifying the character from the dots and dashes.
const int MAX_SYMBOLS = 10;
char currentSymbolBuff[MAX_SYMBOLS]; // Maximum possible symbols in Morse code is 10
int currentSymbolIndex = 0; // index to the currentSymbolBuff
bool garbageReceived = false; // Indicates whether the received symbol sequence is invalid

void playOscillator() {
  morseKeyState = digitalRead(KEY_PIN);
  if (morseKeyState == HIGH) {
    int now = millis();
    if (symbolStartedAt == -1) {
      symbolStartedAt = now;
    }
    tone(TONE_PIN, TONE_HZ);
  } else {
    noTone(TONE_PIN);
    if (symbolStartedAt != -1) {
      if (currentSymbolIndex > MAX_SYMBOLS) {
        garbageReceived = true;
      } else if (!garbageReceived) {
        int now = millis();
        if (now - symbolStartedAt >= dashLen) {
          currentSymbolBuff[currentSymbolIndex++] = '_';
        } else {
          currentSymbolBuff[currentSymbolIndex++] = '.';
        }
        symbolStartedAt = -1;
        lastSymbolReceivedAt = millis();
        lastCharReceivedAt = -1;
      }
    }
    int now = millis();
    if (lastCharReceivedAt != -1 && now - lastCharReceivedAt > wordSpacing) { // Have we completed a word?
      Serial.print(F(" "));
      lastCharReceivedAt = -1;
    } else if (lastSymbolReceivedAt != -1 && now - lastSymbolReceivedAt > charSpacing) { // Have we completed a character?
      printChar(currentSymbolBuff);
      garbageReceived = false;
      resetCurrentSymbolBuff();
      lastSymbolReceivedAt = -1;
      lastCharReceivedAt = millis();
    }
  }
}

void resetCurrentSymbolBuff() {
  for (int i = 0; i < MAX_SYMBOLS; i++) {
    currentSymbolBuff[i] = '\0';
  }
  currentSymbolIndex = 0;
}

void pause(int delayTime) {
  noTone(TONE_PIN);
  delay(delayTime);
}

int lengthof(char const str[]) {
  int i = 0;
  while (str[i] != '\0') {
    i++;
  }
  return i;
}

bool isEqual(char ch1[], char const ch2[]) {
  if (lengthof(ch1) != lengthof(ch2)) return false;
  int i = 0;
  while (ch1[i] != '\0' && ch2[i] != '\0') {
    if (ch1[i] != ch2[i]) return false;
    i++;
  }
  return true;
}

void printChar(char morseStr[]) {
  for (int i = 0; i < ArraySize(morseMappings); i++) {
    MorseMapping mm;
    PROGMEM_readAnything (&morseMappings[i], mm);
    if (isEqual(morseStr, mm.morseSeq)) {
      Serial.print(mm.ch);
      if (isEqual(mm.ch, "=")) {
        Serial.println();
      }
      return;
    }
  }
  Serial.print('#');
}

// ------------ functions for playing Morse mode -----------

void playCodeFromSerial() {
  if (Serial.available() > 0) {
    String strToPlay = Serial.readString();
    int i = 0;
    int len = strToPlay.length();
    while (i < len) {
      if (strToPlay[i] == '<') { // Handle joint characters
        String tmp = strToPlay.substring(i);
        int j = tmp.substring(1).indexOf('>');
        if (j == -1) {
          Serial.println(F("####### INVALID INPUT #######"));
          return;
        }
        tmp = tmp.substring(0, j + 2);
        playMorse(tmp);
        Serial.print(tmp);
        i = i + j + 2;
      } else {
        if (strToPlay[i] == ' ') {
          delay(wordSpacing);
        } else {
          playMorse(toLowerCase(strToPlay[i]));
          delay(charSpacing);
        }
        Serial.print(strToPlay[i]);
        if (strToPlay[i] == '=') {
          Serial.println();
        }
        i++;
      }
    }
    Serial.println();
  }
}

void playMorse(char normalChar[]) {
  for (int i = 0; i < ArraySize(morseMappings); i++) {
    MorseMapping mm;
    PROGMEM_readAnything (&morseMappings[i], mm);
    if (normalChar == mm.ch[0]) {
      playMorseSequence(mm.morseSeq);
      return;
    }
  }
}

void playMorse(String normalChar) {
  for (int i = 0; i < ArraySize(morseMappings); i++) {
    MorseMapping mm;
    PROGMEM_readAnything (&morseMappings[i], mm);
    String str = mm.ch;
    if (normalChar == str) {
      playMorseSequence(mm.morseSeq);
      return;
    }
  }
}

void playMorseSequence(char morseSequence[]) {
  for (int i = 0; i < lengthof(morseSequence); i++) {
    if (morseSequence[i] == '.') {
      di();
    } else if (morseSequence[i] == '_') {
      dah();
    }
  }
}

void di() {
  tone(TONE_PIN, TONE_HZ, dotLen);
  delay(dotLen);
  pause(symbolSpacing);
}

void dah() {
  tone(TONE_PIN, TONE_HZ, dashLen);  // start playing a tone
  delay(dashLen);               // hold in this position
  pause(symbolSpacing);
}
