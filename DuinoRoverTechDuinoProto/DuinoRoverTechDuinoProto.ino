/*
Adafruit Arduino - Lesson 15. Bi-directional Motor
 */


int in1Pin = 2;
int in2Pin = 3;

int in3Pin = 4;
int in4Pin = 5;

int enable1Pin = 6;
int enable2Pin = 9;

void setup()
{
  Serial.begin(9600);
  Serial.println("Setup...");
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  pinMode(enable2Pin, OUTPUT);
  // pinMode(switchPin, INPUT_PULLUP);
}
void loop()
{
  Serial.println("Running...");
  reverse();
  halt();
  forward();
  halt();
  turnRight();
  halt();
  turnLeft();
  halt();
  forward();
  
}

void forward(){
  Serial.println("Forward");
  setMotor1(255, true);
  setMotor2(255, true);
  delay(5000);
}


void halt(){
  stopMotor1();
  stopMotor2();
  delay(500);
}

void reverse(){
  Serial.println("Reverse");
  setMotor1(255, false);
  setMotor2(255, false);
  delay(5000);
}

void turnLeft(){
  Serial.println("Left turn");
  setMotor1(255, true);
  setMotor2(255, false);
  delay(5000);
}

void turnRight(){
  Serial.println("Right turn");
  setMotor1(255, false);
  setMotor2(255, true);
  delay(5000);
}

void setMotor1(int speed, boolean reverse)
{
  analogWrite(enable1Pin, speed);
  digitalWrite(in1Pin, ! reverse);
  digitalWrite(in2Pin, reverse);
}

void setMotor2(int speed, boolean reverse)
{
  analogWrite(enable2Pin, speed);
  digitalWrite(in3Pin, ! reverse);
  digitalWrite(in4Pin, reverse);
}

void stopMotor2(){
  analogWrite(enable2Pin, 0);
  digitalWrite(in3Pin, false);
  digitalWrite(in4Pin, false);
}

void stopMotor1(){
  analogWrite(enable1Pin, 0);
  digitalWrite(in1Pin, false);
  digitalWrite(in2Pin, false);
}


