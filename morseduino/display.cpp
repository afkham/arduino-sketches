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

Display::Display(byte width, byte height) {
    _ssd1306Display = new Adafruit_SSD1306(width, height, &Wire, -1);
}

bool Display::init() {
    return _ssd1306Display->begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void Display::showProgress(String text, int val, int maxVal) {
  _printHeader();
  _ssd1306Display->setTextSize(2); // Draw 2X-scale text
  _ssd1306Display->setTextColor(WHITE);
  _ssd1306Display->setCursor(15, 15);
  _ssd1306Display->println(text);
  _ssd1306Display->setCursor(15, 32);
  _ssd1306Display->println(val);

  _ssd1306Display->drawRect(15, 55, 100, 5, WHITE);
  _ssd1306Display->fillRect(15, 55, round((float) 100 * val / maxVal), 5, WHITE);
  for (byte i = 0; i < 90; i += 10) {
    _ssd1306Display->drawPixel(25 + i, 60, WHITE);
  }

  _ssd1306Display->display();
}

void Display::showHomeScreen(byte wpm, unsigned int toneHz, String opMode) {
  _printHeader();
  _ssd1306Display->setTextSize(2); // Draw 2X-scale text
  _ssd1306Display->setCursor(15, 15);
  String wpmText = "WPM  ";
  wpmText.concat(wpm);
  _ssd1306Display->println(wpmText);
  _ssd1306Display->setCursor(15, 33);
  String toneText = "Tone ";
  toneText.concat(toneHz);
  _ssd1306Display->println(toneText);
  _ssd1306Display->setCursor(15, 50);
  String modeText = "Mode ";
  modeText.concat(opMode);
  _ssd1306Display->println(modeText);
  _ssd1306Display->display();
}

void Display::_printHeader() {
  _ssd1306Display->clearDisplay();
  _ssd1306Display->setTextSize(1);
  _ssd1306Display->setTextColor(WHITE);
  _ssd1306Display->setCursor(35, 2);
  _ssd1306Display->println("Morseduino");
}
