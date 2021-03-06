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
/*
  ===============================================================================================================
  SimpleRotary.h
  Library for using rotary encoders with an Arduino
  Learn more at [https://github.com/mprograms/SimpleRotary]

  Supports:

  - Getting rotary encoder rotation direction.
  - Getting rotary select button press.
  - Getting rotary select button long press.
  - Getting rotary select button press time.
  - Basic debouncing of rotary / button.
  - Basic error correcting of out-of-sync rotational readings.
  - Use of both high and low triggers.
*/
#include "Arduino.h"
#include "rotary.h"

Morseduino::SimpleRotary::SimpleRotary(uint8_t pinA, uint8_t pinB, uint8_t pinS) {
    _pinA = pinA;
    _pinB = pinB;
    _pinS = pinS;
    _currentTime = millis();
    _debounceRTime = _currentTime;
    _debounceSTime = _currentTime;
    _errorTime = _currentTime;

    _setInputPins();
}


/**
	SET TRIGGER
	Sets the trigger for rotations / button press either to high or low.
	If set to HIGH then the library will automaticall enable pull up resisters for the rotaty
	encoder's pins.

	@since v0.1;
**/
void Morseduino::SimpleRotary::setTrigger(uint8_t i) {
    _trigger = i;
    _setInputPins();
}


/**
	SET DEBOUNCE DELAY
	Sets the debounce delay time in milliseconds.

	This number shold be set as small as possible. Higher values can result in missing input pulses.
	Finding this value will vary from one rotary encoder to another and can be found simply with
	trial and error.

	You can turn this feature off by setting the delay value to 0.

	@since v0.1;
**/
void Morseduino::SimpleRotary::setDebounceDelay(uint8_t i) {
    _debounceRDelay = i;
}


/**
	SET ERROR CORRECTION DELAY
	Sets the error correction delay delay time in milliseconds.

	Setting a higher number here will result in smoother direction values at the cost of being able to quickly change directions. 
	For example, setting a value of 250ms will require a the user to pause 1/4 second between changing direction of the rotary encoder. 
	It takes most users longer then 250ms to reorient their fingers to the new direction so in most cases this would be fine. 
	If a user was to change direction of the rotary encoder faster then 1/4 of a second then the output value would be the previous direction. 
	This number should be set to whatever delay works best for your application.

	You can turn this feature off by setting the delay value to 0.

	@since v0.1;
**/
void Morseduino::SimpleRotary::setErrorDelay(uint8_t i) {
    _errorDelay = i;
}


/**
	GET ROTARY DIRECTION
	Gets the direction the rotary encoder is turned.

	0x00 = Not turned.
	0x01 = Clockwise;
	0x02 = Counter-Clockwise

	@since v0.1;
**/
uint8_t Morseduino::SimpleRotary::rotate() {
    uint8_t _dir = 0x00;
    _updateTime();

    if (_currentTime >= (_debounceRTime + _debounceRDelay)) {

        _statusA = digitalRead(_pinA) == _trigger;
        _statusB = digitalRead(_pinB) == _trigger;

        if (!_statusA && _statusA_prev) {

            if (_statusB != _statusA) {
                _dir = 0x01;
            } else {
                _dir = 0x02;
            }

            if (_currentTime < (_errorTime + _errorDelay)) {
                _dir = _errorLast;
            } else {
                _errorLast = _dir;
            }

            _errorTime = _currentTime;

        }
        _statusA_prev = _statusA;
        _debounceRTime = _currentTime;
    }

    return _dir;

}


/**
	GET BUTTON PUSH
	Gets the status of the pushbutton

	Returned values
	0x00 = Not turned.
	0x01 = Clockwise;
	0x02 = Counter-Clockwise

	@since v0.1;
	@return uint8_t, value of turned knob.
**/
uint8_t Morseduino::SimpleRotary::push() {
    _updateTime();
    uint8_t val = 0x00;

    _statusS = digitalRead(_pinS) == _trigger;

    if (_currentTime >= _debounceSTime + _debounceSDelay) {
        if (!_statusS && _statusS_prev) {
            val = 0x01;
            _pulse = false;
            _pushTime = _currentTime;
        }
        _statusS_prev = _statusS;
        _debounceSTime = _currentTime;
    }
    return val;
}


