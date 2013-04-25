/*
Adafruit Arduino - Lesson 13. DC Motor
 */
int motorPin = 3;
void setup()
{
  pinMode(motorPin, OUTPUT);
  Serial.begin(9600);
  while (! Serial);
  Serial.println("Speed 0 to 255");
}
void loop()
{
  analogWrite(motorPin, 0);
  delay(2000);
  analogWrite(motorPin, 50);
  delay(2000);
  analogWrite(motorPin, 100);
  delay(2000);
  analogWrite(motorPin, 150);
  delay(2000);
  analogWrite(motorPin, 200);
  delay(2000);
  analogWrite(motorPin, 250);
  delay(2000);
  
  /*if (Serial.available())
  {
    int speed = Serial.parseInt();
    if (speed >= 0 && speed <= 255)
    {
      analogWrite(motorPin, speed);
    }
  }*/
}

