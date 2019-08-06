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
#ifndef SimpleRotary_h
#define SimpleRotary_h

#include "Arduino.h"

namespace Morseduino {
    class SimpleRotary {
    public:
        SimpleRotary(uint8_t pinA, uint8_t pinB, uint8_t pinS);

        uint8_t rotate();

        uint8_t push();

        uint16_t pushTime();

        uint8_t pushLong(uint16_t i);

        uint8_t pushType(uint16_t i);

        void setTrigger(uint8_t i);

        void setDebounceDelay(uint8_t i);

        void setErrorDelay(uint8_t i);

        void resetPush();

    private:
        uint8_t _trigger = HIGH;
        uint8_t _pinA;
        uint8_t _pinB;
        uint8_t _pinS;
        unsigned long _currentTime;
        unsigned long _debounceRTime;
        unsigned long _debounceSTime;
        unsigned long _pushTime;
        uint8_t _debounceRDelay = 2;
        uint8_t _debounceSDelay = 200;
        bool _pulse = false;
        bool _pushStatus = false;
        bool _btnPressed = false;
        unsigned long _errorTime;
        unsigned int _errorDelay = 100;
        uint8_t _errorLast = 0;
        bool _statusA = false;
        bool _statusB = false;
        bool _statusS = false;
        bool _statusA_prev = false;
        bool _statusS_prev = false;

        void _updateTime();

        void _setInputPins();
    };
};

#endif
