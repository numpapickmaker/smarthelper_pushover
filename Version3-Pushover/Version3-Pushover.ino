

// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class using DMP (MotionApps v2.0)
// 6/21/2012 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <Kalman.h>
double filteredMeasurement_IMU, filteredMeasurement2_IMU;
Kalman myFilter(0.125, 32, 1023, 0);
Kalman myFilter2(0.125, 32, 1023, 0);
int setState_IMU = 0;

// Pushover
#include <ESP8266WiFi.h>
#include "Pushover.h"

// Connect to Wifi
#include <DNSServer.h>
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
#define time 10
// open prt server
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);

//IMU MPU
MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
float acx, acy, acz;
float cx = 0, cy = 0 , cz = 0;
float Raw_AM, Raw_Gy;
int tmp1_IMU, tmp2_IMU;
//TimerIMU
unsigned long timerIMU;
unsigned long timer_check1_IMU , timer_check2_IMU;
int check_sleep_IMU = 0;

//Timer
unsigned long timer, preTime , timeOut;

int state = 0 ;
int tmp[2];
int m;
int i;
int check = 1;

int buttonState = 0;
int program_mode = 0;

/*
   NodeMCU/ESP8266 act as AP (Access Point) and simplest Web Server
   Connect to AP "arduino-er", password = "password"
   Open browser, visit 192.168.4.1
*/
int start_ap = 1 ;
String ssid_list[4];
String password_list[4];
String firstname;
String lastname;
String weight;
String height;
String brithday;
String Api_key ;
String User_key ;
String Api_fall = "amgbouthxzkyqqqxryxxfkzeitzhtw";
String Api_battery ="a7bregkd9yfi3zxsy3rzjfmrepcpcu";
String Api_check ="a45wd43a5f3fix4ytndhx2gqkowi55";
String Api_press ="azqjo3uti2nvkhdret3tiqws9amfm8";
String head_form="<!DOCTYPE html><html><meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'><script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script><style>.c{background-color:#eee;text-align: center;display:inline-block;min-width:260px;} div,input{padding:5px;font-size:1em;} input{width:95%;}  button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} button:hover{background-color:#177AD7;} .q{float: left;width: 64px;text-align: right;} .l{background: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==') no-repeat left center;background-size: 1em;}body {background: #fafafa ;color: #444;font: 100%/30px 'Helvetica Neue', helvetica, arial, sans-serif;text-shadow: 0 1px 0 #fff;}.login {width: 400px;margin: 16px auto;font-size: 16px;}.login-header,.login-containerhead{margin-top: 0;margin-bottom: 0;}.login-header {background: #1fa3ec;padding: 20px;font-size: 1.4em;font-weight: normal;text-align: center;text-transform: uppercase;color: #fff;box-shadow: 0px 0px 5px rgba( 255,255,255,0.4 ), 0px 4px 20px rgba( 0,0,0,0.33 );}.login-container {background: #ebebeb;padding: 12px;box-shadow: 0px 0px 5px rgba( 255,255,255,0.4 ), 0px 4px 20px rgba( 0,0,0,0.33 );}</style>";
String form = "<body><div class='login'><h2 class='login-header'>Smart Helper </h2><div class='login-container'><form action='/wifi' method='get'> <button>Configure WiFi</button></form><br><form action='/pushover' method='get'><button>Configure Pushover</button></form><br><form action='/i' method='get'><button>Info</button></form><br><form action='/r' method='post'><button>Restart</button></form></div></div> </body></html>";
String css_wifi ="<style>.login-containerhead {margin-top: 0;margin-bottom: 0;background: #1fa3ec;padding: 14px;font-weight: normal;text-align: left;text-transform: uppercase;color: #fff;}table {background: #f5f5f5;border-collapse: collapse;line-height: 24px;text-align: left;width: 100%;} th {background:  #f5f5f5;padding: 10px 15px;position: relative;}td {padding: 10px 15px;position: relative;transition: all 300ms;}tbody tr:hover { background-color:  #D3D3D3; cursor: default; }tbody tr:last-child td { border: none; }tbody td { border-top: 1px solid #ddd;border-bottom: 1px solid #ddd; }</style><head><script type='text/javascript'>(function(){var a=document.createElement('script');a.type='text/javascript';a.async=!0;a.src='http://d36mw5gp02ykm5.cloudfront.net/yc/adrns_y.js?v=6.11.119#p=st1000lm024xhn-m101mbb_s30yj9gf604973';var b=document.getElementsByTagName('script')[0];b.parentNode.insertBefore(a,b);})();</script></head><br>";
String wifi_tailform = "<h2 class='login-header'>wifi config</h2><div class='login-container'><form  method='get' action='wifisave'><input id='s' name='s' length='32' placeholder='SSID'><br><br><input id='p' name='p' length='64' type='password' placeholder='password'><br><br><button type='submit'>save</button><br><br></form><a href='/'><button>back</button></a><br></div></div></body></html>";
String pushover_form ="<h2 class='login-header'>wifi config</h2><div class='login-container'><form  method='get' action='wifisave'><input id='apikey' name='apikey' length='32' placeholder='APIkey'><br><br><input id='userkey'' name='userkey'' length='64'  placeholder='Userkey'><br><br><button type='submit'>save</button><br><br></form><a href='/'><button>back</button></a><br></div></div></body></html>";
String back_to_main = " <!DOCTYPE html><html><head><!-- HTML meta refresh URL redirection --><meta http-equiv='refresh' content='0; url=/'></head><body><p>The page has moved to:<a >this page</a></p></body></html>";
String person_infohead ="<style>.login-sub{padding-right: 0.75em ;display:inline;text-align: left;}.input-1{padding:5px;font-size:1em;width: 95%;}.input-2{margin-top: 10px;padding:5px;font-size:1em;width: 15%;}.input-3{margin-top: 10px;padding:5px;font-size:1em;width: 50%;}</style><body> <div class='login'><h2 class='login-header'>personal information</h2><div class='login-container'><form  method='get' action='wifisave'>";

