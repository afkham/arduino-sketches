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

namespace Morseduino {
    class Decoder {
    public:
        Decoder(Display *display, uint8_t keyPin, uint8_t tonePin);

        void setDotLength(uint8_t dotLen);

        void setTone(uint16_t toneHz);

        void decode();

        ~Decoder();

    private:
        Display *_display;
        uint8_t _keyPin = 0;  // Morse key pin
        uint8_t _tonePin = 0; // Pin where tone is played
        uint16_t _toneHz = 0; // Tone to play

        WPM_DECLARATIONS

        // The time at which a dot or dash started. This is used for identifying dots and dashes
        uint64_t _symbolStartedAt = 0;

        // The time at which the last dot or dash was received. This is used to identify character boundaries.
        uint64_t _lastSymbolReceivedAt = 0;

        // The time at which the last character was received. This is used for identifying word boundaries.
        uint64_t _lastCharReceivedAt = 0;

        // Holds the currently active symbol (dots and dashes) buffer.
        // This will be used later for identifying the character from the dots and dashes.
        static const uint8_t MAX_SYMBOLS = 10;
        char _currentSymbolBuff[MAX_SYMBOLS]; // Maximum possible symbols in Morse code is 10
        uint8_t _currentSymbolIndex = 0; // index to the currentSymbolBuff

        void _printChar(char morseStr[]);

        void _resetCurrentSymbolBuff();

        unsigned long _debounceTime;
        uint8_t _debounceDelay = 10;
    };
};

#endif
