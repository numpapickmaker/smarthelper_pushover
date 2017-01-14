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
unsigned long timerBat, preTimeBat, preTime2Bat , timeOutBat;
float sensorValue = 0;  // variable to store the value coming from the sensor
boolean toggleLEDBat,toggleBat;
void toggle_battery_led (int toggle_time , int pin){
   if(toggleLEDBat == true){
   //Serial.println("toggleLED true");
   timerBat = millis();
   if(timerBat - preTime2Bat > toggle_time){
    toggleBat = !toggleBat;
    Serial.println(toggleBat);
    digitalWrite(pin,toggleBat);
    preTime2Bat = timerBat;
   }
   }
}

void read_battery_milsec(unsigned long t){
  timerBat = millis();
  if (timerBat - preTimeBat > t) {
    sensorValue = analogRead(A0);
    if (sensorValue < 855 ) {//น้อยกว่า3.5V จะเตือน//837
      Serial.print("case1 : ");
      Serial.println(sensorValue);//*3.3/1024
      toggleLEDBat = true;
      preTimeBat = timerBat;
      //int x = (sensorValue*4.3)/1024;
    } else {//
      Serial.print("case2 : ");
      Serial.println(sensorValue);//*3.3/1024
      toggleLEDBat = false;
      preTimeBat = timerBat;
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
  Serial.println("test");
  // read the value from the sensor:
  read_battery_milsec(5000);
  toggle_battery_led(1000,13);
  

}


