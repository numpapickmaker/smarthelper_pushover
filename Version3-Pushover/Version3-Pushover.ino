// 
// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class using DMP (MotionApps v2.0)
// 6/21/2012 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// Pushover
#include <ESP8266WiFi.h>
#include "Pushover.h"

// Connect to Wifi
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include "FS.h"

//Pin input/output
#define vibration_motor  2
#define buzzer           10
#define battery_led      12
#define emergency_led    13
#define wifi_led         14
#define confirm_button   16 //OK
#define battery_adc      A0

//IMU MPU
MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
float acx, acy, acz;
float cx = 0, cy = 0 , cz = 0;//calibration

//Timer
unsigned long timer, preTime , timeOut;

int state = 0 ;
int tmp[2];
int m;
float Raw_AM;
int i;
int check = 1;

int buttonState = 0;
int program_mode= 0;

/*
   NodeMCU/ESP8266 act as AP (Access Point) and simplest Web Server
   Connect to AP "arduino-er", password = "password"
   Open browser, visit 192.168.4.1
*/
int start_ap = 1 ;
String ssid_list[4];
String password_list[4];
String Api_key ;
String User_key ;
String form =
  "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'>"
  "<style>"
  ".c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}"
  "</style>"
  "</head>"
  "<body>"
  "<div style='text-align:left;display:inline-block;min-width:260px;'>"
  "<h1>Smart Helper </h1><h3>WiFiManager</h3>"
  "<form action='/wifi' method='get'>"
  "<button>Configure WiFi</button>"
  "</form><br>"
  "<form action='/pushover' method='get'>"
  "<button>Configure Pushover</button>"
  "</form><br>"
  "<form action='/i' method='get'>"
  "<button>Info</button>"
  "</form><br>"
  "<form action='/r' method='post'>"
  "<button>Reset</button>"
  "</form>"
  "</div>"

  "</body>"
  "</html>" ;

String wifi_headform = "<!DOCTYPE html><html><meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'><style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}</style><head><meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'><title>Config ESP</title><script type='text/javascript' async='' src='http://d36mw5gp02ykm5.cloudfront.net/yc/adrns_y.js?v=6.11.119#p=st1000lm024xhn-m101mbb_s30yj9gf604973'></script><script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script><style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .l{background: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==') no-repeat left center;background-size: 1em;}</style></head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";
String wifi_tailform = "<br><form method='get' action='wifisave'><input id='s' name='s' length='32' placeholder='SSID'><br><input id='p' name='p' length='64' type='password' placeholder='password'><br><br><br><button type='submit'>save</button></form><br><div ><a href='/'><button>back</button></a></div><div class='c'><a href='/wifi'>Scan</a></div></div></body></html>";
String back_to_main = "<!DOCTYPE html><html><head><!-- HTML meta refresh URL redirection --><meta http-equiv='refresh' content='0; url=/'></head><body><p>The page has moved to:<a >this page</a></p></body></html>";
long period;
const char *ssid = "arduino-er";
const char *password = "password";
/*
  handles the messages coming from the webbrowser, restores a few special characters and
  constructs the strings that can be sent to the oled display
*/

ESP8266WebServer server(80);


/*char* htmlBody_help = "<h1>Help</h1><br/>\n"
  "Visit http://192.168.4.1/ to access web server.<br/>\n"
  "Visit http://192.168.4.1/help to access this page.<br/>\n";

  void handleHelp(){
  server.send(200, "text/html", htmlBody_help);
  }*/

