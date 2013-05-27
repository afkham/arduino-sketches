#include <Bounce.h>
#include <pitches.h>

const int signal = 9;             
const int led = 13;
const int sw = 10; // on/off switchitch
const int onLED = 11;
const int offLED = 12;
const int tonePin = 8;
const int relay = 7;

boolean initialized = false;
int state = HIGH; // state of the shield, on or off

// Instantiate a Debounce object with a 20 millisecond debounce time
Bounce bouncer = Bounce(sw, 20); 

void setup()
{
  // initialize the serial communications:
  Serial.begin(9600);

  pinMode(signal, INPUT);
  pinMode(sw, INPUT);
  pinMode(led, OUTPUT);
  pinMode(onLED, OUTPUT);
  pinMode(offLED, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(led, LOW);
}

int noteIndex = 0;
const int noteCount = 2;
//int notes[noteCount] = {NOTE_F7,NOTE_FS7, NOTE_A7, NOTE_AS7};
int notes[noteCount] = {
  NOTE_A7, NOTE_AS7};

void loop(){
  if(initialized){
    if ( bouncer.update() ) {
      if ( bouncer.read() == HIGH) {
        if ( state == LOW ) {
          state = HIGH;
        } 
        else {
          state = LOW;
          digitalWrite(led, LOW);
          noTone(tonePin);
        }
        digitalWrite(onLED, state);
        digitalWrite(offLED, 1-state);
      }
    }
    if(state == LOW){
      return; 
    }
    int val = digitalRead(signal);
    if(val == 1){
      if(playTone(notes[noteIndex], 10)){
        noteIndex++;
        if(noteIndex >= noteCount){
          noteIndex=0; 
        }
      }
      digitalWrite(led, HIGH);
      digitalWrite(relay, LOW);
    } 
    else {
      digitalWrite(led, LOW);
      digitalWrite(relay, HIGH);
      noTone(tonePin);
    }    
  } 
  else {
    initialized = true;  
    digitalWrite(onLED, HIGH);
    digitalWrite(offLED, LOW);
    noTone(tonePin);
    digitalWrite(led, LOW);
    digitalWrite(relay, LOW);
  }
}

long noteStartTime = -1;

boolean playTone(int note, int noteDuration){
  // to calculate the note duration, take one second 
  // divided by the note type.
  //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

  int duration = 1000/noteDuration;
  tone(tonePin, note,duration);

  // to distinguish the notes, set a minimum time between them.
  // the note's duration + 30% seems to work well:
  if(noteStartTime == -1){
    noteStartTime = millis();
    return false;
  } 
  else if(noteStartTime !=-1 && millis() - noteStartTime >= duration * 1.3){
    noTone(tonePin); 
    noteStartTime = -1;
  } 
  else {
    Serial.println(false);
    return false;
  }
  return true;
}












