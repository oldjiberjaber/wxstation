/*
  Ardiono Weather Station
  based on Davis instruments and SPI sensors
  ESP-12 device Wemos D1 Mini Pro
  160MHz Clock
  4M (3M SPIFFS)

  Arduino ESP8266 based MCU with Davis Anemometer and rain gauge.  Main unit installed within a home made Stephenson screen with temperature and humidity measured using a DS2438 and  HIH-5031 (https://taaralabs.eu/1-wire-humidity-temperature-sensor/).

    013 - Added "reset" via serial port command (NOTE: reset will hand on first boot after serial flash, use wifi or a manual reboot after flashing.)
          Added WIFISTN definition for OTA identification
  //014 - Added none blocking MQTT connection
  //015 - Removed BME and returned BMP sensor, commented humidity and dew point calcs and statements out of use
  //016 - Removed temperature using BME and BMP sensor, Moved to Dallas One Wire for reliability
  //017 - Implemented remote pressure sensor and recieve value over MQTT. MQTT msgs no longer published as retained messages some issues if large number of MQTT msgs published in a row. BEWARE
  //018 - Added DS2438 Temp & Humidity sensor
  //019 - Added additional call to read DS2438 Temp & Humidity sensor and upload to Thingspeak, used F() Macro to save circa 3K dynamic memory
  //020 - We now tweet via Thingspeak Thing! A reboot message and a broadcast of data every 92 mins
          Split out scheduled time tasks in to own tabs to aid readability of loop function
          Added temperature trend and 10m average
  //021 - Added variable char array for MQTT message buffer 
        - Amended build_timestamp() 
        - Added Uptime clock, build_uptime()
        - Added twitter enable flag
        - Added status MQTT message in same format as Sonoff devices sent with the MQTT updates
        - Stopped syslog from writting to file to see if it extends device life time
  //022 - Added Humidity back in to Wunderground upload 
  //023 - ??? No version notes!
  //024 - added some comments to the main loop
        - disabled thingspeak data upload (twitter still enabled)
  //025 - added switches to turn off WU and MQTT for testing 
        - renamed system clock to myclock to solve compile error.
  //026 - added Met Office WOW upload
        - added sw version define
        - ICACHE_RAM_ATTR in interrupt prototypes to move them into IRAM
  /027  - Added sandpit option to test and work through tings without taking actual live wxstn off line
        - added env MQTT transmission
  /028  - consolidated MQTT in to a JSON object rather than seperate values
  /029  - ArduinoJSON library and MQTT for env message 
  /030  - ArduinoJSON message for WeeWX MQTT service & updated MQTT STATE JSON to include signal (new function in Wifi tab to calc from RSSI)
        - added MqttCount counter, made uptimeclock global so can be used for seconds of uptime.
  /031  - Implemented SmallFS to replace EEPROM and increased the variables that are saved 
        - Added INFO1 & INFO2 MQTT broadcasts under tele topic to send saved and read variables
        - Removed METWoW Upload 
        - Added MQTT LWT
  /032  - Implemented MQTT Calibration option from other sketches
   033  - Added WeeWX_RainAmount to provide amount of rain per observation to WeeWX     
   034  - Created seperate credentials tab    
 */
#define SW_VERSION "034"
//#define SANDPIT


#include <ESP8266WiFi.h>      //ESP library from http://github.com/esp8266/Arduino
#include <Wire.h>
#include <OneWire.h> // Dallas one wire lib
#include <DS2438.h>  // debug_DS2438 is available here for debugging
#include "RTClib.h"  // RTC Lib 
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
//#include <Adafruit_BME280.h>
#include "Adafruit_SI1145.h"
//#include <EEPROM.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
//#include <ESP8266FtpServer.h>
#include <string.h>
#include <PubSubClient.h>
#include "RunningAverage.h"
#include <ArduinoJson.h>
#include "FS.h"
#include <LittleFS.h>

#define COMPILE_DATE __DATE__
#define COMPILE_TIME __TIME__
#define VaneOffset +30;
//#define VaneOffset 0;

