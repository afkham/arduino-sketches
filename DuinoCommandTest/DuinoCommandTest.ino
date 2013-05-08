#include <AFMotor.h>

#define MODERATE_SPEED 100
#define TURNING_SPEED 200
#define TOP_SPEED  255

AF_DCMotor Motor1(3);
AF_DCMotor Motor2(4);

void setup()
{
  Serial.begin(9600);
}

void loop(){
  char c = Serial.read();
  if(c == 'f'){
    forward();
  } 
  else if (c == 'b') {
    reverse();
  } 
  else if (c == 'l'){
    turnLeft();
  } 
  else if (c == 'r'){
    turnRight();
  } 
  else if (c == 'x'){
    brake(); 
  } 
  else if (c == 'a'){
    autox(); 
  }
}

void forward(){
  Serial.println("Forward...");
  Motor1.setSpeed(MODERATE_SPEED);
  Motor2.setSpeed(MODERATE_SPEED);
  Motor1.run(FORWARD);
  Motor2.run(FORWARD); 
}

void reverse(){
  Motor1.setSpeed(MODERATE_SPEED);
  Motor2.setSpeed(MODERATE_SPEED);
  Motor1.run(BACKWARD);
  Motor2.run(BACKWARD);
}

void brake(){
  Motor1.setSpeed(0);
  Motor2.setSpeed(0);
  Motor1.run(BRAKE);
  Motor2.run(BRAKE);
  delay(500);
}

void turnLeft(){
  turn(Motor1, Motor2);
}

void turnRight(){
  turn(Motor2, Motor1);
}

void turn(AF_DCMotor &motor1, AF_DCMotor &motor2){
  motor1.setSpeed(TURNING_SPEED);
  motor2.setSpeed(TURNING_SPEED - 50);
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  delay(500);
}

void autox(){
  forward();
  delay(2000);
  brake();
  delay(1000);
  turnLeft();
  delay(2000);

  brake();
  delay(1000);
  forward();
  delay(2000);

  brake();
  delay(1000);
  turnRight();
  delay(2000);

  brake();
  delay(1000);
  reverse();
  delay(2000);
  brake();
}










