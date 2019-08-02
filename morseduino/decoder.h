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
#ifndef MorseduinoDecoder_h
#define MorseduinoDecoder_h

#include <Arduino.h>
#include "display.h"

class Decoder {
  public:
    Decoder(Display* display, byte keyPin, byte tonePin);
    void setDotLength(byte dotLen);
    void setTone(int toneHz);
    void decode();
    ~Decoder();

  private:
    Display* _display;
    byte _keyPin;  // Morse key pin
    byte _tonePin; // Pin where tone is played
    int _toneHz; // Tone to play

    byte _dotLen;     // length of the morse code 'dot'
    int _dashLen;    // length of the morse code 'dash'
    int _symbolSpacing; // length of the pause between elements of a character
    int _charSpacing; // length of the spaces between characters
    int _wordSpacing; // length of the pause between words

    // The time at which a dot or dash started. This is used for identifying dots and dashes
    int _symbolStartedAt = -1;

    // The time at which the last dot or dash was received. This is used to identify character boundaries.
    int _lastSymbolReceivedAt = -1;

    // The time at which the last character was received. This is used for identifying word boundaries.
    int _lastCharReceivedAt = -1;

    // Holds the currently active symbol (dots and dashes) buffer.
    // This will be used later for identifying the character from the dots and dashes.
    static const byte MAX_SYMBOLS = 10;
    char _currentSymbolBuff[MAX_SYMBOLS]; // Maximum possible symbols in Morse code is 10
    byte _currentSymbolIndex = 0; // index to the currentSymbolBuff
    bool _garbageReceived = false; // Indicates whether the received symbol sequence is invalid

    void _printChar(char morseStr[]);
    bool _isEqual(char ch1[], char const ch2[]);
    byte _lengthof(char const str[]);
    void _pause(int delayTime);
    void _resetCurrentSymbolBuff();
};

#endif