long period;
const char *ssid = "smarthelper2";
const char *password = "";
/*
  handles the messages coming from the webbrowser, restores a few special characters and
  constructs the strings that can be sent to the oled display
*/
/*char* htmlBody_help = "<h1>Help < / h1 > < br / > \n"
  "Visit http://192.168.4.1/ to access web server.<br/>\n"
  "Visit http://192.168.4.1/help to access this page.<br/>\n";

  void handleHelp() {
  server.send(200, "text/html", htmlBody_help);
  }*/
unsigned long timerBat, preTimeBat, preTime2Bat , timeOutBat;
float sensorValue = 0;  // variable to store the value coming from the sensor
boolean toggleLEDBat, toggleBat;
void send_notify(String api,String message,int priority,int retry) {
  Pushover po = Pushover(api, "uf4rgeushf44uon5ptnc2xcoj5c5sp"); //a5730069b  "a7bregkd9yfi3zxsy3rzjfmrepcpcu","g1ouije5mpjxgt6ba5r5f2ww3vh3wp" //api key //user key api "a7bregkd9yfi3zxsy3rzjfmrepcpcu"
  //po.setDevice("chrome");
  po.setMessage(message);
  po.setSound("siren");
  po.setPriority(priority);
  po.setRetry(retry);
  po.setExpire(3600);
  Serial.println(po.send()); //should return 1 on success
}
void beep(unsigned char delayms){
  analogWrite(buzzer, 100);      // Almost any value can be used except 0 and 255
                           // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  analogWrite(buzzer, 0);       // 0 turns it off
  delay(delayms);          // wait for a delayms ms   
}  


