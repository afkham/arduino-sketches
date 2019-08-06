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

#ifndef MorseduinoDisplay_h
#define MorseduinoDisplay_h

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "datastructure.h"

namespace Morseduino {
    class Display {
    public:
        Display(uint8_t width, uint8_t height);

        ~Display();

        bool init();

        void showHomeScreen(uint8_t wpm, uint16_t toneHz, OpMode opMode);

        void showProgress(const char *text, uint16_t val, uint16_t maxVal);

        void showText(const String& text);

        void clear();

    private:
        Adafruit_SSD1306 *_ssd1306Display;

        void _printHeader();

        uint8_t _currentX = 0;
        uint8_t _currentY = 0;
        const char _header[11] = "Morseduino";
        const char _wpmText[6] = "WPM  ";
        const char _toneText[6] = "Tone ";
        const char _modeText[6] = "Mode ";
        const char _decModeText[4] = "DEC";
        const char _encModeText[4] = "ENC";
    };
};

#endif