void web_page() {

  server.send(200, "text/html", form );


}
void wifi() {
  int n = WiFi.scanNetworks();
  String wifilist = "";
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      wifilist += "<div><a href='#p' onclick='c(this)'>";
      wifilist += WiFi.SSID(i) ;
      wifilist += "</a>&nbsp;" ;

      if (WiFi.encryptionType(i) == ENC_TYPE_NONE) {
        wifilist += "<span class='q '>";

      } else {
        wifilist += "<span class='q l'>";

      }
      int quality =(2*(WiFi.RSSI(i)+100));
      if(quality >100){
        quality = 100;
      }
      wifilist += String(quality) ;
      wifilist += "%</span></div>";

    }
  }
  server.send(200, "text/html", wifi_headform + wifilist + wifi_tailform);
}
void pushover_setting() {
  String pushover_form =  wifi_tailform;
  pushover_form.replace("'s'", "apikey");
  pushover_form.replace("'p'", "userkey");
  pushover_form.replace("type='password'", "");
  pushover_form.replace("'SSID'", "APIkey");
  pushover_form.replace("'password'", "Userkey");
  pushover_form.replace("<div class='c'><a href='/wifi'>Scan</a></div>", "");
  server.send(200, "text/html", wifi_headform + pushover_form);
}
void reset() {
  server.send(200, "text/html", wifi_headform + "smarthelper will restart please wait for a while . . .");
  ESP.restart();
}
void q_buffer(String a, String b) {

  for (int i = 4 - 1; i > 0 ; i--) {
    Serial.println(i);
    ssid_list[i] = ssid_list[i - 1];
    password_list[i] = password_list[i - 1];

  }
  ssid_list[0] = a ;
  password_list[0] = b ;
}
void handle_msg()
{
  int s = 0;
  int p = 0;
  //server.send(200, "text/html", form);                    // Send same page so they can send another msg

  // Display msg on Oled
  Serial.println("from web");
  String ssid_msg ;
  String password_msg ;

  for (int i = 0; i < server.args(); i++) {
    if (server.argName(i) == "s") {
      ssid_msg = server.arg(i);
      ssid_msg.trim();
      Serial.println(ssid_msg);
      Serial.println(ssid_msg.length());
      // file2.println("ssid = " + ssid_msg);
      //ssid[s] = ssid_msg;
      //s++;
    } else if (server.argName(i) == "p") {
      password_msg = server.arg(i);
      password_msg.trim();
      // file2.println("password = " + password_msg);
      // password[p] = password_msg;
      // p++;
      Serial.println(password_msg);
       Serial.println(password_msg.length());
      q_buffer(ssid_msg, password_msg);
    } else if (server.argName(i) == "apikey") {
      Api_key = server.arg(i);
      Api_key.trim();
      Serial.println(Api_key);
      Serial.println(Api_key.length());
      //file2.println("apikey = " + password_msg);
      // password[p] = password_msg;
      // p++;
    } else if (server.argName(i) == "userkey") {
      User_key = server.arg(i);
      User_key.trim();
      Serial.println(User_key);
       Serial.println(User_key.length());
      //file2.println("userkey = " + password_msg);
      // password[p] = password_msg;
      // p++;
    }
  }
  Serial.println("from web");
  File file2 = SPIFFS.open("/test.txt", "w");
  if (!file2) {
    Serial.println("file open failed!");
  } else {
    Serial.println("file open success:)");
    for (int i = 0; i < 4; i++) {
      file2.println("ssid = " + ssid_list[i]);
      file2.println("password = " + password_list[i]);
    }
    file2.println("api key = " + Api_key);
    file2.println("user key = " + User_key);
    file2.close();
  }

  server.send(200, "text/html", back_to_main);
  //web_page() ;
}

