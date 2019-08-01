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

class Display {
  public:
    Display(byte width, byte height);
    bool init();
    void showHomeScreen(byte wpm, unsigned int toneHz, OpMode opMode);
    void showProgress(String text, int val, int maxVal);
	
  private:
    Adafruit_SSD1306* _ssd1306Display;
    void _printHeader();
};

#endif
