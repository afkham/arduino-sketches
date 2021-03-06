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
#include <Arduino.h>
#include <EEPROM.h>
#include "rotary.h"
#include "display.h"
#include "encoder.h"
#include "decoder.h"
#include "datastructure.h"

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
#define MIN_TONE 300
// ------------------------------------------------------------

Morseduino::SimpleRotary rotary(SPEED_ENC_PIN_A, SPEED_ENC_PIN_B, SPEED_ENC_PIN_BUTTON);
Morseduino::Display display(128, 64);
Morseduino::Decoder decoder(&display, KEY_PIN, TONE_PIN);
Morseduino::Encoder encoder(TONE_PIN);

uint16_t toneHz = 1850;      // music tone/pitch in Hertz
uint8_t dotLen;     // length of the morse code 'dot'

OpMode currentOpMode = invalid;

bool showHome = true;
// ------------------------------------------------------------

void setWpmDefaults();

void configureWpm();

void setToneDefaults();

void configureTone();

uint8_t getWpm(uint8_t dotLen);

void checkRotary();

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
    if (digitalRead(MODE_SELECT_PIN)) {
        if (currentOpMode == invalid || currentOpMode == dec) { // If it has toggled
            currentOpMode = enc;
            display.showHomeScreen(getWpm(dotLen), toneHz, currentOpMode);
            showHome = true;
        }
        encoder.encode();
    } else {
        if (currentOpMode == invalid || currentOpMode == enc) { // If it has toggled
            currentOpMode = dec;
            display.showHomeScreen(getWpm(dotLen), toneHz, currentOpMode);
            showHome = true;
        }
        decoder.decode();
    }
    checkRotary();
}

#define MODE_WPM 11
#define MODE_TONE 15

uint8_t rotaryMode = MODE_WPM;

void checkRotary() {
    // 0 = not pushed, 1 = pushed, 2 = long pushed
    uint8_t push = rotary.pushType(700);
    if (push == 1 && !showHome) { // pushed while home screen is not shown
        if (rotaryMode == MODE_WPM) {
            rotaryMode = MODE_TONE;
            configureTone();
        } else if (rotaryMode == MODE_TONE) {
            display.clear();
            rotaryMode = MODE_WPM;
            configureWpm();
        }
    } else if (push == 2) { // long pushed
        if (!showHome) {
            display.showHomeScreen(getWpm(dotLen), toneHz, currentOpMode);
            showHome = true;
            return; // No need to check rotation once you show home screen
        } else {
            showHome = false;
            rotaryMode = MODE_WPM;
            configureWpm();
        }
    }

    if (showHome) return; // Rotation is not valid when the home screen is shown

    // 0 = not turning, 1 = CW, 2 = CCW
    uint8_t rotated = rotary.rotate();
    if (rotated == 1) { // CW
        if (rotaryMode == MODE_WPM) {
            dotLen -= dotLen / 20;
            setWpmDefaults();
            configureWpm();
        } else if (rotaryMode == MODE_TONE) {
            toneHz += 50;
            setToneDefaults();
            configureTone();
        }
    } else if (rotated == 2) { // CCW
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

uint8_t getWpm(uint8_t _dotLen) {
    return round((float) 1000 / _dotLen);
}

void setWpmDefaults() {
    if (dotLen >= MAX_DOT_LEN) dotLen = MAX_DOT_LEN; else if (dotLen <= MIN_DOT_LEN) dotLen = MIN_DOT_LEN;
}

const char *WPM_TXT = "WPM";

void configureWpm() {
    EEPROM.write(DOT_LEN_ADDR, dotLen);
    decoder.setDotLength(dotLen);
    encoder.setDotLength(dotLen);
    display.showProgress(WPM_TXT, getWpm(dotLen), getWpm(MIN_DOT_LEN)); // Max WPM
}

const char *TONE_TXT = "Tone(Hz)";

void configureTone() {
    EEPROM.put(TONE_HZ_ADDR, toneHz);
    decoder.setTone(toneHz);
    encoder.setTone(toneHz);
    display.showProgress(TONE_TXT, toneHz, MAX_TONE);
}

void setToneDefaults() {
    if (toneHz >= MAX_TONE) toneHz = MAX_TONE; else if (toneHz <= MIN_TONE) toneHz = MIN_TONE;
}
