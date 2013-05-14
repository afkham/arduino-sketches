#include <AFMotor.h>
#include <SoftwareSerial.h>
#include <NewPing.h>

#define BT_TX_PIN 3
#define BT_RX_PIN 10

#define TRIGGER_PIN  14  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     15  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define MODERATE_SPEED 200
#define REVERSE_SPEED  200
#define TURNING_SPEED 255
#define TOP_SPEED  255
#define MIN_SPEED  125

NewPing forwardSonar(14,15, MAX_DISTANCE); // Forward sonar
NewPing leftSonar(16,17, MAX_DISTANCE); // Left sonar
NewPing rightSonar(18,19, MAX_DISTANCE);  // Right sonar

AF_DCMotor leftMotor(3);
AF_DCMotor rightMotor(4);

enum motion{
  stateForward,stateReverse,stateLeftTurn,stateRightTurn,stateStopped};
motion _motion;

SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

void setup()
{
  Serial.begin(115200);
  BTSerial.begin(9600);
  forward();
  delay(100);
  _motion = stateStopped;
}


int prevDistance = 0;
int currentSpeed = 0;


long reverseStartTime = -1;
int reversingTime;

long turnStartTime = -1;
long turnTime;

boolean autoMode = true;
void loop()
{
  if (BTSerial.available() > 0){
    char ch = BTSerial.read();
    if(ch == 'A'){
       autoMode = !autoMode; 
    }
  }
  if(autoMode){
    runAutoMode();
  } else {
    runManualMode();
  }
}

void runManualMode(){
  
}

void runAutoMode(){
    switch(_motion){
  case stateForward:
    { 
      float forwardDistance = getDistance(forwardSonar);
      if(forwardDistance == 0) {
        forwardDistance = 20;
      }

      Serial.print("Forward distance: ");
      Serial.println(forwardDistance);

      // Move the Rover
      if(prevDistance == -1){
        prevDistance = forwardDistance;
      }  
      if(forwardDistance > 15){
        _motion = stateForward;
        forward(forwardDistance);
        prevDistance = forwardDistance;
      } 
      else {
        _motion = stateReverse;
        reversingTime = random(600);
        reverseStartTime = millis();
      } 
      break;
    }
  case stateReverse: 
    {
      Serial.println("Reversing...");
      if(millis() - reverseStartTime < reversingTime){
        reverse();
        break; // continue reversing
      } 
      else {  
        turnTime = random(600);
        turnStartTime = millis();
        makeTurnDecision();
      }
      break;
    }
  case stateLeftTurn:
    {
      Serial.println("Turning left...");
      if(millis() - turnStartTime < turnTime){
        turnLeft();
      } 
      else {
        float forwardDistance = getDistance(forwardSonar);
        if(forwardDistance > 15){
          _motion = stateForward;
        } 
        else {
          turnTime = random(500);
          turnStartTime = millis();
        }
      }
      break;
    }
  case stateRightTurn:
    {
      Serial.println("Turning right...");
      if(millis() - turnStartTime < turnTime){
        turnRight();
      } 
      else {
        float forwardDistance = getDistance(forwardSonar);
        if(forwardDistance > 15){
          _motion = stateForward;
        } 
        else {
          turnTime = random(500);
          turnStartTime = millis();
        }
      }
      break;
    }
  case stateStopped: 
    {
      Serial.println("Stopped");
      _motion = stateForward;
      break;
    }
  default:
    break;
  }
}

void makeTurnDecision(){
  float leftDistance = getDistance(leftSonar);
  float rightDistance = getDistance(rightSonar);
  // Decide whether to turn right or left
  if(leftDistance > rightDistance){
    _motion = stateLeftTurn;
  } 
  else if (leftDistance < rightDistance){
    _motion = stateRightTurn;
  } 
  else { // Random turn
    if (random(2) == 0) {
      _motion = stateLeftTurn;
    } 
    else {
      _motion = stateRightTurn;
    }
  }
}

float getDistance(NewPing &sonar){
  return sonar.ping_median(5)/US_ROUNDTRIP_CM;
  //return sonar.ping_cm();
}

void randomTurn(){
  if (random(2) == 0) {
    turnLeft();
  } 
  else {
    turnRight(); 
  }
}

void forward(){
  leftMotor.setSpeed(TOP_SPEED);
  rightMotor.setSpeed(TOP_SPEED);
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD); 
}

void forward(int currentDistance){
  // start slow and increase speed
  Serial.println("Forward...");
  /*if(currentSpeed == 0){
   currentSpeed = MIN_SPEED; 
   }
   if(currentDistance > 40){ // increase speed
   if(currentSpeed < MODERATE_SPEED){
   if(currentSpeed < MIN_SPEED + 10){
   currentSpeed += 5; 
   } 
   else {
   currentSpeed += 2;
   }
   }
   // currentSpeed = MODERATE_SPEED;
   } 
   else { // reduce speed
   if(currentSpeed > MIN_SPEED){
   currentSpeed -= 10;
   }
   }*/

  // TODO: temporarilly setting to max speed
  currentSpeed = TOP_SPEED;
  Serial.print("Current speed: ");
  Serial.println(currentSpeed);

  leftMotor.setSpeed(currentSpeed);
  rightMotor.setSpeed(currentSpeed);
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD); 
}

void reverse(){
  leftMotor.setSpeed(REVERSE_SPEED);
  rightMotor.setSpeed(REVERSE_SPEED);
  leftMotor.run(BACKWARD);
  rightMotor.run(BACKWARD);
}

void brake(){
  leftMotor.setSpeed(0);
  rightMotor.setSpeed(0);
  leftMotor.run(BRAKE);
  rightMotor.run(BRAKE);
  currentSpeed = 0;
}

void turnLeft(){
  turn(leftMotor, rightMotor);
}

void turnRight(){
  turn(rightMotor, leftMotor);
}

void turn(AF_DCMotor &motor1, AF_DCMotor &motor2){
  motor1.setSpeed(TURNING_SPEED);
  motor2.setSpeed(TURNING_SPEED - 50);
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
}




































