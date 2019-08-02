
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
#include "rotary.h"
#include "display.h"
#include "encoder.h"
#include "decoder.h"
#include "datastructure.h"

// #define ENABLE_SERIAL 1

// --------------- EEPROM addresses --------------------------
#define DOT_LEN_ADDR 0   // to store the Morse dot length used for WPM calculation
#define TONE_HZ_ADDR 2   // to store the tone frequency 

// --------------- pins --------------------------------------
#define SPEED_ENC_PIN_A 13
#define SPEED_ENC_PIN_B 12
#define SPEED_ENC_PIN_BUTTON 11
#define TONE_PIN 8      // output audio on pin 8
#define KEY_PIN 2       // Morse key pin
#define MODE_SELECT_PIN 4 // Pin for switching between Morse key input and serial input

// ---------------- limits ----------------------------------

#define MAX_DOT_LEN 150
#define MIN_DOT_LEN 20
#define MAX_TONE 2200
#define MIN_TONE 600
// ------------------------------------------------------------

SimpleRotary rotary(SPEED_ENC_PIN_A, SPEED_ENC_PIN_B, SPEED_ENC_PIN_BUTTON);
Display display(128, 64);
Decoder decoder(&display, KEY_PIN, TONE_PIN);
Encoder encoder(TONE_PIN);


int toneHz = 1850;      // music tone/pitch in Hertz
byte dotLen;     // length of the morse code 'dot'

OpMode currentOpMode = NULL;

bool showHome = true;
// ------------------------------------------------------------


/**
   SETUP
*/
void setup() {
  pinMode(KEY_PIN, INPUT);
  pinMode(MODE_SELECT_PIN, INPUT);
  pinMode(SPEED_ENC_PIN_A, INPUT);
  pinMode(SPEED_ENC_PIN_B, INPUT);
  #ifdef ENABLE_SERIAL
  Serial.begin(9600);
  #endif

  if (!display.init()) {
    #ifdef ENABLE_SERIAL
    Serial.println(F("SSD1306 allocation failed"));
    #endif
    for (;;); // Don't proceed, loop forever
  }
  dotLen = EEPROM.read(DOT_LEN_ADDR);
  EEPROM.get(TONE_HZ_ADDR, toneHz);

  rotary.setDebounceDelay(0);
  rotary.setErrorDelay(0);

  setWpmDefaults();
  configureWpm();

  setToneDefaults();
  configureTone();
}

/**
   LOOP
*/
void loop() {
  checkRotary();
  if (digitalRead(MODE_SELECT_PIN)) {
    if (currentOpMode == NULL || currentOpMode == dec) { // If it has toggled
      currentOpMode = enc;
      display.showHomeScreen(getWpm(dotLen), toneHz, currentOpMode);
    }
    encoder.encode();
  } else {
    if (currentOpMode == NULL || currentOpMode == enc) { // If it has toggled
      currentOpMode = dec;
      display.showHomeScreen(getWpm(dotLen), toneHz, currentOpMode);
    }
    decoder.decode();
  }
}

#define MODE_WPM 11
#define MODE_TONE 15

byte rotaryMode = MODE_WPM;

void checkRotary() {
  // 0 = not pushed, 1 = pushed, 2 = long pushed
  byte push = rotary.pushType(1000); // number of milliseconds button has to be pushed for it to be considered a long push.
  if ( push == 1  && !showHome) { // pushed while home screen is not shown
    if (rotaryMode == MODE_WPM) {
      rotaryMode = MODE_TONE;
      configureTone();
    } else if (rotaryMode == MODE_TONE) {
      rotaryMode = MODE_WPM;
      configureWpm();
    }
  } else if ( push == 2 ) { // long pushed
    if (!showHome) {
      display.showHomeScreen(getWpm(dotLen), toneHz, currentOpMode);
      showHome = true;
    } else {
      showHome = false;
      rotaryMode = MODE_WPM;
      configureWpm();
    }
  }

  // 0 = not turning, 1 = CW, 2 = CCW
  byte rotated = rotary.rotate();
  if (!showHome) {
    if (rotated == 1 ) { // CW
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
  display.showProgress("WPM", getWpm(dotLen), getWpm(MIN_DOT_LEN)); // Max WPM
}

void configureTone() {
  EEPROM.put(TONE_HZ_ADDR, toneHz);
  decoder.setTone(toneHz);
  encoder.setTone(toneHz);
  display.showProgress("Tone(Hz)", toneHz, MAX_TONE);
}

void setToneDefaults() {
  if (toneHz >= MAX_TONE) toneHz = MAX_TONE; else if (toneHz <= MIN_TONE) toneHz = MIN_TONE;
}
