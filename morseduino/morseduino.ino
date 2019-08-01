
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
#include <SimpleRotary.h>
#include "display.h"
#include "decoder.h"
#include "encoder.h"

#define SPEED_ENC_PIN_A 13
#define SPEED_ENC_PIN_B 12
#define SPEED_ENC_PIN_BUTTON 11
#define DOT_LEN_ADDR 0
#define TONE_HZ_ADDR 2
#define TONE_PIN 8      // output audio on pin 8
#define KEY_PIN 2       // Morse key pin
#define MODE_SELECT_PIN 4 // Pin for switching between Morse key input and serial input

#define MAX_DOT_LEN 150
#define MIN_DOT_LEN 20
#define MAX_TONE 2200
#define MIN_TONE 600

#define OP_MODE_ENC "en" // Encoder mode of operation
#define OP_MODE_DEC "de" // Decoder mode of operation

String opMode = "";

int toneHz = 1850;      // music tone/pitch in Hertz

// Pin A, Pin B, Button Pin
SimpleRotary rotary(SPEED_ENC_PIN_A, SPEED_ENC_PIN_B, SPEED_ENC_PIN_BUTTON);
Display display(128, 64);
Decoder decoder(KEY_PIN, TONE_PIN);
Encoder encoder(TONE_PIN);

byte dotLen;     // length of the morse code 'dot'

/**
   SETUP
*/
void setup() {
  pinMode(KEY_PIN, INPUT);
  pinMode(MODE_SELECT_PIN, INPUT);
  pinMode(SPEED_ENC_PIN_A, INPUT);
  pinMode(SPEED_ENC_PIN_B, INPUT);
  Serial.begin(9600);
  
  if (!display.init()) {
     Serial.println(F("SSD1306 allocation failed"));
     for (;;); // Don't proceed, loop forever
  }
  _init();
}

int morseKeyState; // the current reading from the Morse key

/**
   LOOP
*/
void loop() {
  checkRotary();
  if (digitalRead(MODE_SELECT_PIN)) {
    if (opMode.length() == 0 || opMode.equals(OP_MODE_DEC)) {
      opMode = OP_MODE_ENC;
      display.showHomeScreen(getWpm(dotLen), toneHz, opMode);
    }
    encoder.encode();
  } else {
    if (opMode.length() == 0 || opMode.equals(OP_MODE_ENC)) {
      opMode = OP_MODE_DEC;
      display.showHomeScreen(getWpm(dotLen), toneHz, opMode);
    }
    decoder.decode();
  }
}

byte encoderPinALast = LOW;

void _init() {
  dotLen = EEPROM.read(DOT_LEN_ADDR);
  EEPROM.get(TONE_HZ_ADDR, toneHz);
  setWpmDefaults();
  configureWpm();

  setToneDefaults();
  configureTone();
}

#define MODE_WPM 11
#define MODE_TONE 15

byte rotaryMode = MODE_WPM;

void checkRotary() {
  // 0 = not pushed, 1 = pushed, 2 = long pushed
  byte push = rotary.pushType(1000); // number of milliseconds button has to be pushed for it to be considered a long push.

  if ( push == 1 ) { // pushed
    if (rotaryMode == MODE_WPM) {
      rotaryMode = MODE_TONE;
      configureTone();
    } else if (rotaryMode == MODE_TONE) {
      rotaryMode = MODE_WPM;
      configureWpm();
    }
  } else if ( push == 2 ) { // long pushed
    display.showHomeScreen(getWpm(dotLen), toneHz, opMode);
  }

  // 0 = not turning, 1 = CW, 2 = CCW
  byte rotated = rotary.rotate();

  if ( rotated == 1 ) { // CW
    if (rotaryMode == MODE_WPM) {
      dotLen -= dotLen / 20;
      setWpmDefaults();
      configureWpm();
    } else if (rotaryMode == MODE_TONE) {
      toneHz += 50;
      setToneDefaults();
      configureTone();
    }
  } else if ( rotated == 2 ) { // CCW
    if (rotaryMode == MODE_WPM) {
      dotLen += dotLen / 20;
      setWpmDefaults();
      configureWpm();
    } else if (rotaryMode == MODE_TONE) {
      toneHz -= 50;
      setToneDefaults();
      configureTone();
    }
  }
}

byte getWpm(int _dotLen) {
  return round((float) 1000 / _dotLen);
}

void setWpmDefaults() {
  if (dotLen >= MAX_DOT_LEN) dotLen = MAX_DOT_LEN; else if (dotLen <= MIN_DOT_LEN) dotLen = MIN_DOT_LEN;
}

void configureWpm() {
  EEPROM.write(DOT_LEN_ADDR, dotLen);
  decoder.setDotLength(dotLen);
  encoder.setDotLength(dotLen);
  printWpm();
}

void printWpm() {
  display.showProgress("WPM", getWpm(dotLen), getWpm(MIN_DOT_LEN)); // Max WPM
}

void configureTone() {
  EEPROM.put(TONE_HZ_ADDR, toneHz);
  decoder.setTone(toneHz);
  encoder.setTone(toneHz);
  printTone();
}

void setToneDefaults() {
  if (toneHz >= MAX_TONE) toneHz = MAX_TONE; else if (toneHz <= MIN_TONE) toneHz = MIN_TONE;
}

void printTone() {
  display.showProgress("Tone(Hz)", toneHz, MAX_TONE);
}
