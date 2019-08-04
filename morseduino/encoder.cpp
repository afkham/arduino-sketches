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

#include "encoder.h"
#include "datastructure.h"

#include <Arduino.h>

Encoder::Encoder(byte tonePin): _tonePin(tonePin) {}

void Encoder::setDotLength(byte dotLen) {
  WPM_RULES
}

void Encoder::setTone(int toneHz) {
  _toneHz = toneHz;
}

void Encoder::encode() {
  if (Serial.available() > 0) {
    String strToEncode = Serial.readString();
    int i = 0;
    int len = strToEncode.length();
    while (i < len) {
      if (strToEncode[i] == '<') { // Handle joint characters
        String tmp = strToEncode.substring(i);
        int j = tmp.substring(1).indexOf('>');
        if (j == -1) {
          Serial.println(F("####### INVALID INPUT #######"));
          return;
        }
        tmp = tmp.substring(0, j + 2);
        _playMorse(tmp);
        Serial.print(tmp);
        i = i + j + 2;
      } else {
        if (strToEncode[i] == ' ') {
          delay(_wordSpacing);
        } else {
          _playMorse(toLowerCase(strToEncode[i]));
          delay(_charSpacing);
        }
        Serial.print(strToEncode[i]);
        if (strToEncode[i] == '=') {
          Serial.println();
        }
        i++;
      }
    }
    Serial.println();
  }
}

void Encoder::_playMorse(char normalChar[]) {
  for (byte i = 0; i < ArraySize(morseMappings); i++) {
    MorseMapping mm;
    PROGMEM_readAnything (&morseMappings[i], mm);
    if (normalChar == mm.ch[0]) {
      _playMorseSequence(mm.morseSeq);
      return;
    }
  }
}

void Encoder::_playMorse(String normalChar) {
  for (byte i = 0; i < ArraySize(morseMappings); i++) {
    MorseMapping mm;
    PROGMEM_readAnything (&morseMappings[i], mm);
    String str = mm.ch;
    if (normalChar == str) {
      _playMorseSequence(mm.morseSeq);
      return;
    }
  }
}

void Encoder::_playMorseSequence(char morseSeq[]) {
  for (byte i = 0; i < _lengthof(morseSeq); i++) {
    if (morseSeq[i] == '.') {
      _di();
    } else if (morseSeq[i] == '_') {
      _dah();
    }
  }
}

//TODO: Duplicate fn in decoder and encoder
byte Encoder::_lengthof(char const str[]) {
  int i = 0;
  while (str[i] != '\0') {
    i++;
  }
  return i;
}

void Encoder::_pause(int delayTime) {
  noTone(_tonePin);
  delay(delayTime);
}

void Encoder::_di() {
  tone(_tonePin, _toneHz, _dotLen);
  delay(_dotLen);
  _pause(_symbolSpacing);
}

void Encoder::_dah() {
  tone(_tonePin, _toneHz, _dashLen);  // start playing a tone
  delay(_dashLen);               // hold in this position
  _pause(_symbolSpacing);
}