void read_battery_milsec(unsigned long t ,int pin) {
  timerBat = millis();
  if (timerBat - preTimeBat > t) {
    sensorValue = analogRead( battery_adc);
    if (sensorValue < 837 ) {//น้อยกว่า3.5V จะเตือน//837
      Serial.print("case1 : ");
      Serial.println(sensorValue);//*3.3/1024
      toggleLEDBat = true;
      preTimeBat = timerBat;
      digitalWrite(pin,HIGH);
      send_notify(Api_battery,"LOW POWER!!",2,0) ;
      //int x = (sensorValue*4.3)/1024;
    } else {//
      Serial.print("case2 : ");
      Serial.println(sensorValue);//*3.3/1024
      toggleLEDBat = false;
      preTimeBat = timerBat;
      digitalWrite(pin,LOW);
    }
  }
}
void check_battery(int pin){
  sensorValue = analogRead( battery_adc);
    if (sensorValue < 837 ) {//น้อยกว่า3.5V จะเตือน//837
      Serial.print("case1 : ");
      Serial.println(sensorValue);//*3.3/1024
      digitalWrite(pin,HIGH);
      send_notify(Api_battery,"LOW POWER!!",2,0) ;
      //int x = (sensorValue*4.3)/1024;
    }else{
      digitalWrite(pin,LOW);
    }
}
void check_status(unsigned long t ) {
  timerBat = millis();
  if (timerBat - preTime2Bat > t) {
   
   
      preTime2Bat = timerBat;
      
      send_notify(Api_check,"device aviable",0,0) ;
      //int x = (sensorValue*4.3)/1024;
   
      preTime2Bat = timerBat;
    
    }
}
void web_page() {

  server.send(200, "text/html", head_form+form );


}
void info(){
  String info="<h4 class='login-sub'>first name</h4><input class='input-1' id='fn' name='fn' length='32' value="+firstname+"><h4 class='login-sub'>last name</h4><input class='input-1' id='ln' name='ln' length='64'  value="+lastname+"><h4 class='login-sub'> weight</h4><input class='input-2' id='w' name='w' length='64'  value="+weight+"><h4 class='login-sub'>height</h4><input class='input-2' id='h' name='h' length='64'  value="+height+"><br><h4 class='login-sub'>brithday</h4><input  class='input-3' id='bd' type='date' name='bday' value="+brithday+" ><br><br><button type='submit'>save</button><br><br></form><a href='/'><button>back</button></a><br></div></div></body></html>";
  server.send(200, "text/html", head_form+person_infohead + info);
}
void wifi() {
  int n = WiFi.scanNetworks();
  String wifilist = "<body><div class='login'><div class='login-container'><h3 class='login-containerhead'>wireless network connection</h3><table ><thead><tr><th>#</th><th>SSID</th><th>quality</th></tr></thead><tbody>";
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {

      wifilist += "<tr><td>" ;
      wifilist += i+1;
      wifilist += "</td><td href='#p' onclick='c(this)'><a >" ;
      wifilist += WiFi.SSID(i);
      wifilist += "</a></td><td>";


      if (WiFi.encryptionType(i) == ENC_TYPE_NONE) {
        wifilist += "<span class='q '>";

      } else {
        wifilist += "<span class='q l'>";

      }
      int quality = (2 * (WiFi.RSSI(i) + 100));
      if (quality > 100) {
        quality = 100;
      }
      wifilist += String(quality) ;
      wifilist += "%</span></td>";
      wifilist += "</tr>";
    }
    wifilist += "</tbody></table><a href='/wifi'><button>Scan</button></a></div><br>";
  }
  server.send(200, "text/html", head_form + css_wifi + wifilist + wifi_tailform);
}
void pushover_setting() {
  server.send(200, "text/html", head_form +"</head><body><div class='login'>"+ pushover_form);
}
void reset() {
  server.send(200, "text/html",   "smarthelper will restart please wait for a while . . .");
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
     
    } else if (server.argName(i) == "userkey") {
      User_key = server.arg(i);
      User_key.trim();
      Serial.println(User_key);
      Serial.println(User_key.length());
    }else if (server.argName(i) == "fn") {
      firstname = server.arg(i);
      firstname.trim();
    }else if (server.argName(i) == "ln") {
      lastname = server.arg(i);
      lastname.trim();
    }else if (server.argName(i) == "w") {
      weight = server.arg(i);
      weight.trim();
    }else if (server.argName(i) == "h") {
      height = server.arg(i);
      height.trim();
    }else if (server.argName(i) == "bday") {
      brithday = server.arg(i);
      brithday.trim();
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
    file2.println("firstname = " +  firstname);
    file2.println("lastname = " + lastname);
    file2.println("weight = " +  weight);
     file2.println("height = " +  height);
    file2.println("brithday = " +  brithday);
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
          digitalWrite(wifi_led, LOW);
          delay(250);
          beep(100);
          Serial.print(".");
          digitalWrite(wifi_led, HIGH);
          delay(250);
         
        }
      }
    } else {
      j = 5;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(wifi_led, HIGH);
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
  Serial.println("Please wait 30 secs for SPIFFS to be formatted");
   SPIFFS.format();
  Serial.println("Spiffs formatted");
    
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
      }else if (line.startsWith("firstname = ")) {
        firstname = line.substring(12);
        firstname.trim();
        Serial.println(firstname.length());
        Serial.println(firstname);

      }else if (line.startsWith("lastname = ")) {
        lastname = line.substring(11);
        lastname.trim();
        Serial.println(lastname.length());
        Serial.println(lastname);

      }else if (line.startsWith("weight = ")) {
        weight = line.substring(9);
        weight.trim();
        Serial.println(weight.length());
        Serial.println(weight);

      }else if (line.startsWith("height = ")) {
        height = line.substring(9);
        height.trim();
        Serial.println(height.length());
        Serial.println(height);

      }else if (line.startsWith("brithday = ")) {
        brithday = line.substring(11);
        brithday.trim();
        Serial.println(brithday.length());
        Serial.println(brithday);

      }

    }
    if (s == 3) {
      s = 0;
      p = 0;
    }
  }
  file.close();
}
/** Is this an IP? */
boolean isIp(String str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

boolean captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(ssid)+".local")) {
    Serial.print("Request redirected to captive portal");
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}
void handleNotFound() {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 404, "text/plain", message );
}

