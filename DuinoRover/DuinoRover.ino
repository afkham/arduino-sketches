/*
Adafruit Arduino - Lesson 15. Bi-directional Motor
 */
int enablePin = 6;

int in1Pin = 2;
int in2Pin = 3;

int in3Pin = 4;
int in4Pin = 5;

int switchPin = 7;
int potPin = 0;
void setup()
{
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
 // pinMode(enablePin, OUTPUT);
 // pinMode(switchPin, INPUT_PULLUP);
}
void loop()
{
  //int speed = analogRead(potPin) / 4;
  //boolean reverse = digitalRead(switchPin);
  setMotor1(1023, true);
  setMotor2(1023, true);
  delay(2000);
  setMotor1(1023, false);
  setMotor2(1023, false);
  delay(2000);
  setMotor1(0, false);
  setMotor2(0, false);
  delay(2000);
}

void setMotor1(int speed, boolean reverse)
{
  analogWrite(enablePin, speed);
  digitalWrite(in1Pin, ! reverse);
  digitalWrite(in2Pin, reverse);
}

void setMotor2(int speed, boolean reverse)
{
  analogWrite(enablePin, speed);
  digitalWrite(in3Pin, ! reverse);
  digitalWrite(in4Pin, reverse);
}