/**
	GET LONG BUTTON PUSH TIME
	Gets the amount of time in milliseconds that the button is held down.

	@since v0.1;

	@return uint16_t, time in MS that the button has been held down.
**/
uint16_t Morseduino::SimpleRotary::pushTime() {
    uint16_t t = 0;
    if (!_statusS && !_statusS_prev) {
        t = _currentTime - _pushTime;
    }
    return t;
}


/**
	RESET PUSH TIMER
	Resets the push time back to the current time.

	Use this function if you want to create a long press that will reset itseld after (n) milliseconds thus
    causing a pulse to be created every time it resets. For example if you want to have the button do something
    each time it is held down for 1 second you would call:

	void loop(){
		uint16_t t = rotary.pushTime();
		if ( t > 1000 ) {
			rotary.resetPush();
			// do something really cool
		}
	}

	@since v0.1;
**/
void Morseduino::SimpleRotary::resetPush() {
    _updateTime();
    _pushTime = _currentTime;
}


/**
	GET LONG BUTTON PUSH
	Checks to see if the button has been held down for (n) milliseconds.

	If placed in a loop, this function will return a steady stream of 0's until the button has
	been held down long enough. Once the button has been held down (n)th milliseconds, a pulse of 1 will be produced.
    If the button is continued to be held down past this, only 0's will be produced.
	The button needs to be released and then pressed again in order to reset.

	Returned values
	0x00 = Button not pressed long enough
	0x01 = Button was pressed long enough.

	@param i, uint16_t, the number of milliseconds the button needs to be pressed in order
				   to be considered a long press.

	@since v0.1;

	@return uint8_t
**/
uint8_t Morseduino::SimpleRotary::pushLong(uint16_t i) {
    uint16_t time = pushTime();
    uint8_t val = 0x00;

    if ((_currentTime + time > _currentTime + i) && !_pulse) {
        val = 0x01;
        _pulse = true;
    }
    return val;
}


/**
	GET BUTTON PUSH TYPE
	Checks to see if the button push is a short or long push

	Note: Unlike push(), pushType() generates a return value for a short press only on button release.
    This allows us to track a long press without a false positive of a short press. It is for this reason that you should use this function if you are tying to check for a long or short press in the same function.

	Returned values
	0x00 = Button not pressed.
	0x01 = Button was pressed.
	0x00 = Button was pressed for n milliseconds.

	@param i, uint16_t, the number of milliseconds the button needs to be pressed in order
				   to be considered a long press.

	@since v1.1.0;

	@return uint8_t
**/
uint8_t Morseduino::SimpleRotary::pushType(uint16_t i = 1000) {
    _updateTime();
    _statusS = digitalRead(_pinS) == _trigger;
    uint8_t val = 0x00;

    if (_currentTime >= _debounceSTime + _debounceSDelay) {

        // Button has been pressed
        if (!_statusS && _statusS_prev) {
            _btnPressed = true;
            _pushTime = _currentTime;
        }

        // Button has been released
        if (_statusS && !_statusS_prev && _btnPressed) {
            _btnPressed = false;
            val = 0x01;
        }
        // Button is being held
        if (!_statusS && !_statusS_prev && _btnPressed) {
            if (_currentTime > _pushTime + i) {
                _btnPressed = false;
                val = 0x02;
            }
        }

        _statusS_prev = _statusS;
        _debounceSTime = _currentTime;
    }


    return val;
}


/**
	SET INPUT PINS
	Sets the input pins and pinmode based on the defined pins and _trigger value.

	@see setTrigger();

	@since v0.2;
**/
void Morseduino::SimpleRotary::_setInputPins() {
    if (_trigger == HIGH) {
        pinMode(_pinA, INPUT_PULLUP);
        pinMode(_pinB, INPUT_PULLUP);
        pinMode(_pinS, INPUT_PULLUP);
    } else {
        pinMode(_pinA, INPUT);
        pinMode(_pinB, INPUT);
        pinMode(_pinS, INPUT);
    }
}


/**
	UPDATE THE TIME
	Updates the _currentTime value to the current time in milliseconds.

	@since v0.1;
**/
void Morseduino::SimpleRotary::_updateTime() {
    _currentTime = millis();
}
