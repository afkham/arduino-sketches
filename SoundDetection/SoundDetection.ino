//
//
//
int mic=A0;
float sensorvalue=0,lastsensorvalue=0,lastminsensorvalue=1024;
int i;
int led[]={11,12,7,6,5,4};
float val;
//
//
//
void setup() 
{ 
//sg90.attach(9);
Serial.begin(9600);
for (int pin=0; pin < 6; pin++)  
{
  pinMode(led[pin], OUTPUT);      
}
for (int colu=2; colu < 6; colu++)  
{
  digitalWrite(led[colu], HIGH);      
}
//
// line 1 and 2 UP
//
digitalWrite(led[0], HIGH);
digitalWrite(led[1], HIGH);
//
//
//
} 
//
//
//
void loop() 
{
// val = sensibility
// this variable can be changed to potentiometer input
//
val=130;
sensorvalue=analogRead(mic);
sensorvalue=sensorvalue/val;
//
// Get MAX
//
if (sensorvalue > lastsensorvalue)
{
  lastsensorvalue=sensorvalue;
}
//
// Get MIN
//
if( sensorvalue < lastminsensorvalue)
{
  lastminsensorvalue=sensorvalue;
}
//
//
//
Serial.print("MAX: ");
Serial.print(lastsensorvalue);
Serial.print("  ");
Serial.print("LAST: ");
Serial.print(sensorvalue);
Serial.print("  ");
Serial.print("MIN: ");
Serial.print("  ");
Serial.print(lastminsensorvalue);
Serial.print("Sensibility: ");
Serial.println(val);
delay(1);
//
// Clear led matrix
//
if (sensorvalue == lastminsensorvalue)
{
  for(int i=6;i>1;i--)
  {
    digitalWrite(led[i], HIGH);
    delay(130);
  }

}
//
//
//
else
{
  for (i=2;i<sensorvalue; i++) // turn on the leds up to the volume level
    {
      digitalWrite(led[i], LOW);
      delay(1);
    }
  for(i=i;i<6;i++)  // turn off the leds above the voltage level
    {
      digitalWrite(led[i], HIGH);
    }
}
}