//Error and Logging codes
const int Sys_boot= 1;
const int NTP_success= 2;
const int NTP_fail= 3;
const int WU_success= 4;
const int WU_fail= 5;
const int I2C_BMP1fail= 6;
const int I2C_UV1fail= 7;
const int I2C_RTCfail = 8;
const int I2C_RTCpower = 9;
const int I2C_RTCsync = 10;
const int INT_Raindrop = 11;
const int Sys_Boottime = 12;
/*********************************************
   Prototypes
 *********************************************/
void WIFI_connection();
time_t getNtpTime();
// void sendNTPpacket(IPAddress &address);
unsigned long sendNTPpacket(IPAddress& address);
unsigned long ntptime();
int localhour();
int localmin();
int localsec();
void printDigits(int digits);
void digitalClockDisplay();
void ThingSpeakSend();
void Arduino_OTA();
void startudp();
void ICACHE_RAM_ATTR rain();
void ICACHE_RAM_ATTR rpm();
void Sensors_Debug();
float DirWind();
float speedwind();
float speedgust();
void senddata();
void Wind_Direction_cardinal(float wind_dir);
bool ReadI2C(void);
long int check_valid(void);
String build_timestamp(bool wu);
void sync_rtc();
void log(const int logcode);
time_t get_rtc();
void mqttclient_pub(String MQTTTOPIC, String MQTTMSG, boolean mqttdebug);
void mqtt_callback(char* topic, byte* payload, unsigned int length) ;
boolean reconnect();
void mqtt_pub();
 boolean getOneWireTemp();
 char* tempToAscii(double temp);
 bool Find_Sensors(void);
void wowsenddata(void);
void updateTwitterStatus(String tsData);
void Tasks5s(void) ;
void Tasks60s(void);
void ThingTweetSend();
void StatusMsg();
/*********************************************
   Definitions & Variables
 *********************************************/
    
uint8_t MAC_array[6];
char MAC_char[18];
String ID_MAC;


// Calibration mode when enabled:
// changes the MQTT broadcast for home/temp/<device> from average value to readTemperature()-temp_offset
// increases the MQTT broadcast rate to every 15s
// decreases the time it takes to write the stored values to the filesystem for backup.
String calibration_target_topic = "tele/device/cal/#";
String calibration_target_topicT = "tele/device/cal/t";
String calibration_target_topicH = "tele/device/cal/h";
bool Calibration = 0 ;                      //debug = 1 -> enable read value-offset



/*********************************************
   Time Specifics
 *********************************************/
const int timeZone = 0;               // Central European Time
//const char* timerServerDNSName = "0.europe.pool.ntp.org";
long currentmillis = 0;
long ntp_interval = 600000;        // 10 mins
unsigned long currentMillis, ntp_currentMillis;
bool ntp_sync = 0;
unsigned int ntp_attempts, ntp_success;
IPAddress timeServerIP; // time.nist.gov NTP server address
DateTime myclock;
unsigned long boottime;
//---------------------NTP VAR---------------------

// const char* ntpServerName = "europe.pool.ntp.org";
const char* ntpServerName = "ntp.homehub.btopenworld.com";
//const char* ntpServerName = "europe.pool.ntp.org";
//const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];
unsigned long epoch;
bool ntp_OK = 0;
bool RTC_OK = 0;
/*********************************************
   Instances
 *********************************************/
// A UDP instance to let us send and receive packets over UDP
unsigned int localPort = 2390;      // local port to listen for UDP packets
WiFiUDP udp;
//WiFiServer server81(81);
//WiFiClient client = server81.available(); 
WiFiClient client; 
//FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial
int temp_counter = 0;
// MQTT Specifics
//PubSubClient client(espClient);
PubSubClient mqttclient = client;

long lastReconnectAttempt = 0;

/*********************************************
   Connections
 *********************************************/
