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

#ifndef MorseduinoDataStructure_h
#define MorseduinoDataStructure_h

#define ENABLE_SERIAL 1
//#define TONE_OFF 1

/*
    There are rules to help people distinguish dots from dashes in Morse code.

    1. The length of a dot is 1 time unit.
    2. A dash is 3 time units.
    3. The space between symbols (dots and dashes) of the same letter is 1 time unit.
    4. The space between letters is 3 time units.
    5. The space between words is 7 time units.
*/
#define WPM_DECLARATIONS \
    uint8_t _dotLen = 0;   \
    uint16_t _dashLen = 0;  \
    uint8_t _symbolSpacing = 0;  \
    uint16_t _charSpacing = 0; \
    uint16_t _wordSpacing = 0; \

#define WPM_RULES \
    _dotLen = dotLen; \
    _dashLen = dotLen * 3; \
    _symbolSpacing = dotLen; \
    _charSpacing = dotLen * 3; \
    _wordSpacing = dotLen * 7; \

/*
    The following template is used for reading from PROGMEM
    See: https://arduino.stackexchange.com/questions/13545/using-progmem-to-store-array-of-structs
*/
template<typename T>
void PROGMEM_readAnything(const T *sce, T &dest) {
    memcpy_P(&dest, sce, sizeof(T));
}

/*
   Number of items in an array
*/
template<typename T, size_t N>
size_t ArraySize(T (&)[N]) {
    return N;
}

// Character to Morse code mapping
struct MorseMapping {
    char ch[6];
    char morseSeq[10];
};

// Morse Alphabet
const PROGMEM MorseMapping morseMappings[47] = {
        {"A",     "._"},
        {"B",     "_..."},
        {"C",     "_._."},
        {"D",     "_.."},
        {"E",     "."},
        {"F",     ".._."},
        {"G",     "__."},
        {"H",     "...."},
        {"I",     ".."},
        {"J",     ".___"},
        {"K",     "_._"},
        {"L",     "._.."},
        {"M",     "__"},
        {"N",     "_."},
        {"O",     "___"},
        {"P",     ".__."},
        {"Q",     "__._"},
        {"R",     "._."},
        {"S",     "..."},
        {"T",     "_"},
        {"U",     ".._"},
        {"V",     "..._"},
        {"W",     ".__"},
        {"X",     "_.._"},
        {"Y",     "_.__"},
        {"Z",     "__.."},
        {"1",     ".____"},
        {"2",     "..___"},
        {"3",     "...__"},
        {"4",     "...._"},
        {"5",     "....."},
        {"6",     "_...."},
        {"7",     "__..."},
        {"8",     "___.."},
        {"9",     "____."},
        {"0",     "_____"},
        {".",     "._._._"},
        {",",     "__..__"},
        {"?",     "..__.."},
        {"=",     "_..._"},
        {"+",     "._._."},
        {"-",     "_...._"},
        {"<SOS>", "...___..."},
        {"<KA>",  "_._._"},
        {"<AS>",  "._..."},
        {"<AR>",  "._._."},
        {"<SK>",  "..._._"},
};

// --------------- Modes of operation -------------------------
enum OpMode {
    invalid,
    dec, // Decoder mode of operation
    enc // Encoder mode of operation
};

#endif
