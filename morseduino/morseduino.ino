
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

const int TONE_PIN = 8;      // output audio on pin 8
const int KEY_PIN = 2;       // Morse key pin
const int MODE_SELECT_PIN = 4; // Pin for switching between Morse key input and serial input
const int TONE_HZ = 1850;      // music TONE_HZ/pitch in Hertz

// Character to Morse code mapping
typedef struct {
  char ch[6];
  char morseSeq[10];
} MorseMapping;

const int MAPPING_SIZE = 47;

// Morse Alphabet
const MorseMapping morseMappings[MAPPING_SIZE] = {
  {"a", "._"}, {"b", "_..."}, {"c", "_._."}, {"d", "_.."}, {"e", "."}, {"f", ".._."}, {"g", "__."},
  {"h", "...."}, {"i", ".."}, {"j", ".___"}, {"k", "_._"}, {"l", "._.."}, {"m", "__"}, {"n", "_."},
  {"o", "___"}, {"p", ".__."}, {"q", "__._"}, {"r", "._."}, {"s", "..."}, {"t", "_"}, {"u", "_."},
  {"v", "..._"}, {"w", ".__"}, {"x", "_.._"}, {"y", "_.__"}, {"z", "__.."},
  {"1", ".____"}, {"2", "..___"}, {"3", "...__"}, {"4", "...._"}, {"5", "....."}, {"6", "_...."},
  {"7", "__..."}, {"8", "___.."}, {"9", "____."}, {"0", "_____"},
  {".", "._._._"}, {",", "__..__"}, {"?", "..__.."}, {"=", "_..._"}, {"+", "._._."}, {"-", "_...._"},
  {"<SOS>", "...___..."}, {"<KA>", "_._._"}, {"<AS>", "._..."}, {"<AR>", "._._."}, {"<SK>", "..._._"},
};

void setup() {
  pinMode(KEY_PIN, INPUT);
  pinMode(MODE_SELECT_PIN, INPUT);
  Serial.begin(9600);
}

/*
  Set the speed of your morse code
  Adjust the DOT_LEN length to speed up or slow down your morse code
    (all of the other lengths are based on the DOT_LEN)

  There are rules to help people distinguish dots from dashes in Morse code.

  1. The length of a dot is 1 time unit.
  2. A dash is 3 time units.
  3. The space between symbols (dots and dashes) of the same letter is 1 time unit.
  4. The space between letters is 3 time units.
  5. The space between words is 7 time units.
*/
const int DOT_LEN = 80;     // length of the morse code 'dot'
const int DASH_LEN = DOT_LEN * 3;    // length of the morse code 'dash'
const int SYMBOL_SPACING = DOT_LEN;  // length of the pause between elements of a character
const int CHAR_SPACING = DOT_LEN * 3;     // length of the spaces between characters
const int WORD_SPACING = DOT_LEN * 7;  // length of the pause between words

int morseKeyState;             // the current reading from the Morse key

void loop() {
  digitalRead(MODE_SELECT_PIN) ? playCodeFromSerial() : playOscillator();
}

// ------------ functions for oscillator mode -----------

// The time at which a dot or dash started. This is used for identifying dots and dashes
int symbolStartedAt = -1;

// The time at which the last dot or dash was received. This is used to identify character boundaries.
int lastSymbolReceivedAt = -1;

// The time at which the last character was received. This is used for identifying word boundaries.
int lastCharReceivedAt = -1;

// Holds the currently active symbol (dots and dashes) buffer.
// This will be used later for identifying the character from the dots and dashes.
const int MAX_SYMBOLS = 10;
char currentSymbolBuff[MAX_SYMBOLS]; // Maximum possible symbols in Morse code is 10
int currentSymbolIndex = 0; // index to the currentSymbolBuff
bool garbageReceived = false; // Indicates whether the received symbol sequence is invalid

