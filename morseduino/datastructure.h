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

/*
    The following template is used for reading from PROGMEM
    See: https://arduino.stackexchange.com/questions/13545/using-progmem-to-store-array-of-structs
*/
template <typename T> void PROGMEM_readAnything (const T * sce, T& dest) {
  memcpy_P (&dest, sce, sizeof (T));
}

/*
   Number of items in an array
*/
template< typename T, size_t N > size_t ArraySize (T (&) [N]) {
  return N;
}

// Character to Morse code mapping
typedef struct {
  char ch[6];
  char morseSeq[10];
} MorseMapping;

// Morse Alphabet
const MorseMapping morseMappings[47] PROGMEM = {
  {"a", "._"}, {"b", "_..."}, {"c", "_._."}, {"d", "_.."}, {"e", "."}, {"f", ".._."}, {"g", "__."},
  {"h", "...."}, {"i", ".."}, {"j", ".___"}, {"k", "_._"}, {"l", "._.."}, {"m", "__"}, {"n", "_."},
  {"o", "___"}, {"p", ".__."}, {"q", "__._"}, {"r", "._."}, {"s", "..."}, {"t", "_"}, {"u", ".._"},
  {"v", "..._"}, {"w", ".__"}, {"x", "_.._"}, {"y", "_.__"}, {"z", "__.."},
  {"1", ".____"}, {"2", "..___"}, {"3", "...__"}, {"4", "...._"}, {"5", "....."}, {"6", "_...."},
  {"7", "__..."}, {"8", "___.."}, {"9", "____."}, {"0", "_____"},
  {".", "._._._"}, {",", "__..__"}, {"?", "..__.."}, {"=", "_..._"}, {"+", "._._."}, {"-", "_...._"},
  {"<SOS>", "...___..."}, {"<KA>", "_._._"}, {"<AS>", "._..."}, {"<AR>", "._._."}, {"<SK>", "..._._"},
};

// --------------- Modes of operation -------------------------
enum OpMode {
  enc, // Encoder mode of operation
  dec // Decoder mode of operation
};
#endif