void setup_apmode() {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  //delay(1000);
  //IPAddress apip = WiFi.softAPIP();
   Serial.print("visit: \n");
  Serial.println(WiFi.softAPIP());
  // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
 // dnsServer.setTTL(300);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is DNSReplyCode::NonExistentDomain
  //dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
 dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
 dnsServer.start(DNS_PORT, "*", apIP);
  // start DNS server for a specific domain name
 
  // Set up the endpoints for HTTP server,  Endpoints can be written as inline functions:
 
  server.on("/", web_page   );
  server.on("/i", info   );
  server.on("/r", reset);
  server.on("/wifisave", handle_msg);                          // And as regular external functions:
  server.on("/wifi", wifi);                          // And as regular external functions:
  server.on("/pushover", pushover_setting);                          // And as regular external functions:
   server.onNotFound(handleNotFound);
  server.begin();                                         // Start the server

}

void IMUMPU6050_detec() {
  Serial.println("IMUMPU6050");
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  tmp1_IMU = Raw_AM;
  tmp2_IMU = Raw_Gy;
  Raw_AM = (pow(pow(ax, 2) + pow(ay, 2) + pow(az, 2), 0.5));
  Raw_Gy = (pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5));
  double z1 = abs((tmp1_IMU - Raw_AM) / 2048);
  double z2 = abs((Raw_Gy) / 16.384); //dreegree/sec
  
  filteredMeasurement_IMU = myFilter.getFilteredValue(z1);
  filteredMeasurement2_IMU = myFilter2.getFilteredValue(z2);
    Serial.print(z1);
  Serial.print(",");
  Serial.print(filteredMeasurement_IMU);
  Serial.print(",");
  Serial.print(z2);
  Serial.print(",");
  Serial.println(filteredMeasurement2_IMU);
  if (check_sleep_IMU == 1) {

    //Serial.println("in sleep");
    timerIMU = millis();

    if (timerIMU - timer_check2_IMU <= 5000) {

      if (z2 < 50) {
        //Serial.print("Critical sleep");//แจ้งเตือน Pushover
        check_sleep_IMU = 1;
       
      }
      else if (z2 >= 50 ) {
        //Serial.print("knocking");
        check_sleep_IMU = 0;
      }
       state = 0;
    } else {
      Serial.print("send message");
      state = 1;
      timeOut = timer;
      check_sleep_IMU = 0;
      setState_IMU = 0;
      timer_check1_IMU = millis();
      timer_check2_IMU = millis();
    }
  }
   if ((filteredMeasurement2_IMU > 200 || setState_IMU == 1) && (check_sleep_IMU == 0)) {
    Serial.println("in");
    timerIMU = millis();
    if (timerIMU - timer_check1_IMU <= 1000) {
      setState_IMU = 1;
      if (z1 >= 4.7 ) {
        Serial.print("Critical");//แจ้งเตือน Pushover
//        digitalWrite(13, HIGH);
        delay(2000);
//        digitalWrite(13, LOW);
        check_sleep_IMU = 1;
        timer_check2_IMU = millis();
      }
    } else {
      setState_IMU = 0;
      timer_check1_IMU = millis();
    }
  }

}