//A0 analog Arduino pin A0 (A0 on ESP8266-012 D1 Mini) //Wind Direction //Read wind direction signal 
#define WD_Pin A0 
//D0 Arduino pin 16 (D0 on ESP8266-012 D1 Mini)

//D1 Arduino Pin 5 (D1 on ESP8266-012 D1 Mini) //Rain Sensor //Read wind speed signal 
#define RS_Pin D1 
//D2 Arduino pin 4 (D2 on ESP8266-012 D1 Mini) //Read wind speed signal  //Wind Speed
#define WS_Pin D2 
//D3 Arduino pin 0 (D3 on ESP8266-012 D1 Mini)

//D4 Arduino pin 2 (D4 on ESP8266-012 D1 Mini)
#define LED D4 //On-board Indicator LED (D7 on ESP8266-012 D1 Mini)
//D5 Arduino pin 14 (D5 on ESP8266-012 D1 Mini)
#define SDA_Pin D5 
//D6 Arduino pin 12 (D6 on ESP8266-012 D1 Mini)
#define SCL_Pin D6 

//D7 Arduino pin 13 (D7 on ESP8266-012 D1 Mini)
#define DSPin D7 //Dallas Temperature Sensor
//D8 Arduino pin 15 (D8 on ESP8266-012 D1 Mini)

//I2C

typedef struct
{
  unsigned int counter;
  boolean  Present;
  float Pressure, Humidity, TempC, TempF, DewPoint;
} BMP_Sensor;

typedef struct
{
  unsigned int counter;
  boolean  Present;
  float UVindex, Vis, IR;
} UV_Sensor;

BMP_Sensor BMP1;
UV_Sensor UV1;
RTC_DS3231 rtc;

/*********************************************

*********************************************/


float altitudepws = 125.00;      //LOCAL Alitude of the PWS to get relative pressure
const char* host = "weatherstation.wunderground.com";
//---------------------WEATHER VAR-----------------
float windspeed_eu;
float windgust_eu;
float winddir_eu;
float tempout_eu;
float dewpout_eu;
float humidity_eu;
float baro_eu;
float rain1h_eu;
float rain24h_eu;
float uvindex_eu, uvviz_eu;
boolean avg2m = false;
boolean avg10m = false;
//running averages
RunningAverage windspeed_eu_avg2m(2), winddir_eu_avg2m(2), winddir_eu_avg10m(10), windgust_eu_avg10m(10),baro_eu_avg(181),temp_avg10m(10);
//unsigned int ave_cnt_10m = 0;
//float lastave_baro_eu;
float ave_baro_eu, temp_avg;
String barotrend, temptrend;

  
//US
String windspeed;
String windgust;
String winddir;
String windspd_avg2m;
String winddir_avg2m;
String windgust_avg10m;
String winddir_avg10m;
String baro_avg3h;

String tempout;
String dewpout;
String humidity;
String baro;
String rain1h;
String rain24h;
//{"TempCal":{"raw":4.13,"offset":0.5,"corrected":4.63},"HumCal":{"raw":100,"offset":-6.48,"corrected":93.52},"CalMode":false}
int radio = 60;               // Radio from vertical anemometer axis to a cup center (mm) 
//int temp_offset = -4.6;         // Temp. Offset
float temp_offset = 0.5;         // Temp. Offset
float hum_offset = -6.48;          // Humidity Offset
float pressure_offset = 0;          // Pressure Offset
int averagecounter =1;
//---------------------PROG VAR--------------------

unsigned long count5sec;
unsigned long count15sec;
unsigned long count60sec;
unsigned long count5min;
unsigned long count10min;
unsigned long count1h;
unsigned int pulseswind;
unsigned int pulsesgust;
float tempwindgust;
float raincount = 0.00;
float raincount1h = 0.00;
const float pi = 3.14159265;  // pi number
float calcgustspeed;
float calcwindspeed;
float sensor_count = 0.00f;
float winddir_sum = 0.00f;
float tempout_sum = 0.00f;
float humidity_sum = 0.00f;
float baro_sum = 0.00f;
float uvindex_sum = 0.00f;
float uvviz_sum = 0.00f;
unsigned long rain_last=0;
unsigned long wind_last=0;
 float wu_count = 0;  //count how many times we have attempted a connection to wu
 float wu_fail = 0;  //count how many times we have Failed in attempted a connection to wu
