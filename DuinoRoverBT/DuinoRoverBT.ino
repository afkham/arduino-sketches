#include <AFMotor.h>
#include <SoftwareSerial.h>
#include <NewPing.h>

#define BT_TX_PIN 3
#define BT_RX_PIN 10

#define TRIGGER_PIN  14  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     15  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define MODERATE_SPEED 200
#define REVERSE_SPEED  125
#define TURNING_SPEED 225
#define TOP_SPEED  255
#define MIN_SPEED  150

#define SONAR_NUM     3 // Number or sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

#define INVALID_DISTANCE -1

NewPing forwardSonar(14,15, MAX_DISTANCE); // Forward sonar
NewPing leftSonar(16,17, MAX_DISTANCE); // Left sonar
NewPing rightSonar(18,19, MAX_DISTANCE);  // Right sonar

AF_DCMotor leftMotor(3);
AF_DCMotor rightMotor(4);


SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

void setup()
{
  Serial.begin(9600);
  BTSerial.begin(9600);
}

boolean movingBack = false;

int prevDistance = 0;
int currentSpeed = -1;

void loop()
{

  if (BTSerial.available() > 0){
    char ch = BTSerial.read();
    switch (ch) {
    case 'U': // up
      forward();
      break;
    case 'D': // down
      reverse();
      break;
    case 'L': // left
      turnLeft();
      break;
    case 'R': // right
      turnRight();
      break;
    case 'C':
      brake();
      break;
    default:
      break;
    }
  }
}

void randomTurn(){
  if (random(2) == 0) {
    turnLeft();
  } 
  else {
    turnRight(); 
  }
}

void forward(int currentDistance){
  // start slow and increase speed
  Serial.println("Forward...");
  if(currentSpeed == 0){
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
  }
  //currentSpeed = 80;
  Serial.print("Current speed: ");
  Serial.println(currentSpeed);

  leftMotor.setSpeed(currentSpeed);
  rightMotor.setSpeed(currentSpeed);
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD); 
}

void forward(){
  leftMotor.setSpeed(TOP_SPEED);
  rightMotor.setSpeed(TOP_SPEED);
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD); 
}

void reverse(){
  //  Serial.println("Reverse...");
  leftMotor.setSpeed(REVERSE_SPEED);
  rightMotor.setSpeed(REVERSE_SPEED);
  leftMotor.run(BACKWARD);
  rightMotor.run(BACKWARD);
  delay(random(500));
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

void turn(AF_DCMotor &leftMotor, AF_DCMotor &rightMotor){
  leftMotor.setSpeed(TURNING_SPEED);
  rightMotor.setSpeed(TURNING_SPEED - 50);
  leftMotor.run(FORWARD);
  rightMotor.run(BACKWARD);
  delay(random(600));
}















