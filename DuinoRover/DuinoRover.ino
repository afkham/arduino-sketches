/*
Arduino powered autonomous robot with bluetooth manual control mode in addition to the autonomous mode
 
 */

#include <AFMotor.h>
#include <Servo.h> 
#include <SoftwareSerial.h>
#include <NewPing.h>

#define LOG_ENABLED false

#define BT_TX_PIN 3
#define BT_RX_PIN 10

#define TRIGGER_PIN  14  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     15  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define MODERATE_SPEED 200
#define REVERSE_SPEED  200
#define TURNING_SPEED 255
#define TOP_SPEED  255
#define MIN_SPEED  125

#define SERVO_MID_POSITION 80
#define SERVO_LEFT_POSITION 140
#define SERVO_RIGHT_POSITION 20

NewPing forwardSonar(14,15, MAX_DISTANCE); // Forward sonar
NewPing leftSonar(16,17, MAX_DISTANCE); // Left sonar
NewPing rightSonar(18,19, MAX_DISTANCE);  // Right sonar

AF_DCMotor leftMotor(3);
AF_DCMotor rightMotor(4);
Servo myservo;  // create servo object to control a servo 

#define MOVING_AVG_SAMPLES 3
int forwardDistances[MOVING_AVG_SAMPLES] = {
  -1, -1, -1};
int forwardDistanceIndex = 0;

enum motion{
  stateForward,stateReverse,stateLeftTurn,stateRightTurn,stateStopped};
motion _motion;

enum ServoPosition{
  right, rightCenter, leftCenter, left};
ServoPosition servoCurrentPosition = right;

SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

unsigned int forwardFrontDistance = -1;
unsigned int forwardLeftDistance = -1;
unsigned int forwardRightDistance = -1;

void setup()
{
  if(LOG_ENABLED) Serial.begin(115200);
  BTSerial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  /*myservo.write(SERVO_MID_POSITION);  
   delay(500);
   forwardFrontDistance = getDistance(forwardSonar);
   
   myservo.write(SERVO_LEFT_POSITION);
   delay(500);
   forwardLeftDistance = getDistance(forwardSonar);
   
   myservo.write(SERVO_RIGHT_POSITION);
   delay(500);
   forwardRightDistance = getDistance(forwardSonar);
   
   myservo.write(SERVO_MID_POSITION);  
   delay(500);*/
  calculateForwardDistances();
  //forward();
  delay(1000);
  _motion = stateStopped;
}

void calculateForwardDistances(){
  myservo.write(SERVO_MID_POSITION);  
  delay(100);
  forwardFrontDistance = getDistance(forwardSonar);

  myservo.write(SERVO_LEFT_POSITION);
  delay(100);
  forwardLeftDistance = getDistance(forwardSonar);

  myservo.write(SERVO_RIGHT_POSITION);
  delay(100);
  forwardRightDistance = getDistance(forwardSonar);

  myservo.write(SERVO_MID_POSITION);  
  delay(50);
}

int prevDistance = 0;
int currentSpeed = 0;


long reverseStartTime = -1;
int reversingTime;

long turnStartTime = -1;
long turnTime;

long forwardDistanceStartTime = -1;

boolean autoMode = true;
void loop()
{
  char ch;
  if (BTSerial.available() > 0){
    ch = BTSerial.read();
    if(LOG_ENABLED){
      Serial.println(ch);
    }
    if(ch == 'a'){
      brake();
      _motion = stateStopped;
      autoMode = !autoMode; 
      if(LOG_ENABLED){
        Serial.print("Auto mode: ");
        Serial.println(autoMode);
      }
    }
  }
  if(autoMode){
    runAutoMode();
  } 
  else {
    runManualMode(ch);
  }
}

void runManualMode(char ch){
  switch (ch) {
  case 'U': // up
    if(LOG_ENABLED) Serial.println("Moving forward...");
    _motion = stateForward;
    forward();
    break;
  case 'D': // down
    if(LOG_ENABLED) Serial.println("Moving backward...");
    _motion = stateReverse;
    reverse();
    break;
  case 'L': // left
    if(LOG_ENABLED) Serial.println("Turning left...");
    _motion = stateLeftTurn;
    turnLeft();
    break;
  case 'R': // right
    if(LOG_ENABLED) Serial.println("Turning right...");
    _motion = stateRightTurn;
    turnRight();
    break;
  case 'C':
    if(LOG_ENABLED) Serial.println("Stopped");
    _motion = stateStopped;
    brake();
    break;
  default:
    break;
  }
}

float _forwardDistance;

boolean _stateChanged;