long wu_down_timer=0; //timer used to only save a wu url 
long wu_interval = 300009;        // 5 mins plus a bit

int addr=0;
float pressure;

//Adafruit_BME280 bme;

//Adafruit_BME280 bme; // I2C
Adafruit_BMP280 bme; // I2C
bool I2C_BME280 = 0;   //set this is we are using the BME (Humidity sensor version)
Adafruit_SI1145 uv = Adafruit_SI1145();
OneWire  ds(DSPin);  // Dallas 1-wire on pin 10
bool DSPresent = false;
//DS18B20 Address
byte DSaddr[8] = {0x28, 0xFF, 0x10, 0x38, 0x2E, 0x4, 0x0, 0x3B}; 
uint8_t DS2438_address[] = { 0x26, 0xad, 0xc9, 0x00, 0x02, 0x00, 0x00, 0xe9 };
DS2438 ds2438(&ds, DS2438_address);
String Wind_DirC;
//String envString = "{\"temp\":\"";
String wxString = "";

// -----------------------------------------------
//ArduinoJSON Document: location WeeWX 
/*

{
"barometer":977.67,
"outTemp":23.78,
"dewpoint":23.78,
"outHumidity":31.94,
"windSpeed":99.99,
"windDir":360,
"windGust":99.99,
"windGustDir":360,
"rain":0.00,
"rainRate":0.00,
"RSSI":81,
"signal":100
}
Document size
Data structures  192 Bytes needed to stores the JSON objects and arrays in memory 
Strings 104 Bytes needed to stores the strings in memory 
Total (minimum) 296 Minimum capacity for the JsonDocument.
Total (recommended) 384 Including some slack in case the strings change, and rounded to a power of two

*/ 
//11 items
//const size_t WeeWXcapacity = JSON_OBJECT_SIZE(11);
DynamicJsonDocument WeeWX(384);
float WeeWX_RainAmount = 0;
// -----------------------------------------------
//ArduinoJSON Document: location env 
/*
{"temp":2.97,"hum":100,"tempraw":2.97}

Document size
Data structures  48  Bytes needed to stores the JSON objects and arrays in memory 
Strings 17  Bytes needed to stores the strings in memory 
Total (minimum) 65  Minimum capacity for the JsonDocument.
Total (recommended) 96  Including some slack in case the strings change, and rounded to a power of two
*/ 

//const size_t envcapacity = JSON_OBJECT_SIZE(7);
DynamicJsonDocument env(96);
double round2(double value);


// -----------------------------------------------
//ArduinoJSON Document: location wxstation 

/*
{
  "Time": "2020-12-29T17:30:46",
  "Uptime": "0T00:25:09",
  "UptimeSec": 1509,
  "Heap": 27,
  "SleepMode": "Dynamic",
  "Sleep": 50,
  "LoadAvg": 19,
   "MqttCount" : 6,
  "Wifi": {
    "AP": 1,
    "SSId": "SSID",
    "BSSId": "MAC",
    "Channel": 11,
    "RSSI": 62,
    "Signal": -69   
  }
}
https://arduinojson.org/v6/assistant/ 

Document size (ESP8266 Serialize)

Data structures  240 Bytes needed to stores the JSON objects and arrays in memory 
Strings 171 Bytes needed to stores the strings in memory 
Total (minimum) 411 Minimum capacity for the JsonDocument.
Total (recommended) 512 Including some slack in case the strings change, and rounded to a power of two
*/