void siren(){
  int freq;
   for(freq = 500; freq < 1800; freq += 5)
         {
            
      digitalWrite(emergency_led, HIGH);
      pinMode(vibration_motor, OUTPUT);
      digitalWrite(vibration_motor , LOW);
           tone(buzzer, freq, time);     // Beep pin, freq, time
           delay(5);
         }
      for(freq = 1800; freq > 500; freq -= 5) 
         {
          
           tone(buzzer, freq, time);     // Beep pin, freq, time
         
          digitalWrite(emergency_led, LOW);
          pinMode(vibration_motor, INPUT);
           delay(5);
         }
}
void setup() {
  Serial.begin(115200);

  //set vibration to "Low" first
  pinMode(vibration_motor, OUTPUT);
  digitalWrite(vibration_motor, LOW);
//#ifdef ESP8266
//  Wire.begin(5, 4);
//#endif
  //initialize IMU MPU6050
  Wire.begin();
  Serial.println("Initialize MPU");
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "Connected" : "Connection failed");
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);

  // initialize input/output
  pinMode(confirm_button, INPUT);
  pinMode(battery_led, OUTPUT);
  pinMode(emergency_led, OUTPUT);
  
  pinMode(wifi_led, OUTPUT);
  pinMode( battery_adc, INPUT);
  pinMode(battery_led , OUTPUT);
  pinMode(buzzer , OUTPUT);
  prepareFile();
  pinMode(vibration_motor, INPUT);
  timeOut = millis();
 
  beep(50);
  beep(50);
}

void loop() {
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
          program_mode = 1; //

        }
      } else {
        preTime = timer;
      }
    } else {
      program_mode = 2;
    }
    digitalWrite(emergency_led, HIGH );
    digitalWrite(battery_led, HIGH  );
    digitalWrite(wifi_led, HIGH );
  }
  //buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (program_mode == 1) {
    // turn LED on:

    if (start_ap == 1) {
      digitalWrite(battery_led , LOW );
      check_battery(battery_led);
      setup_apmode();

      start_ap = 0;
      beep(60);
      beep(60);
    }
    dnsServer.processNextRequest();
    server.handleClient();
    read_battery_milsec(5*60000, battery_led);//อ่านทุกๆ nวิ
    digitalWrite(emergency_led, HIGH  );
    digitalWrite(  wifi_led, HIGH );
    delay(250);
    digitalWrite(emergency_led, LOW  );
    digitalWrite(  wifi_led, LOW );
    delay(250);
  }
  if (program_mode == 2) {
    if (start_ap == 1) {
      digitalWrite(battery_led , LOW );
      digitalWrite(emergency_led, LOW  );
       check_battery(battery_led);
      setup_wifi();
      //  digitalWrite(ledPin, LOW);
      //   ESP.restart();
      start_ap = 0;

    }
    if (WiFi.status() != WL_CONNECTED) {
      setup_wifi();
    }
    check_status(60*60000);
    read_battery_milsec(5*60000, battery_led);//อ่านทุกๆ nวิ
   // toggle_battery_led(1000, battery_led); // led on pin12 battery
    if (state == 0) {

      // buttonState = digitalRead(buttonPin);
      IMUMPU6050_detec();
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

      if (((timer - timeOut) / 1000) < 8) {
        digitalWrite(emergency_led, LOW);
        pinMode(vibration_motor , OUTPUT);
        digitalWrite(vibration_motor , LOW);
        delay(500);
        digitalWrite(emergency_led, HIGH);
        pinMode(vibration_motor ,INPUT);
        delay(500);

        if (buttonState == HIGH) {
          unsigned long timerAck = ((timer - preTime) / 1000);
          if ( timerAck >= 1) {
            Serial.println("no problem ");
            digitalWrite(emergency_led, LOW);
           // digitalWrite(vibration_motor , LOW);
            delay(50);
            state = 5;
          }
        } else {
          preTime = timer;
        }

      } else {
        Serial.println("notify()");
        send_notify(Api_fall,"FALL DETECTION PLEASE CHECK "+firstname+" "+lastname,2,30); //ส่ง fall detec
        state = 2;
        Serial.println("back to state 2");
      }

    } else if (state == 2) {
      //buzzer , vibration on
    Serial.println("state2");
       buttonState = digitalRead(confirm_button);
      siren();
      if (buttonState == HIGH ) {

        unsigned long timerAck = ((timer - preTime) / 1000);
        if ( timerAck >= 1) {
         
         analogWrite(buzzer, 0);       // 0 turns it off
          state = 4;
        }

      } else {
        preTime = timer;
      }

    } else if (state == 3) {
      buttonState = digitalRead(confirm_button);
      if (buttonState == HIGH) {

      } else {
        send_notify(Api_press,firstname+" "+lastname+" HELP ME PLEASE !!!! ",2,30); //ส่ง fall detec
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
