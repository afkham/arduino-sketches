
const String CONST_STRING = "cq cq cq 4s7kg";

const int TONE_PIN = 8;      // output audio on pin 8
const int KEY_PIN = 2;       // Morse key pin
const int MODE_SELECT_PIN = 4; // Pin for switching between Morse key input and serial input
const int TONE_HZ = 1800;      // music TONE_HZ/pitch in Hertz

void setup() {
  pinMode(KEY_PIN, INPUT);
  pinMode(MODE_SELECT_PIN, INPUT);
  Serial.begin(9600);
}

// Morse Alphabet
const char MORSE_A[] = "._";
const char MORSE_B[] = "_...";
const char MORSE_C[] = "_._.";
const char MORSE_D[] = "_..";
const char MORSE_E[] = ".";
const char MORSE_F[] = ".._.";
const char MORSE_G[] = "__.";
const char MORSE_H[] = "....";
const char MORSE_I[] = "..";
const char MORSE_J[] = ".___";
const char MORSE_K[] = "_._";
const char MORSE_L[] = "._..";
const char MORSE_M[] = "__";
const char MORSE_N[] = "_.";
const char MORSE_O[] = "___";
const char MORSE_P[] = ".__.";
const char MORSE_Q[] = "__._";
const char MORSE_R[] = "._.";
const char MORSE_S[] = "...";
const char MORSE_T[] = "_";
const char MORSE_U[] = "_.";
const char MORSE_V[] = "..._";
const char MORSE_W[] = ".__";
const char MORSE_X[] = "_.._";
const char MORSE_Y[] = "_.__";
const char MORSE_Z[] = "__..";
const char MORSE_1[] = ".____";
const char MORSE_2[] = "..___";
const char MORSE_3[] = "...__";
const char MORSE_4[] = "...._";
const char MORSE_5[] = ".....";
const char MORSE_6[] = "_....";
const char MORSE_7[] = "__...";
const char MORSE_8[] = "___..";
const char MORSE_9[] = "____.";
const char MORSE_0[] = "_____";
const char MORSE_PERIOD[] = "._._._";
const char MORSE_COMMA[] = "__..__";
const char MORSE_QUESTION_MARK[] = "..__..";
const char MORSE_EQ[] = "_..._";
const char MORSE_PLUS[] = "._._.";
const char MORSE_MINUS[] = "_...._";

const char MORSE_SOS[] = "...___...";
const char MORSE_START[] = "_._._";
const char MORSE_PLEASE_STOP[] = "._...";
const char MORSE_END_OF_TX[] = "._._.";
const char MORSE_END_OF_WORK[] = "..._._";

/*
  Set the speed of your morse code
  Adjust the 'DOT_LEN' length to speed up or slow down your morse code
    (all of the other lengths are based on the DOT_LEN)

  Here are the ratios code elements:
    Dash length = Dot length x 3
    Pause between elements = Dot length
      (pause between dots and dashes within the character)
    Pause between characters = Dot length x 3
    Pause between words = Dot length x 7
*/
const int DOT_LEN = 60;     // length of the morse code 'dot'
const int DASH_LEN = DOT_LEN * 3;    // length of the morse code 'dash'
const int SYMBOL_SPACING = DOT_LEN;  // length of the pause between elements of a character
const int CHAR_SPACING = DOT_LEN * 3;     // length of the spaces between characters
const int WORD_SPACING = DOT_LEN * 7;  // length of the pause between words

// Selection between player mode and oscillator mode
const int MODE_OSCILLATOR = 0;
const int MODE_READ_FROM_SERIAL = 1;
int mode = MODE_OSCILLATOR;

int morseKeyState;             // the current reading from the Morse key

