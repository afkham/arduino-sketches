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

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(14,15, MAX_DISTANCE), // Forward sonar
  NewPing(16,17, MAX_DISTANCE), // Left sonar
  NewPing(18,19, MAX_DISTANCE)  // Right sonar
};

AF_DCMotor Motor1(3);
AF_DCMotor Motor2(4);

//NewPing fwdSonar(14, 15, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
//NewPing leftSonar(16, 17, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
//NewPing rightSonar(18, 19, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

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
      cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
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

void loop()
{
  checkPingSensors();
  
  // Move the Rover
  
  if(cm[0] > 20){
    if(movingBack){
      brake();
      movingBack = false;
    }
    forward();
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
    if (random(2) == 0) {
      turnLeft();
    } 
    else {
      turnRight(); 
    }
    //    delay(500);                                                                                                                                           ; o
  }
}

void forward(){
//  Serial.println("Forward...");
  Motor1.setSpeed(MODERATE_SPEED);
  Motor2.setSpeed(MODERATE_SPEED);
  Motor1.run(FORWARD);
  Motor2.run(FORWARD); 
}

void reverse(){
//  Serial.println("Reverse...");
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