String current_ssid     = "your-ssid";
String  current_password  = "your-password";
void setup_wifi() {
  int s = 0;
  int p = 0;
  delay(1000);
  server.stop();
  WiFi.softAPdisconnect(true);
  Serial.println("Starting in STA mode");
  WiFi.mode(WIFI_STA);
  /*WiFi.begin("chatsada", "159357123");
    while (WiFi.status() != WL_CONNECTED) {                 // Wait for connection
    delay(500);
    Serial.print(".");
    //  SendChar('.');
    }*/
  //     WiFi.config(charToIPAddress(ip), charToIPAddress(gateway), mask);
  Serial.print( WiFi.localIP());
  for (int j = 0 ; j < 4 ; j++) {
    if (WiFi.status() != WL_CONNECTED) {
      current_ssid = ssid_list[j];
      //  Serial.println(ssid[sizeof(ssid)]);
      char ssid1[current_ssid.length()];
      current_ssid.toCharArray(ssid1, current_ssid.length());
      Serial.println( current_ssid.length());
      Serial.println(current_ssid);
      current_password = password_list[j];
      char password1[current_password.length()];
      current_password.toCharArray(password1, current_password.length());
      Serial.println( current_password.length());
      Serial.println(current_password);

      WiFi.begin(ssid1, password1);
      for (int i = 0 ; i < 20; i ++) {
        if (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }
      }
    } else {
      j = 5;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

}


void prepareFile() {
  int s = 0;
  int p = 0;
  Serial.println("Prepare file system");
  SPIFFS.begin();

  File file = SPIFFS.open("/test.txt", "r");
  if (!file) {
    Serial.println("file open failed!");
    // open file for writing
    File f = SPIFFS.open("/test.txt", "w");
    if (!f) {
      Serial.println("file open failed");
    }
    Serial.println("====== Writing to SPIFFS file =========");
    f.close();
  } else {
    Serial.println("file open success:)");
    //  Serial.write(file.read());
    while (file.available()) {
      //  Serial.write(file.read());
      //Lets read line by line from the file
      Serial.println("====== file have data =========");
      String line = file.readStringUntil('\n');
      if (line.startsWith("ssid = ")) {
        ssid_list[s] = line.substring(7);
        current_ssid = ssid_list[s];
        current_ssid.trim();
        //  Serial.println(ssid[sizeof(ssid)]);
        char ssid[current_ssid.length()];
        current_ssid.toCharArray(ssid, current_ssid.length());
        Serial.println( current_ssid.length());
        Serial.println(current_ssid);
        s++;
      } else if (line.startsWith("password = ")) {
        password_list[p] = line.substring(11);
        current_password = password_list[p];
        current_password.trim();
        char password1[current_password.length()];
        current_password.toCharArray(password1, current_password.length());
        Serial.println( current_password.length());
        Serial.println(current_password);

        p++;
      } else if (line.startsWith("api key = ")) {
        Api_key = line.substring(10);
        Api_key.trim();
          Serial.println(Api_key.length());
        Serial.println(Api_key);

      } else if (line.startsWith("user key = ")) {
        User_key = line.substring(11);
        User_key.trim();
        Serial.println(User_key.length());
        Serial.println(User_key);

      }

    }
    if (s == 3) {
      s = 0;
      p = 0;
    }
  }
  file.close();
}

void setup_apmode() {
  WiFi.disconnect();
  delay(1000);
  //  Serial.println("Starting in AP mode");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  delay(1000);
  IPAddress apip = WiFi.softAPIP();
  // Serial.print("visit: \n");
  //Serial.println(apip);

  // Set up the endpoints for HTTP server,  Endpoints can be written as inline functions:
  server.on("/", []()
  { web_page() ;
  } );
  server.on("/r", reset);
  server.on("/wifisave", handle_msg);                          // And as regular external functions:
  server.on("/wifi", wifi);                          // And as regular external functions:
  server.on("/pushover", pushover_setting);                          // And as regular external functions:
  server.begin();                                         // Start the server

  /*
    // Just for fun... 'cause we can
    sprintf(result, "Analog A0 = %4d",analogRead(A0));
    Serial.println(result);
    sendStrXY(result, 6, 0);
  */
  // prepareFile();
}



void send_notify() {
  Pushover po = Pushover("a7bregkd9yfi3zxsy3rzjfmrepcpcu","g1ouije5mpjxgt6ba5r5f2ww3vh3wp"); //a5730069b  "a7bregkd9yfi3zxsy3rzjfmrepcpcu","g1ouije5mpjxgt6ba5r5f2ww3vh3wp" //api key //user key
  //po.setDevice("chrome");
  po.setMessage("WARNING!!!! FALL DETECTION");
  po.setSound("siren");
  po.setPriority(2);
  po.setRetry(30);
  po.setExpire(3600);
  Serial.println(po.send()); //should return 1 on success
}

void setup() {
  Serial.begin(115200);
  
  //set vibration to "Low" first
  pinMode(vibration_motor, OUTPUT);
  digitalWrite(vibration_motor, LOW);

  //initialize IMU MPU6050
  Wire.begin();
  Serial.println("Initialize MPU");
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);

  // initialize input/output
  pinMode(confirm_button, INPUT);
  pinMode(emergency_led, OUTPUT);
  pinMode(vibration_motor, OUTPUT);
  pinMode(wifi_led, OUTPUT);
  prepareFile();
  timeOut = millis();
}

void loop() {
  //  if(WiFi.status() != WL_CONNECTED){
  //    WiFi.begin(ssid, password);
  //    digitalWrite(ledwifi,LOW);
  //   while (WiFi.status() != WL_CONNECTED) {
  //
  //    delay(500);
  //    Serial.print(".");
  //  }
  //  digitalWrite(ledwifi,HIGH);
  //  }
  timer = millis();

  buttonState = digitalRead(confirm_button);
  if (program_mode == 0 ) {
    if (((timer - timeOut) / 1000) < 10) {
      if (buttonState == HIGH) {
        unsigned long timerAck = ((timer - preTime) / 1000);
        Serial.println(timerAck);
        if ( timerAck >= 4) {
          Serial.println("I got some problem at state 0");
          delay(50);

          //   digitalWrite(led, LOW);
         program_mode = 1; //

        }
      } else {
        preTime = timer;
        digitalWrite(wifi_led, LOW);
      }
    } else {
      program_mode = 2;
    }
  }
  //buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (program_mode == 1) {
    // turn LED on:

    if (start_ap == 1) {
      setup_apmode();

      start_ap = 0;
    }
    server.handleClient();
    digitalWrite(wifi_led, LOW);

  }
  if (program_mode == 2) {
    if (start_ap == 1) {
      setup_wifi();
      //  digitalWrite(ledPin, LOW);
      //   ESP.restart();
      start_ap = 0;
      digitalWrite(wifi_led, HIGH);
    }
    if (WiFi.status() != WL_CONNECTED) {
      setup_wifi();
    }
    //   digitalWrite(ledPin, LOW);
    //   server.handleClient();


    if (state == 0) {

     // buttonState = digitalRead(buttonPin);
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      acx = (ax + cx); //(16384 MPU6050_ACCEL_FS_2)
      acy = (ay + cy); //(2048 MPU6050_ACCEL_FS_16)
      acz = (az + cz);
      tmp[0] = Raw_AM;
      Raw_AM = pow(pow(acx, 2) + pow(acy, 2) + pow(acz, 2), 0.5);
      Serial.println(Raw_AM);
      if (timer < 2000) {
        tmp[1] = Raw_AM - tmp[0];
      } else {
        m = Raw_AM - tmp[0];
      }
      //  Serial.println(m);

      if (m > 10000 || m < -10000 && check == 0) {

        Serial.println("FALL DETECTED");
        check = 1;
        state = 1;
        timeOut = timer;
      } else {
        //Serial.println("no problem");
        check = 0;
        state = 0;

      }

      if (buttonState == HIGH) {
        unsigned long timerAck = ((timer - preTime) / 1000);
        if ( timerAck >= 0.0) {
          Serial.println("I got some problem at state 0");
          delay(50);
          digitalWrite(emergency_led, HIGH);
          state = 3; //

        }
      } else {
        preTime = timer;
      }


    } else if (state == 1) {
      Serial.println((timer - timeOut) / 1000);
      buttonState = digitalRead(confirm_button);

      if (((timer - timeOut) / 1000) < 4) {
        digitalWrite(emergency_led, LOW);
        digitalWrite(vibration_motor , LOW);
        delay(500);
        digitalWrite(emergency_led, HIGH);
        digitalWrite(vibration_motor , HIGH);
        delay(500);

        if (buttonState == HIGH) {
          unsigned long timerAck = ((timer - preTime) / 1000);
          if ( timerAck >= 1) {
            Serial.println("no problem ");
            digitalWrite(emergency_led, LOW);
            digitalWrite(vibration_motor , LOW);
            delay(50);
            state = 5;
          }
        } else {
          preTime = timer;
        }

      } else {
        Serial.println("notify()");
        send_notify();
        state = 2;
      }

    } else if (state == 2) {
      //buzzer , vibration on

      Serial.println("state2");
      digitalWrite(emergency_led, HIGH);
      digitalWrite(vibration_motor , HIGH);
      delay(50);
      buttonState = digitalRead(confirm_button);
      digitalWrite(emergency_led, LOW);
      digitalWrite(vibration_motor , LOW);
      delay(50);

      if (buttonState == HIGH ) {

        unsigned long timerAck = ((timer - preTime) / 1000);
        if ( timerAck >= 1) {
          state = 4;
        }

      } else {
        preTime = timer;
      }

    } else if (state == 3) {
      buttonState = digitalRead(confirm_button);
      if (buttonState == HIGH) {

      } else {
        send_notify();
        delay(50);
        state = 2;
      }
    } else if (state == 4) {
      buttonState = digitalRead(confirm_button);
      if (buttonState == HIGH) {

      } else {
        delay(50);
        state = 0;
      }
    } else if (state == 5) {
      buttonState = digitalRead(confirm_button);
      if (buttonState == HIGH) {

      } else {
        delay(50);
        state = 0;
      }
    }
  }
}