void loop() {
  int mode = (digitalRead(MODE_SELECT_PIN) == HIGH) ? MODE_READ_FROM_SERIAL : MODE_OSCILLATOR;
  switch (mode) {
    case MODE_OSCILLATOR:
      playOscillator();
      break;
    case MODE_READ_FROM_SERIAL:
      playCodeFromSerial();
      break;
    default:
      break;
  }
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
          //            Serial.print("_");
          currentSymbolBuff[currentSymbolIndex++] = '_';
        } else {
          //            Serial.print(".");
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
  if (isEqual(morseStr, MORSE_A)) {
    Serial.print('a');
  } else if (isEqual(morseStr, MORSE_B)) {
    Serial.print('b');
  } else if (isEqual(morseStr, MORSE_C)) {
    Serial.print('c');
  } else if (isEqual(morseStr, MORSE_D)) {
    Serial.print('d');
  } else if (isEqual(morseStr, MORSE_E)) {
    Serial.print('e');
  } else if (isEqual(morseStr, MORSE_F)) {
    Serial.print('f');
  } else if (isEqual(morseStr, MORSE_G)) {
    Serial.print('g');
  } else if (isEqual(morseStr, MORSE_H)) {
    Serial.print('h');
  } else if (isEqual(morseStr, MORSE_I)) {
    Serial.print('i');
  } else if (isEqual(morseStr, MORSE_J)) {
    Serial.print('j');
  } else if (isEqual(morseStr, MORSE_K)) {
    Serial.print('k');
  } else if (isEqual(morseStr, MORSE_L)) {
    Serial.print('l');
  } else if (isEqual(morseStr, MORSE_M)) {
    Serial.print('m');
  } else if (isEqual(morseStr, MORSE_N)) {
    Serial.print('n');
  } else if (isEqual(morseStr, MORSE_O)) {
    Serial.print('o');
  } else if (isEqual(morseStr, MORSE_P)) {
    Serial.print('p');
  } else if (isEqual(morseStr, MORSE_Q)) {
    Serial.print('q');
  } else if (isEqual(morseStr, MORSE_R)) {
    Serial.print('r');
  } else if (isEqual(morseStr, MORSE_S)) {
    Serial.print('s');
  } else if (isEqual(morseStr, MORSE_T)) {
    Serial.print('t');
  } else if (isEqual(morseStr, MORSE_U)) {
    Serial.print('u');
  } else if (isEqual(morseStr, MORSE_V)) {
    Serial.print('v');
  } else if (isEqual(morseStr, MORSE_W)) {
    Serial.print('w');
  } else if (isEqual(morseStr, MORSE_X)) {
    Serial.print('x');
  } else if (isEqual(morseStr, MORSE_Y)) {
    Serial.print('y');
  } else if (isEqual(morseStr, MORSE_Z)) {
    Serial.print('z');
  } else if (isEqual(morseStr, MORSE_1)) {
    Serial.print('1');
  } else if (isEqual(morseStr, MORSE_2)) {
    Serial.print('2');
  } else if (isEqual(morseStr, MORSE_3)) {
    Serial.print('3');
  } else if (isEqual(morseStr, MORSE_4)) {
    Serial.print('4');
  } else if (isEqual(morseStr, MORSE_5)) {
    Serial.print('5');
  } else if (isEqual(morseStr, MORSE_6)) {
    Serial.print('6');
  } else if (isEqual(morseStr, MORSE_7)) {
    Serial.print('7');
  } else if (isEqual(morseStr, MORSE_8)) {
    Serial.print('8');
  } else if (isEqual(morseStr, MORSE_9)) {
    Serial.print('9');
  } else if (isEqual(morseStr, MORSE_0)) {
    Serial.print('0');
  } else if (isEqual(morseStr, MORSE_PERIOD)) {
    Serial.print('.');
  } else if (isEqual(morseStr, MORSE_COMMA)) {
    Serial.print(',');
  } else if (isEqual(morseStr, MORSE_QUESTION_MARK)) {
    Serial.print('?');
  } else if (isEqual(morseStr, MORSE_EQ)) {
    Serial.print('=');
  } else if (isEqual(morseStr, MORSE_PLUS)) {
    Serial.print('+');
  } else if (isEqual(morseStr, MORSE_MINUS)) {
    Serial.print('-');
  } else if (isEqual(morseStr, MORSE_SOS)) {
    Serial.print("|SOS|");
  } else if (isEqual(morseStr, MORSE_PLEASE_STOP)) {
    Serial.print("|AS|");
  } else if (isEqual(morseStr, MORSE_END_OF_TX)) {
    Serial.print("|AR|");
  } else if (isEqual(morseStr, MORSE_END_OF_WORK)) {
    Serial.print("|SK|");
  } else if (isEqual(morseStr, MORSE_START)) {
    Serial.print("|KA|");
  } else if (garbageReceived) {
    Serial.print('#');
  } else {
    //    Serial.println();
    //    Serial.println(morseStr);
    Serial.print('#');
  }
}

