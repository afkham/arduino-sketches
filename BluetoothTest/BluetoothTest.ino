
#include <SoftwareSerial.h>

#define BT_TX_PIN 9
#define BT_RX_PIN 8



SoftwareSerial blueToothSerial(BT_RX_PIN, BT_TX_PIN);

void setup()
{
  pinMode(16,OUTPUT);
  pinMode(17,OUTPUT);
  digitalWrite(16,HIGH);
  digitalWrite(17,LOW);
  Serial.begin(9600);
  setupBlueToothConnection();
  Serial.println("Setup complete");
}


void loop()
{

  if (blueToothSerial.available() > 0){
    char c= blueToothSerial.read();
     Serial.println(c);
     if(c == 'U'){
        Serial.println("Forward..."); 
     }
  }

}

//The following code is necessary to setup the bluetooth shield ------copy and paste----------------
void setupBlueToothConnection()
{
 blueToothSerial.begin(9600); //Set BluetoothBee BaudRate to default baud rate 38400
 
}













