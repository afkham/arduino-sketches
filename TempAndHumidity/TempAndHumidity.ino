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
#include <dht11.h>

dht11 DHT11;

#define DHT11PIN 2

#define API_KEY "AHgL53Kmz1AM8YNAKW1wO4iBgoySAKw2MFpCS1ovV0NqYz0g" // your Cosm API key
#define FEED_ID 122936 // your Cosm feed ID

// MAC address for your Ethernet shield
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Analog pin which we're monitoring (0 and 1 are used by the Ethernet shield)
int sensorPin = 2;

unsigned long lastConnectionTime = 0;                // last time we connected to Cosm
const unsigned long connectionInterval = 15000;      // delay between connecting to Cosm in milliseconds

// Initialize the Cosm library

// Define the string for our datastream ID
char tempSensorId[] = "temperature";
char humiditySensorId[] = "humidity";

CosmDatastream datastreams[] = {
  CosmDatastream(tempSensorId, strlen(tempSensorId), DATASTREAM_FLOAT),
  CosmDatastream(humiditySensorId, strlen(humiditySensorId), DATASTREAM_FLOAT)
  };

  // Wrap the datastream into a feed
  CosmFeed feed(FEED_ID, datastreams, 2 /* number of datastreams */);

EthernetClient client;
CosmClient cosmclient(client);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Cosm Temperature & Humidity");
  Serial.println("==========================");

  Serial.println("Initializing network");
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
    readSensor();
    float temperature = (float)DHT11.temperature;
    float humidity = (float)DHT11.humidity;

    // send it to Cosm
    sendData(temperature, humidity);
    // read the datastream back from Cosm
    getData();
    // update connection time so we wait before connecting again
    lastConnectionTime = millis();
  }
}

// send the supplied value to Cosm, printing some debug information as we go
void sendData(float tempSensorValue, float humiditySensorValue) {
  datastreams[0].setFloat(tempSensorValue);
  datastreams[1].setFloat(humiditySensorValue);

  Serial.print("Read sensor values. Temp: ");
  Serial.print(datastreams[0].getFloat());
  Serial.print("oC, Humidity: ");
  Serial.print(datastreams[1].getFloat());
  Serial.println("%");

  Serial.println("Uploading to Cosm");
  int ret = cosmclient.put(feed, API_KEY);
  Serial.print("PUT return code: ");
  Serial.println(ret);

  Serial.println();
}

void readSensor(){
  int chk = DHT11.read(DHT11PIN);

  Serial.print("Read sensor: ");
  switch (chk)
  {
  case DHTLIB_OK: 
    Serial.println("OK"); 
    break;
  case DHTLIB_ERROR_CHECKSUM: 
    Serial.println("Checksum error"); 
    break;
  case DHTLIB_ERROR_TIMEOUT: 
    Serial.println("Time out error"); 
    break;
  default: 
    Serial.println("Unknown error"); 
    break;
  }
}

// get the value of the datastream from Cosm, printing out the value we received
void getData() {
  Serial.println("Reading data from Cosm");

  int ret = cosmclient.get(feed, API_KEY);
  Serial.print("GET return code: ");
  Serial.println(ret);

  if (ret > 0) {
    Serial.print("Datastream is: ");
    Serial.print(feed[0]);
    Serial.print(", ");
    Serial.println(feed[1]);

    Serial.print("Read value is: ");
    Serial.print(feed[0].getFloat());
    Serial.print(", ");
    Serial.println(feed[1].getFloat());
  }

  Serial.println();
}





