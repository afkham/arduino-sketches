#include <AFMotor.h>
#include <NewPing.h>

#define TRIGGER_PIN  14  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     15  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

AF_DCMotor Motor1(3);
AF_DCMotor Motor2(4);

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup()
{
  Serial.begin(9600);
}

boolean movingBack = false;

void loop()
{
    Serial.println("Forward...");
    Motor1.setSpeed(255);
    Motor2.setSpeed(255);
    Motor1.run(FORWARD);
    Motor2.run(FORWARD);
    delay(2000);
  
    brake();
//    
   Serial.println("Backward...");
    //Motor1.setSpeed(255);
//    //Motor2.setSpeed(255);
//    Motor1.run(BACKWARD);
//    Motor2.run(BACKWARD);
//    delay(2000);
//    
//    brake();
  
}

void brake(){
  Motor1.setSpeed(0);
  Motor2.setSpeed(0);
  Motor1.run(BRAKE);
  Motor2.run(BRAKE);
  delay(500);
}







