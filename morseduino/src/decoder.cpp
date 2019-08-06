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

#include "decoder.h"
#include "datastructure.h"

#include <Arduino.h>

Morseduino::Decoder::Decoder(Display *display, uint8_t keyPin, uint8_t tonePin) : _display(display), _keyPin(keyPin),
                                                                                  _tonePin(tonePin) {}

Morseduino::Decoder::~Decoder() { delete _display; }

void Morseduino::Decoder::setDotLength(uint8_t dotLen) {
    WPM_RULES
}

void Morseduino::Decoder::setTone(uint16_t toneHz) {
    _toneHz = toneHz;
}

void Morseduino::Decoder::decode() {
    uint64_t currentTime = millis();
    if (currentTime - _debounceTime >= _debounceDelay) {
        uint8_t morseKeyState = digitalRead(_keyPin);
        if (morseKeyState == HIGH) {
            uint64_t now = millis();
            if (_symbolStartedAt == 0) {
                _symbolStartedAt = now;
            }
#ifndef TONE_OFF
            tone(_tonePin, _toneHz);
#endif
            return;
        } else {
#ifndef TONE_OFF
            noTone(_tonePin);
#endif
            if (_symbolStartedAt != 0) {
                if (_currentSymbolIndex >= MAX_SYMBOLS) {
                    _currentSymbolIndex = 0;
                    _symbolStartedAt = 0;
                    _lastSymbolReceivedAt = millis();
                    _lastCharReceivedAt = 0;
                    return;
                } else {
                    uint32_t now = millis();
                    if (now - _symbolStartedAt >= _dashLen) {
                        _currentSymbolBuff[_currentSymbolIndex++] = '_';
                    } else {
                        _currentSymbolBuff[_currentSymbolIndex++] = '.';
                    }
                    _symbolStartedAt = 0;
                    _lastSymbolReceivedAt = millis();
                    _lastCharReceivedAt = 0;
                    return;
                }
            } else {
                uint32_t now = millis();
                if (_lastCharReceivedAt != 0 &&
                    now - _lastCharReceivedAt >= _wordSpacing) { // Have we completed a word?
#ifdef ENABLE_SERIAL
                    Serial.print(F(" "));
#endif
                    _display->showText(" ");
                    _lastCharReceivedAt = 0;
                    return;
                } else if (_lastSymbolReceivedAt != 0 &&
                           now - _lastSymbolReceivedAt >= _charSpacing) { // Have we completed a character?
                    _printChar(_currentSymbolBuff);
                    _resetCurrentSymbolBuff();
                    _lastSymbolReceivedAt = 0;
                    _lastCharReceivedAt = millis();
                    return;
                }
            }
        }
        _debounceTime = currentTime;
    }
}

void Morseduino::Decoder::_resetCurrentSymbolBuff() {
    for (char & i : _currentSymbolBuff) {
        i = '\0';
    }
    _currentSymbolIndex = 0;
}

void Morseduino::Decoder::_printChar(char morseStr[]) {
    for (uint8_t i = 0; i < ArraySize(morseMappings); i++) {
        MorseMapping mm;
        PROGMEM_readAnything(&morseMappings[i], mm);
        if (strcmp(morseStr, mm.morseSeq) == 0) {
#ifdef ENABLE_SERIAL
            Serial.print(mm.ch);
#endif
            _display->showText(mm.ch);
#ifdef ENABLE_SERIAL
            if (strcmp(mm.ch, "=") == 0) {
                Serial.println();
            }
#endif
            return;
        }
    }
#ifdef ENABLE_SERIAL
    Serial.print('#');
#endif
    _display->showText("#");
}
