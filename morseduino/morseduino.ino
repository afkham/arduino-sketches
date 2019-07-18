
const String CONST_STRING = "cq cq cq 4s7kg";

const int tonePin = 8;      // output audio on pin 8
const int keyPin = 2;       // Morse key pin
const int note = 1750;      // music note/pitch

/*
  Set the speed of your morse code
  Adjust the 'dotlen' length to speed up or slow down your morse code
    (all of the other lengths are based on the dotlen)

  Here are the ratios code elements:
    Dash length = Dot length x 3
    Pause between elements = Dot length
      (pause between dots and dashes within the character)
    Pause between characters = Dot length x 3
    Pause between words = Dot length x 7
*/
const int dotLen = 100;     // length of the morse code 'dot'
const int dashLen = dotLen * 3;    // length of the morse code 'dash'
const int elemPause = dotLen;  // length of the pause between elements of a character
const int charSpacing = dotLen * 3;     // length of the spaces between characters
const int wordSpacing = dotLen * 7;  // length of the pause between words

const int MODE_PLAY_CODED_STRING = 0;
const int MODE_READ_FROM_SERIAL = 1;
const int MODE_OSCILLATOR = 2;

// int mode = MODE_PLAY_CODED_STRING;
// int mode = MODE_READ_FROM_SERIAL;
int mode = MODE_OSCILLATOR;

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
}

void loop() {
  switch (mode) {
    case MODE_PLAY_CODED_STRING:
      playCode(CONST_STRING);
      break;
    case MODE_READ_FROM_SERIAL:
      playCodeFromSerial();
      break;
    case MODE_OSCILLATOR:
      playOscillator();
      break;  
    default:
      break;
  }
}

void playOscillator() {
  int buttonState = digitalRead(keyPin);
  if(buttonState == HIGH) {
    tone(tonePin, note, dotLen); 
  } else {
    noTone(tonePin); 
  }
}

void playCodeFromSerial() {
  if (Serial.available() > 0) {
    String serialRead = Serial.readString();
    playCode(serialRead);
  }
}

void playCode(String strToPlay) {
  char str[strToPlay.length() + 1] ;
  strToPlay.toCharArray(str, strToPlay.length() + 1);
  for (int i = 0; i < sizeof(str) - 1; i++) {
    playMoreChar(toLowerCase(str[i]));
    delay(dotLen * 2);
    Serial.print(str[i]);
  }
  Serial.println();
  pause(5000);
}

void dit() {
  di();
}
void di() {

  tone(tonePin, note, dotLen); 
  delay(dotLen);              
  pause(elemPause);
}

void dah() {
  tone(tonePin, note, dashLen);  // start playing a tone
  delay(dashLen);               // hold in this position
  pause(elemPause);
}

void pause(int delayTime) {
  noTone(tonePin);             
  delay(delayTime);    
}


void playMoreChar(char normalChar)
{
  // Take the passed character and use a switch case to find the morse code for that character
  switch (normalChar) {
    case 'a':
      di(); dah();
      break;
    case 'b':
      dah(); di(); di(); dit();
      break;
    case 'c':
      dah(); di(); dah(); dit();
      break;
    case 'd':
      dah(); di(); dit();
      break;
    case 'e':
      di();
      break;
    case 'f':
      di(); di(); dah(); dit();
      break;
    case 'g':
      dah(); dah(); dit();
      break;
    case 'h':
      di(); di(); di(); dit();
      break;
    case 'i':
      di(); dit();
      break;
    case 'j':
      di(); dah(); dah(); dah();
      break;
    case 'k':
      dah(); di(); dah();
      break;
    case 'l':
      di(); dah(); di(); dit();
      break;
    case 'm':
      dah(); dah();
      break;
    case 'n':
      dah(); dit();
      break;
    case 'o':
      dah(); dah(); dah();
      break;
    case 'p':
      di(); dah(); dah(); dit();
      break;
    case 'q':
      dah(); dah(); di(); dah();
      break;
    case 'r':
      di(); dah(); dit();
      break;
    case 's':
      di(); di(); dit();
      break;
    case 't':
      dah();
      break;
    case 'u':
      di(); di(); dah();
      break;
    case 'v':
      di(); di(); di(); dah();
      break;
    case 'w':
      di(); dah(); dah();
      break;
    case 'x':
      dah(); di(); di(); dah();
      break;
    case 'y':
      dah(); di(); dah(); dah();
      break;
    case 'z':
      dah(); dah(); di(); di();
      break;
    case '1':
      di(); dah(); dah(); dah(); dah();
      break;
    case '2':
      di(); di(); dah(); dah(); dah();
      break;
    case '3':
      di(); di(); di(); dah(); dah();
      break;
    case '4':
      di(); di(); di(); di(); dah();
      break;
    case '5':
      di(); di(); di(); di(); dit();
      break;
    case '6':
      dah(); di(); di(); di(); dit();
      break;
    case '7':
      dah(); dah(); di(); di(); dit();
      break;
    case '8':
      dah(); dah(); dah(); di(); dit();
      break;
    case '9':
      dah(); dah(); dah(); dah(); dit();
      break;
    case '0':
      dah(); dah(); dah(); dah(); dah();
      break;
    case '.':
      di(); dah(); di(); dah(); di(); dah();
      break;
    case ' ':
      pause(wordSpacing);
      break;
    default:
      pause(charSpacing);
  }
}
