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
#ifndef MorseduinoEncoder_h
#define MorseduinoEncoder_h

#include <Arduino.h>

class Encoder {
  public:
    Encoder(byte tonePin);
    void setDotLength(byte dotLen);
    void setTone(int toneHz);
    void encode();

  private:
    byte _tonePin; // Pin where tone is played
    int _toneHz; // Tone to play
    
    byte _dotLen;     // length of the morse code 'dot'
    int _dashLen;    // length of the morse code 'dash'
    int _symbolSpacing; // length of the pause between elements of a character
    int _charSpacing; // length of the spaces between characters
    int _wordSpacing; // length of the pause between words

    void _di();
    void _dah();
    void _pause(int delayTime);
    byte _lengthof(char const str[]);
    void _playMorse(char normalChar[]);
    void _playMorse(String normalChar);
    void _playMorseSequence(char morseSequence[]);
};

#endif
