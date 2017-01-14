
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
    if (sensorValue < 837 ) {//น้อยกว่า3.5V จะเตือน//837
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
  pinMode(12, OUTPUT);
}

void loop() {
  Serial.println("test");
  // read the value from the sensor:
  read_battery_milsec(5000);//อ่านทุกๆ nวิ
  toggle_battery_led(1000,12);// led on pin12 battery
  

}


