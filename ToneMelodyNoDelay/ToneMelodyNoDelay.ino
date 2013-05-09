/*
  Melody
 
 Plays a melody 
 
 circuit:
 * 8-ohm speaker on digital pin 8
 
 created 21 Jan 2010
 modified 30 Aug 2011
 by Tom Igoe 
 
 This example code is in the public domain.
 
 http://arduino.cc/en/Tutorial/Tone
 
 */
#include "pitches.h"

// notes in the melody:
int noteCount = 8;
int noteIndex =0;
int alarmTonePin = 14;
long noteStartTime = -1;
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };

void setup() {
  // iterate over the notes of the melody:

}

void loop() {
  if(playTone(melody[noteIndex],noteDurations[noteIndex])){
    noteIndex++;
    if(noteIndex >= noteCount){
      noteIndex=0; 
    }
  }
}

boolean playTone(int note, int noteDuration){
  // to calculate the note duration, take one second 
  // divided by the note type.
  //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

  int duration = 1000/noteDuration;
  tone(alarmTonePin, note,duration);

  // to distinguish the notes, set a minimum time between them.
  // the note's duration + 30% seems to work well:
  if(noteStartTime == -1){
    noteStartTime = millis();
    return false;
  } 
  else if(noteStartTime !=-1 && millis() - noteStartTime >= duration * 1.3){
    noTone(alarmTonePin); 
    noteStartTime = -1;
  } 
  else {
    return false;
  }
  return true;

}