void runAutoMode(){    
  switch(_motion){
  case stateForward:
    { 
      /*forwardDistances[forwardDistanceIndex] = getDistance(forwardSonar);
       forwardDistanceIndex++;
       if(forwardDistanceIndex > MOVING_AVG_SAMPLES - 1){
       forwardDistanceIndex = 0;
       }
       float forwardDistance = getMovingAverage();*/

      //float forwardDistance;// = getDistance(forwardSonar);
      if(millis() - forwardDistanceStartTime >= 30){
        int servoPosition = changeServoPosition();
        myservo.write(servoPosition);
        delay(50);
        _forwardDistance = getDistance(forwardSonar);
        if(_forwardDistance == 0) {
          _forwardDistance = 20;
        }
        switch(servoCurrentPosition){
        case right:
          {
            forwardRightDistance = _forwardDistance;
            break;
          }
        case left:
          {
            forwardLeftDistance = _forwardDistance;
            break;
          }
        case leftCenter: 
        case rightCenter:
          {
            forwardFrontDistance = _forwardDistance;
          } 
        }

        forwardDistanceStartTime = millis();      
      }

      // Move the Rover
      /*if(prevDistance == -1){
       prevDistance = _forwardDistance;
       } */
      if(LOG_ENABLED){
            Serial.print("FFD: ");
            Serial.print(forwardFrontDistance);
            Serial.print(", FLD: ");
            Serial.print(forwardRightDistance);
            Serial.print(", FRD: ");
            Serial.println(forwardRightDistance);
      } 
      if(forwardFrontDistance > 10 && forwardRightDistance > 10 && forwardRightDistance > 10){
        //if(_forwardDistance > 10){
        _motion = stateForward;
        if(_stateChanged){
          if(LOG_ENABLED){
            Serial.print("Forward distance: ");
            Serial.println(_forwardDistance);
          }
          forward();
          _stateChanged = false;
        }
        //prevDistance = forwardDistance;
      } 
      else {
        brake();
        forwardRightDistance = forwardLeftDistance = forwardFrontDistance = 20;
        _motion = stateReverse;
        _stateChanged = true;
        reversingTime = random(600);
        reverseStartTime = millis();
      } 
      break;
    }
  case stateReverse: 
    {
      if(millis() - reverseStartTime < reversingTime){
        if(_stateChanged){
          if(LOG_ENABLED) Serial.println("Reversing...");
          reverse(); // continue reversing 
          _stateChanged = false;
        }
      } 
      else {  
        brake();
        turnTime = random(600);
        turnStartTime = millis();
        makeTurnDecision();
      }
      break;
    }
  case stateLeftTurn:
    {
      if(millis() - turnStartTime < turnTime){
        if(_stateChanged){
          if(LOG_ENABLED) Serial.println("Turning left...");
          turnLeft();
          _stateChanged = false;
        }
      } 
      else {
        _forwardDistance = getDistance(forwardSonar);
        if(_forwardDistance > 10){
          brake();
          _motion = stateForward;
          _stateChanged = true;
          forwardDistanceStartTime = millis();
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
      if(millis() - turnStartTime < turnTime){
        if(_stateChanged){
          if(LOG_ENABLED) Serial.println("Turning right...");
          turnRight();
          _stateChanged = false;
        }
      } 
      else {
        _forwardDistance = getDistance(forwardSonar);
        if(_forwardDistance > 10){
          brake();
          _motion = stateForward;
          _stateChanged = true;
          forwardDistanceStartTime = millis();
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
      if(LOG_ENABLED) Serial.println("Stopped");
      _motion = stateForward;
      _stateChanged = true;
      _forwardDistance = forwardSonar.ping_median(10)/US_ROUNDTRIP_CM;
      forwardDistanceStartTime = millis();
      break;
    }
  default:
    break;
  }
}

float getMovingAverage(){
  float total;
  int ommittedPositions = 0;
  for(int i = 0; i < MOVING_AVG_SAMPLES; i++){
    if(forwardDistances[i] != -1){
      total += forwardDistances[i];
    } 
    else {
      ommittedPositions++;
    }
  }
  return total/(MOVING_AVG_SAMPLES-ommittedPositions);
}

void makeTurnDecision(){
  float leftDistance = getDistance(leftSonar);
  float rightDistance = getDistance(rightSonar);

  if(LOG_ENABLED){
    Serial.print("-------- Left distance:"); 
    Serial.println(leftDistance);
    Serial.print("++++++++ Right distance:"); 
    Serial.println(rightDistance);
  }

  leftDistance = (leftDistance != 0) ? leftDistance : 15;
  rightDistance = (rightDistance != 0) ? rightDistance : 15;

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
  _stateChanged = true;
}

float getDistance(NewPing &sonar){
  //return sonar.ping_median(3)/US_ROUNDTRIP_CM;
  return sonar.ping_cm();
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
  if(LOG_ENABLED){
    Serial.print("Current speed: ");
    Serial.println(currentSpeed);
  }

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
  //calculateForwardDistances();
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
  myservo.write(SERVO_MID_POSITION); 
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
}

/*
int getDistance(unsigned int position){
 myservo.write(position); 
 delay(500);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
 //  unsigned int uS = forwardSonar.ping(); // Send ping, get ping time in microseconds (uS).
 //  Serial.print(uS / US_ROUNDTRIP_CM); // Convert ping time to distance in cm and print result (0 = outside set distance range)
 //  Serial.println("cm");
 }
 */

int changeServoPosition(){
  switch(servoCurrentPosition){
  case right:
    {
      servoCurrentPosition = rightCenter;
      Serial.print("Right: ");
      return SERVO_RIGHT_POSITION;
    }
  case left:
    {
      servoCurrentPosition = leftCenter;
      Serial.print("Left: ");
      return SERVO_LEFT_POSITION;
    }
  case leftCenter:
    {
      servoCurrentPosition = right;
      Serial.print("Center: ");
      return SERVO_MID_POSITION;
    } 
  case rightCenter:
    {
      servoCurrentPosition = left;
      Serial.print("Center: ");
      return SERVO_MID_POSITION;

    } 
  }

}

























































