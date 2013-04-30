#include <AFMotor.h>
#include <NewPing.h>

#define TRIGGER_PIN  14  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     15  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define MODERATE_SPEED 150
#define TURNING_SPEED 200
#define TOP_SPEED  255
#define MIN_SPEED  75

#define SONAR_NUM     3 // Number or sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

#define INVALID_DISTANCE -1

NewPing forwardSonar(14,15, MAX_DISTANCE); // Forward sonar
NewPing leftSonar(16,17, MAX_DISTANCE); // Left sonar
NewPing rightSonar(18,19, MAX_DISTANCE);  // Right sonar

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  forwardSonar, // Forward sonar
  leftSonar, // Left sonar
  rightSonar  // Right sonar
};

AF_DCMotor leftMotor(3);
AF_DCMotor rightMotor(4);

void setup()
{
  Serial.begin(9600);
  pingTimer[0] = millis() + 75;           // First ping starts 
  for (uint8_t i = 1; i < SONAR_NUM; i++) // Set the starting time for each sensor.
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
}

boolean movingBack = false;

void checkPingSensors(){
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = INVALID_DISTANCE;                      // Make distance INVALID in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }
}

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    Serial.print(i);
    Serial.print("=");
    Serial.print(cm[i]);
    Serial.print("cm ");
  }
  Serial.println();
}

int prevDistance = 0;
int currentSpeed = -1;

void loop()
{
  checkPingSensors();

  int forwardDistance = (cm[0] == INVALID_DISTANCE) ? prevDistance : cm[0];
  
 // int forwardDistance = forwardSonar.ping_cm();
   
  if(forwardDistance == 0) return;
  
  //Serial.print("Forward distance: ");
  //Serial.println(forwardDistance);

  // Move the Rover
  if(prevDistance == -1){
    prevDistance = forwardDistance;
  }  
  if(forwardDistance > 20){
    if(movingBack){
      brake();
      movingBack = false;
    }
    forward(forwardDistance);
    prevDistance = forwardDistance;
  } 
  else {
    if(!movingBack){
      movingBack = true;
      brake();
    }

    reverse();
    delay(500);
    brake();

    int leftDistance = cm[1];
    int rightDistance = cm[2];

    // randomly turn right or left
    if(leftDistance > rightDistance){
      turnLeft(); 
    } 
    else if (leftDistance < rightDistance){
      turnRight(); 
    } 
    else {
      randomTurn();
    }
    //    delay(500);

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
  if(currentDistance > 50){ // increase speed
    if(currentSpeed < MODERATE_SPEED){
      currentSpeed += 5;
    }
  } 
  else { // reduce speed
    if(currentSpeed > MIN_SPEED){
      currentSpeed -= 5;
    }
  }
  //currentSpeed = MODERATE_SPEED;
  
  Serial.print("Current speed: ");
  Serial.println(currentSpeed);

  leftMotor.setSpeed(currentSpeed);
  rightMotor.setSpeed(currentSpeed);
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD); 
}

void reverse(){
  //  Serial.println("Reverse...");
  leftMotor.setSpeed(MODERATE_SPEED);
  rightMotor.setSpeed(MODERATE_SPEED);
  leftMotor.run(BACKWARD);
  rightMotor.run(BACKWARD);
}

void brake(){
  leftMotor.setSpeed(0);
  rightMotor.setSpeed(0);
  leftMotor.run(BRAKE);
  rightMotor.run(BRAKE);
  currentSpeed = 0;
  delay(500);
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
  delay(500);
}













