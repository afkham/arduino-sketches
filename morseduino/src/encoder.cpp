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

Morseduino::Encoder::Encoder(uint8_t tonePin) : _tonePin(tonePin) {}

void Morseduino::Encoder::setDotLength(uint8_t dotLen) {
    WPM_RULES
}

void Morseduino::Encoder::setTone(uint16_t toneHz) {
    _toneHz = toneHz;
}

void Morseduino::Encoder::encode() {
    if (Serial.available() > 0) {
        String strFromSerial = Serial.readString();
        unsigned int len = strFromSerial.length();
        char strToEncode[len];
        strFromSerial.toCharArray(strToEncode, len);
        unsigned int i = 0;
        while (i < len) {
            if (strToEncode[i] == '<') { // Handle joint characters
                char jointChar[6];
                uint8_t k = 0;
                bool closingBracketFound = false;
                for (uint8_t j = i; j < len; j++) {
                    jointChar[k] = strToEncode[j];
                    i = j;
                    if (strToEncode[j] == '>') {
                        closingBracketFound = true;
                        jointChar[k + 1] = '\0';
                        Serial.print(jointChar);
                        _playMorse(jointChar);
                        delay(_charSpacing);
                        i++;
                        break;
                    }
                    k++;
                }
                if (!closingBracketFound) {
                    Serial.println(F("****** NO closing brackets. INVALID INPUT ******"));
                    return;
                }
            } else {
                if (strToEncode[i] == ' ') {
                    delay(_wordSpacing);
                } else if (strToEncode[i] != '\0') {
                    char ch[2] = {strToEncode[i], '\0'};
                    _playMorse(ch);
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

void Morseduino::Encoder::_playMorse(char normalChar[]) {
    for (uint8_t i = 0; i < ArraySize(morseMappings); i++) {
        MorseMapping mm;
        PROGMEM_readAnything(&morseMappings[i], mm);
        if (strcasecmp(normalChar, mm.ch) == 0) {
            _playMorseSequence(mm.morseSeq);
            return;
        }
    }
    Serial.println(F("****** INVALID INPUT ******"));
}

void Morseduino::Encoder::_playMorseSequence(char morseSeq[]) {
    for (uint8_t i = 0; i < strlen(morseSeq); i++) {
        if (morseSeq[i] == '.') {
            _di();
        } else if (morseSeq[i] == '_') {
            _dah();
        }
    }
}

void Morseduino::Encoder::_pause(uint8_t delayTime) {
    noTone(_tonePin);
    delay(delayTime);
}

void Morseduino::Encoder::_di() {
#ifndef TONE_OFF
    tone(_tonePin, _toneHz, _dotLen);
#endif
    delay(_dotLen);
    _pause(_symbolSpacing);
}

void Morseduino::Encoder::_dah() {
#ifndef TONE_OFF
    tone(_tonePin, _toneHz, _dashLen);  // start playing a tone
#endif
    delay(_dashLen);               // hold in this position
    _pause(_symbolSpacing);
}
