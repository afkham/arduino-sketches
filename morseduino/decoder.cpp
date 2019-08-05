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

Decoder::Decoder(Display* display, byte keyPin, byte tonePin): _display(display), _keyPin(keyPin), _tonePin(tonePin) {}

Decoder::~Decoder() {delete _display;}

void Decoder::setDotLength(byte dotLen) {
  WPM_RULES
}

void Decoder::setTone(int toneHz) {
  _toneHz = toneHz;
}

void Decoder::decode() {
  int currentTime = millis();
  if (currentTime - _debounceTime >= _debounceDelay) {
    byte morseKeyState = digitalRead(_keyPin);
    if (morseKeyState == HIGH) {
      int now = millis();
      if (_symbolStartedAt == -1) {
        _symbolStartedAt = now;
      }
      tone(_tonePin, _toneHz);
      return;
    } else {
      noTone(_tonePin);
      if (_symbolStartedAt != -1) {
          if (_currentSymbolIndex > MAX_SYMBOLS) {
              _garbageReceived = true;
              return;
          } else if (!_garbageReceived) {
              int now = millis();
              if (now - _symbolStartedAt >= _dashLen) {
                  _currentSymbolBuff[_currentSymbolIndex++] = '_';
              } else {
                  _currentSymbolBuff[_currentSymbolIndex++] = '.';
              }
              _symbolStartedAt = -1;
              _lastSymbolReceivedAt = millis();
              _lastCharReceivedAt = -1;
              return;
          }
      } else {
        int now = millis();
        if (_lastCharReceivedAt != -1 && now - _lastCharReceivedAt >= _wordSpacing) { // Have we completed a word?
          #ifdef ENABLE_SERIAL
          Serial.print(F(" "));
          #endif
          _display->showText(" ");
          _lastCharReceivedAt = -1;
          return;
        } else if (_lastSymbolReceivedAt != -1 && now - _lastSymbolReceivedAt >= _charSpacing) { // Have we completed a character?
          _printChar(_currentSymbolBuff);
          _garbageReceived = false;
          _resetCurrentSymbolBuff();
          _lastSymbolReceivedAt = -1;
          _lastCharReceivedAt = millis();
          return;
        }
      }
    }
    _debounceTime = currentTime;
  }
}

void Decoder::_resetCurrentSymbolBuff() {
  for (int i = 0; i < MAX_SYMBOLS; i++) {
    _currentSymbolBuff[i] = '\0';
  }
  _currentSymbolIndex = 0;
}

void Decoder::_pause(int delayTime) {
  noTone(_tonePin);
  delay(delayTime);
}

//TODO: Duplicate fn in decoder and encoder
byte Decoder::_lengthof(char const str[]) {
  int i = 0;
  while (str[i] != '\0') {
    i++;
  }
  return i;
}

bool Decoder::_isEqual(char ch1[], char const ch2[]) {
  if (_lengthof(ch1) != _lengthof(ch2)) return false;
  int i = 0;
  while (ch1[i] != '\0' && ch2[i] != '\0') {
    if (ch1[i] != ch2[i]) return false;
    i++;
  }
  return true;
}

void Decoder::_printChar(char morseStr[]) {
  for (int i = 0; i < ArraySize(morseMappings); i++) {
    MorseMapping mm;
    PROGMEM_readAnything (&morseMappings[i], mm);
    if (_isEqual(morseStr, mm.morseSeq)) {
      #ifdef ENABLE_SERIAL
      Serial.print(mm.ch);
      #endif
      _display->showText(mm.ch);
      #ifdef ENABLE_SERIAL
      if (_isEqual(mm.ch, "=")) {
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