//const size_t statecapacity = JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(6);
//DynamicJsonDocument stateResult(statecapacity);
DynamicJsonDocument stateResult(512);
JsonObject Wifi = stateResult.createNestedObject("Wifi");
unsigned int MqttCount = 0;
unsigned long uptimeclock = 0;
// -----------------------------------------------
//ArduinoJSON Document: saved values for reboot 
DynamicJsonDocument savedvalues(512);
unsigned int eepromctr = 0;


// -----------------------------------------------
//ArduinoJSON Document: Calibration broadcast to tele/<device>/INFO3
/*
 *{
  "Cal": "true",
  "TempCal": {
    "raw": 13.99,
    "offset": 4.4,
    "corrected": 13.99
  },
  "HumCal": {
    "raw": 999,
    "offset": -4.4,
    "corrected": 913.99
  }
}
 */
DynamicJsonDocument calibration(256);
JsonObject TempCal = calibration.createNestedObject("TempCal");
JsonObject HumCal = calibration.createNestedObject("HumCal");


#ifdef SANDPIT
bool debug = 1;
bool Wind_debug_spd = 0;               //debug wind interupts= 1 -> enable debug
bool Wind_debug_dir = 0;               //debug wind interupts= 1 -> enable debug
bool Rain_debug = 1;                   //debug rain interupts= 1 -> enable debug
bool Sensors_debug = 0;                //debug Sensors interupts= 1 -> enable debug
bool I2C_debug_BME280 = 0;             //debug I2C Sensor Reads= 1 -> enable debug
bool I2C_debug_uv = 0;                 //debug I2C Sensor Reads= 1 -> enable debug
bool debug_DS18B20 = 0;                //debug DS18B20 Sensor Reads= 1 -> enable debug
bool debug_DS2438 = 0;                 //debug DS2438 Sensor Reads= 1 -> enable debug
bool debug_FS = 1;                 //debug Filesystem = 1 -> enable debug
bool MQTTdebug = 1;                    //debug MQTT data, set to 1 to enable


bool ClearEEPROM = 0;                  //Clear EEPROM = 0 -> set true (1) to clear the EEPROM, only run this once!
bool ClearEEPROMnow = 0;                  //Clear EEPROM = 0 -> set true (1) to clear the EEPROM, only run this once!

bool TweetEnable = 0;                  //Enable the Twitter broadcasts via ThingTweet set true (1) to enable service 
bool WUEnable = 0;                     //Enable the WeatherUnderground set true (1) to enable service 
bool MQTTEnable = 0;                   //Enable the MQTT broadcast of data, set true (1) to enable service 
#else
bool debug = 0;                        //debug = 1 -> enable debug
bool Wind_debug_spd = 0;               //debug wind interupts= 1 -> enable debug
bool Wind_debug_dir = 0;               //debug wind interupts= 1 -> enable debug
bool Rain_debug = 1;                   //debug rain interupts= 1 -> enable debug
bool Sensors_debug = 0;                //debug Sensors interupts= 1 -> enable debug
bool I2C_debug_BME280 = 0;             //debug I2C Sensor Reads= 1 -> enable debug
bool I2C_debug_uv = 0;                 //debug I2C Sensor Reads= 1 -> enable debug
bool debug_DS18B20 = 0;                //debug DS18B20 Sensor Reads= 1 -> enable debug
bool debug_DS2438 = 0;                 //debug DS2438 Sensor Reads= 1 -> enable debug
bool MQTTdebug = 0;                    //debug MQTT data, set to 1 to enable
bool debug_FS = 1;                 //debug Filesystem = 1 -> enable debug

bool ClearEEPROM = 0;                  //Clear EEPROM = 0 -> set true (1) to clear the EEPROM, only run this once!
bool ClearEEPROMnow = 0;                  //Clear EEPROM = 0 -> set true (1) to clear the EEPROM, only run this once!
bool TweetEnable = 1;                  //Enable the Twitter broadcasts via ThingTweet set true (1) to enable service 
bool WUEnable = 1;                     //Enable the WeatherUnderground set true (1) to enable service 
bool MQTTEnable = 1;                   //Enable the MQTT broadcast of data, set true (1) to enable service 


#endif
