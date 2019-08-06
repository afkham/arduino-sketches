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

#include "display.h"

#define TEXT_SIZE 1

Morseduino::Display::Display(uint8_t width, uint8_t height) {
    _ssd1306Display = new Adafruit_SSD1306(width, height, &Wire, -1);
}

Morseduino::Display::~Display() = default;

bool Morseduino::Display::init() {
    return _ssd1306Display->begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void Morseduino::Display::showText(const String& text) {
    if (_currentX == 0 && _currentY == 0) {
        _ssd1306Display->clearDisplay();
    }
    _ssd1306Display->setTextSize(TEXT_SIZE);
    _ssd1306Display->setTextColor(WHITE);
    _ssd1306Display->setCursor(_currentX, _currentY);
    _currentX += 6 * TEXT_SIZE * text.length();

    if (_currentX >= 110) {
        _currentX = 0;
        if (_currentY >= 48) {
            _currentY = _currentX = 0;
        } else {
            _currentY += 8 * TEXT_SIZE;
        }
    }
    _ssd1306Display->println(text);
    _ssd1306Display->display();
}

void Morseduino::Display::showProgress(const char *text, const uint16_t val, const uint16_t maxVal) {
    _printHeader();
    _ssd1306Display->setTextSize(2); // Draw 2X-scale text
    _ssd1306Display->setTextColor(WHITE);
    _ssd1306Display->setCursor(15, 15);
    _ssd1306Display->println(text);
    _ssd1306Display->setCursor(15, 32);
    _ssd1306Display->println(val);

    _ssd1306Display->drawRect(15, 55, 100, 5, WHITE);
    _ssd1306Display->fillRect(15, 55, round((float) 100 * val / maxVal), 5, WHITE);
    for (uint8_t i = 0; i < 90; i += 10) {
        _ssd1306Display->drawPixel(25 + i, 60, WHITE);
    }

    _ssd1306Display->display();
}

void Morseduino::Display::showHomeScreen(const uint8_t wpm, const uint16_t toneHz, const OpMode opMode) {
    _currentX = _currentY = 0;
    _printHeader();
    _ssd1306Display->setTextSize(2); // Draw 2X-scale text

    // Show WPM
    _ssd1306Display->setCursor(15, 15);
    _ssd1306Display->print(_wpmText);
    _ssd1306Display->setCursor(75, 15);
    _ssd1306Display->println(wpm);

    // Show Tone
    _ssd1306Display->setCursor(15, 33);
    _ssd1306Display->print(_toneText);
    _ssd1306Display->setCursor(75, 33);
    _ssd1306Display->println(toneHz);

    // Show
    _ssd1306Display->setCursor(15, 50);
    _ssd1306Display->print(_modeText);
    _ssd1306Display->setCursor(75, 50);
    if (opMode == dec) {
        _ssd1306Display->println(_decModeText);
    } else if (opMode == enc) {
        _ssd1306Display->println(_encModeText);
    }

    _ssd1306Display->display();
}

void Morseduino::Display::_printHeader() {
    _ssd1306Display->clearDisplay();
    _ssd1306Display->setTextSize(1);
    _ssd1306Display->setTextColor(WHITE);
    _ssd1306Display->setCursor(35, 2);
    _ssd1306Display->println(this->_header);
}

void Morseduino::Display::clear() {
    _currentX = _currentY = 0;
}
