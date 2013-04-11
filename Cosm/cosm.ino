 /**
 * Cosm Arduino sensor client example.
 *
 * This sketch demonstrates connecting an Arduino to Cosm (https://cosm.com),
 * using the new Arduino library to send and receive data.
 *
 * Requirements
 *   * Arduino with Ethernet shield or Arduino Ethernet (board must use the
 *     Wiznet Ethernet chipset)
 *   * Arduino software with version >= 1.0
 *   * An account at Cosm (https://cosm.com)
 *
 * Optional
 *   * An analog sensor connected to pin 2 (note we can still read a value from
 *     the pin without this)
 *
 * Created 8th January, 2013 using code written by Adrian McEwen with
 * modifications by Sam Mulube
 *
 * Full tutorial available here: https://cosm.com/docs/quickstart/arduino.html
 *
 * This code is in the public domain.
 */

#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Cosm.h>

#define API_KEY "T_ZzxAQnRcJjJDKoMVE0FR4BnQySAKxiU0x3djFYWHBQcz0g" // your Cosm API key
#define FEED_ID 119876 // your Cosm feed ID

// MAC address for your Ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Analog pin which we're monitoring (0 and 1 are used by the Ethernet shield)
int lightSensorPin = 2;
int soundSensorPin = 3;

unsigned long lastConnectionTime = 0;                // last time we connected to Cosm
const unsigned long connectionInterval = 15000;      // delay between connecting to Cosm in milliseconds

// Initialize the Cosm library

// Define the string for our datastream ID
char sensorId[] = "sensor_reading";
char soundId[] = "sound";

CosmDatastream datastreams[] = {
  CosmDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
  CosmDatastream(soundId, strlen(soundId), DATASTREAM_FLOAT)
};

// Wrap the datastream into a feed
CosmFeed feed(FEED_ID, datastreams, 2 /* number of datastreams */);

EthernetClient client;
CosmClient cosmclient(client);
IPAddress ip(192,168,1, 177);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Cosm Sensor Client Example");
  Serial.println("==========================");

  Serial.println("Initializing network");
  //Ethernet.begin(mac, ip);
  while (Ethernet.begin(mac) != 1) {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }

  Serial.println("Network initialized");
  Serial.println();
}

void loop() {
  // main program loop
  if (millis() - lastConnectionTime > connectionInterval) {
    // read a value from the pin
    int lightSensorValue = analogRead(lightSensorPin);
    int soundSensorValue = analogRead(soundSensorPin);
    
    //Serial.print(sensorValue);
    
    // send it to Cosm
    sendData(lightSensorValue, soundSensorValue);
    // read the datastream back from Cosm
    getData();
    // update connection time so we wait before connecting again
    lastConnectionTime = millis();
  }
}

// send the supplied value to Cosm, printing some debug information as we go
void sendData(int lightSensorValue, int soundSensorValue) {
  datastreams[0].setFloat(lightSensorValue);
  datastreams[1].setFloat(soundSensorValue);

  //Serial.print("Read sensor value ");
  //Serial.println(datastreams[0].getFloat());

  Serial.println("Uploading to Cosm");
  int ret = cosmclient.put(feed, API_KEY);
  Serial.print("PUT return code: ");
  Serial.println(ret);

  Serial.println();
}

// get the value of the datastream from Cosm, printing out the value we received
void getData() {
  Serial.println("Reading data from Cosm");

  int ret = cosmclient.get(feed, API_KEY);
  Serial.print("GET return code: ");
  Serial.println(ret);

  if (ret > 0) {
    Serial.print("Datastream is: ");
    Serial.println(feed[0]);
    Serial.println(feed[1]);

    Serial.print("Light Sensor value is: ");
    Serial.println(feed[0].getFloat());
    
    Serial.print("Sound Sensor value is: ");
    Serial.println(feed[1].getFloat());
  }

  Serial.println();
}

    