void playOscillator() {
  morseKeyState = digitalRead(KEY_PIN);
  if (morseKeyState == HIGH) {
    int now = millis();
    if (symbolStartedAt == -1) {
      symbolStartedAt = now;
    }
    tone(TONE_PIN, TONE_HZ);
  } else {
    noTone(TONE_PIN);
    if (symbolStartedAt != -1) {
      if (currentSymbolIndex > MAX_SYMBOLS) {
        garbageReceived = true;
      } else if (!garbageReceived) {
        int now = millis();
        if (now - symbolStartedAt >= DASH_LEN) {
          currentSymbolBuff[currentSymbolIndex++] = '_';
        } else {
          currentSymbolBuff[currentSymbolIndex++] = '.';
        }
        symbolStartedAt = -1;
        lastSymbolReceivedAt = millis();
        lastCharReceivedAt = -1;
      }
    }
    int now = millis();
    if (lastCharReceivedAt != -1 && now - lastCharReceivedAt > WORD_SPACING) { // Have we completed a word?
      Serial.print(" ");
      lastCharReceivedAt = -1;
    } else if (lastSymbolReceivedAt != -1 && now - lastSymbolReceivedAt > CHAR_SPACING) { // Have we completed a character?
      printChar(currentSymbolBuff);
      garbageReceived = false;
      resetCurrentSymbolBuff();
      lastSymbolReceivedAt = -1;
      lastCharReceivedAt = millis();
    }
  }
}

void resetCurrentSymbolBuff() {
  for (int i = 0; i < MAX_SYMBOLS; i++) {
    currentSymbolBuff[i] = '\0';
  }
  currentSymbolIndex = 0;
}

void pause(int delayTime) {
  noTone(TONE_PIN);
  delay(delayTime);
}

int lengthof(char const str[]) {
  int i = 0;
  while (str[i] != '\0') {
    i++;
  }
  return i;
}

bool isEqual(char ch1[], char const ch2[]) {
  if (lengthof(ch1) != lengthof(ch2)) return false;
  int i = 0;
  while (ch1[i] != '\0' && ch2[i] != '\0') {
    if (ch1[i] != ch2[i]) return false;
    i++;
  }
  return true;
}

void printChar(char morseStr[]) {
  for (int i = 0; i < MAPPING_SIZE; i++) {
    MorseMapping mm = morseMappings[i];
    if (isEqual(morseStr, mm.morseSeq)) {
      Serial.print(mm.ch);
      if (mm.ch == '=') {
        Serial.println();
      }
      return;
    }
  }
  Serial.print('#');
}

// ------------ functions for playing Morse mode -----------

void playCodeFromSerial() {
  if (Serial.available() > 0) {
    String strToPlay = Serial.readString();
    int i = 0;
    int len = strToPlay.length();
    while (i < len) {
      if (strToPlay[i] == '<') { // Handle joint characters
        String tmp = strToPlay.substring(i);
        int j = tmp.substring(1).indexOf('>');
        if (j == -1) {
          Serial.println("####### INVALID INPUT #######");
          return;
        }
        tmp = tmp.substring(0, j + 2);
        playMorse(tmp);
        Serial.print(tmp);
        i = i + j + 2;
      } else {
        if (strToPlay[i] == ' ') {
          delay(WORD_SPACING);
        } else {
          playMorse(toLowerCase(strToPlay[i]));
          delay(CHAR_SPACING);
        }
        Serial.print(strToPlay[i]);
        i++;
      }
    }
    Serial.println();
  }
}

void playMorse(char normalChar[]) {
  for (int i = 0; i < MAPPING_SIZE; i++) {
    MorseMapping mm = morseMappings[i];
    if (normalChar == mm.ch[0]) {
      playMorseSequence(mm.morseSeq);
      return;
    }
  }
}

void playMorse(String normalChar) {
  for (int i = 0; i < MAPPING_SIZE; i++) {
    MorseMapping mm = morseMappings[i];
    String str = mm.ch;
    if (normalChar == str) {
      playMorseSequence(mm.morseSeq);
      return;
    }
  }
}

void playMorseSequence(char morseSequence[]) {
  for (int i = 0; i < lengthof(morseSequence); i++) {
    if (morseSequence[i] == '.') {
      di();
    } else if (morseSequence[i] == '_') {
      dah();
    }
  }
}

void di() {
  tone(TONE_PIN, TONE_HZ, DOT_LEN);
  delay(DOT_LEN);
  pause(SYMBOL_SPACING);
}

void dah() {
  tone(TONE_PIN, TONE_HZ, DASH_LEN);  // start playing a tone
  delay(DASH_LEN);               // hold in this position
  pause(SYMBOL_SPACING);
}
