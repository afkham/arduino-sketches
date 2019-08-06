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
#include "datastructure.h"

namespace Morseduino {
    class Encoder {
    public:
        Encoder(uint8_t tonePin);

        void setDotLength(uint8_t dotLen);

        void setTone(uint16_t toneHz);

        void encode();

    private:
        uint8_t _tonePin; // Pin where tone is played
        uint16_t _toneHz; // Tone to play

        WPM_DECLARATIONS

        void _di();

        void _dah();

        void _pause(uint8_t delayTime);

        void _playMorse(char normalChar[]);

        void _playMorseSequence(char morseSequence[]);
    };
};

#endif
