/*
  Analog Input
  Demonstr  turning on and off a light emitting diode(LED)  connected to digital pin 13.
  ates analog input by reading an analog sensor on analog pin 0 and
  The amount of time the LED will be on and off depends on
  the value obtained by analogRead().

  The circuit:
   Potentiometer attached to analog input 0
   center pin of the potentiometer to the analog pin
   one side pin (either one) to ground
   the other side pin to +5V
   LED anode (long leg) attached to digital output 13
   LED cathode (short leg) attached to ground

   Note: because most Arduinos have a built-in LED attached
  to pin 13 on the board, the LED is optional.


  Created by David Cuartielles
  modified 30 Aug 2011
  By Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInput

*/
// select the input pin for the potentiometer
// select the pin for the LED
unsigned long timer, preTime, preTime2 , timeOut;
float sensorValue = 0;  // variable to store the value coming from the sensor
boolean toggleLED,toggle;
void toggle_LED (int toggle_time , int pin){
   if(toggleLED == true){
   //Serial.println("toggleLED true");
   timer = millis();
   if(timer - preTime2 > toggle_time){
    toggle = !toggle;
    Serial.println(toggle);
    digitalWrite(pin,toggle);
    preTime2 = timer;
   }
   }
}

void read_battery(){
  timer = millis();
  if (timer - preTime > 5000) {
    sensorValue = analogRead(A0);
    if (sensorValue > 837 ) {
      Serial.print("case1 : ");
      Serial.println(sensorValue);//*3.3/1024
      toggleLED = true;
      preTime = timer;
      //int x = (sensorValue*4.3)/1024;
    } else {
      Serial.print("case2 : ");
      Serial.println(sensorValue);//*3.3/1024
      toggleLED = false;
      preTime = timer;
    }
  }
}
void setup() {
  // declare the ledPin as an OUTPUT:
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  
  // read the value from the sensor:
  read_battery();
  toggle_LED(500,13);
  

}


