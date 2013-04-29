#include <AFMotor.h>
#include <NewPing.h>

#define TRIGGER_PIN  14  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     15  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define MODERATE_SPEED 150
#define TURNING_SPEED 200
#define TOP_SPEED  255

#define SONAR_NUM     3 // Number or sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

#define INFINITE_DISTANCE 10000

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.



NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(14,15, MAX_DISTANCE), // Forward sonar
  NewPing(16,17, MAX_DISTANCE), // Left sonar
  NewPing(18,19, MAX_DISTANCE)  // Right sonar
  };

  AF_DCMotor leftMotor(3);
AF_DCMotor rightMotor(4);

void setup()
{
  Serial.begin(9600);
  pingTimer[0] = millis();           // First ping starts 
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
      cm[currentSensor] = INFINITE_DISTANCE;                      // Make distance infinite in case there's no ping echo for this sensor.
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

int prevDistance = -1;
int currentSpeed = -1;

void loop()
{
  checkPingSensors();

  int forwardDistance = (cm[0] == INFINITE_DISTANCE) ? sonar[0].ping_cm() : cm[0];
  int leftDistance = (cm[1] == INFINITE_DISTANCE) ? sonar[1].ping_cm() : cm[1];
  int rightDistance = (cm[2] == INFINITE_DISTANCE) ? sonar[2].ping_cm(): cm[2];
  
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
  if(currentDistance > 50){ // increase speed
    if(currentSpeed < MODERATE_SPEED){
      currentSpeed += 5;
    }
  } else { // reduce speed
    currentSpeed -= 5;
  }

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