// ------------ functions for playing Morse mode -----------

void playCodeFromSerial() {
  if (Serial.available() > 0) {
    String strToPlay = Serial.readString();
    char str[strToPlay.length() + 1] ;
    strToPlay.toCharArray(str, strToPlay.length() + 1);
    for (int i = 0; i < sizeof(str) - 1; i++) {
      playMorse(toLowerCase(str[i]));
      delay(CHAR_SPACING);
      Serial.print(str[i]);
    }
    Serial.println();
  }
}

void playMorse(char normalChar) {
  switch (normalChar) {
    case 'a':
      playMorseSequence(MORSE_A);
      break;
    case 'b':
      playMorseSequence(MORSE_B);
      break;
    case 'c':
      playMorseSequence(MORSE_C);
      break;
    case 'd':
      playMorseSequence(MORSE_D);
      break;
    case 'e':
      playMorseSequence(MORSE_E);
      break;
    case 'f':
      playMorseSequence(MORSE_F);
      break;
    case 'g':
      playMorseSequence(MORSE_G);
      break;
    case 'h':
      playMorseSequence(MORSE_H);
      break;
    case 'i':
      playMorseSequence(MORSE_I);
      break;
    case 'j':
      playMorseSequence(MORSE_J);
      break;
    case 'k':
      playMorseSequence(MORSE_K);
      break;
    case 'l':
      playMorseSequence(MORSE_L);
      break;
    case 'm':
      playMorseSequence(MORSE_M);
      break;
    case 'n':
      playMorseSequence(MORSE_N);
      break;
    case 'o':
      playMorseSequence(MORSE_O);
      break;
    case 'p':
      playMorseSequence(MORSE_P);
      break;
    case 'q':
      playMorseSequence(MORSE_Q);
      break;
    case 'r':
      playMorseSequence(MORSE_R);
      break;
    case 's':
      playMorseSequence(MORSE_S);
      break;
    case 't':
      playMorseSequence(MORSE_T);
      break;
    case 'u':
      playMorseSequence(MORSE_U);
      break;
    case 'v':
      playMorseSequence(MORSE_V);
      break;
    case 'w':
      playMorseSequence(MORSE_W);
      break;
    case 'x':
      playMorseSequence(MORSE_X);
      break;
    case 'y':
      playMorseSequence(MORSE_Y);
      break;
    case 'z':
      playMorseSequence(MORSE_Z);
      break;
    case '1':
      playMorseSequence(MORSE_1);
      break;
    case '2':
      playMorseSequence(MORSE_2);
      break;
    case '3':
      playMorseSequence(MORSE_3);
      break;
    case '4':
      playMorseSequence(MORSE_4);
      break;
    case '5':
      playMorseSequence(MORSE_5);
      break;
    case '6':
      playMorseSequence(MORSE_6);
      break;
    case '7':
      playMorseSequence(MORSE_7);
      break;
    case '8':
      playMorseSequence(MORSE_8);
      break;
    case '9':
      playMorseSequence(MORSE_9);
      break;
    case '0':
      playMorseSequence(MORSE_0);
      break;
    case '.':
      playMorseSequence(MORSE_PERIOD);
      break;
    case ',':
      playMorseSequence(MORSE_COMMA);
      break;
    case '?':
      playMorseSequence(MORSE_QUESTION_MARK);
      break;
    case '+':
      playMorseSequence(MORSE_PLUS);
      break;
    case '-':
      playMorseSequence(MORSE_MINUS);
      break;
    case '=':
      playMorseSequence(MORSE_EQ);
      break;
    case ' ':
      pause(WORD_SPACING);
      break;
    default:
      pause(CHAR_SPACING);
  }
}

void playMorseSequence(String morseSequence) {
  char morseCharSeq[morseSequence.length() + 1] ;
  morseSequence.toCharArray(morseCharSeq, morseSequence.length() + 1);
  for (int i = 0; i < sizeof(morseCharSeq) - 1; i++) {
    morseCharSeq[i] == '.' ? di() : dah();
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
