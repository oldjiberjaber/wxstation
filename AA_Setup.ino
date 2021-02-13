void setup(){
//setup inputs and outputs
  pinMode(WD_Pin, INPUT); //Wind direction (ADC)
  pinMode(WS_Pin, INPUT_PULLUP); //Wind Speed
  pinMode(RS_Pin, INPUT_PULLUP); //Rain Sensor
  pinMode(LED, OUTPUT); //Status LED
//Start I2C
  Wire.begin(SDA_Pin, SCL_Pin);

//Wire.begin(SDA, SCL);

// Setup console
  Serial.begin(115200);
  delay(100);
 Serial.println(F(""));
  Serial.println(F(""));
  Serial.print(F("Arduino ESP8266 Code Version: "));
  Serial.println(SW_VERSION);
  Serial.print(F("Compiled on: "));
  Serial.print(COMPILE_DATE);
  Serial.print(F(" "));
  Serial.println(COMPILE_TIME);

  delay(500);
  digitalWrite(LED, 0);
    
//let the syslog know we have booted up
log(Sys_boot);  

// RTC
Serial.println(F("Searching for I2C RTC"));
if (!rtc.begin()) { 
  if (check_valid() == 1696376785) { Serial.println(F("Could not find a valid DS3231 RTC, check wiring!")); log(I2C_RTCfail);};
                  } else { 
                    if (check_valid() == 1696376785) { Serial.println(F("Could not find a valid DS3231 RTC, check wiring!")); log(I2C_RTCfail);
                         } else {
                                // Serial.println(check_valid());
                                Serial.println(F("found a valid DS3231 RTC"));   
                                boottime = rtc.now().unixtime(); //record the time we booted up so we can calulate uptime
                                log(Sys_Boottime);
                                 }                  
                        if (rtc.lostPower()) {
                                             Serial.println(F("RTC lost power, Time not valid - will set when NTP valid!"));
                                             rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
                                             Serial.println("**** TIME TEST *****");
                                             get_rtc();
                                             Serial.println("**** TIME TEST *****");
                                             RTC_OK = 0;
                                             } else {
                                                    Serial.println(F("RTC Time probably valid"));
                                                    RTC_OK = 1;
                                                    }
                        
                          
                        }
Serial.println(F("Searching for Sensors"));
 if (!bme.begin()) { Serial.println(F("Could not find a valid BME280 sensor, check wiring!")); BMP1.Present = false; log(I2C_BMP1fail);} else { Serial.println(F("found a valid BME280 sensor")); BMP1.Present = true;}
 if (! uv.begin()) { Serial.println(F("Didn't find Si1145, check wiring!")); UV1.Present = false; log(I2C_UV1fail);}else { Serial.println(F("found a valid Si1145 sensor")); UV1.Present = true;}
// if (! Find_Sensors()) { Serial.println("Didn't find DS18B20, check wiring!"); DSPresent = false; }else { Serial.println("found a valid DS18B20 sensor"); DSPresent = true;}
 if (! Find_Sensors()) { Serial.println(F("Didn't find DS2438, check wiring!")); DSPresent = false; }else { Serial.println(F("found a valid DS2438 sensor")); DSPresent = true;    ds2438.begin();}

Serial.println(); 

  //Start WiFi
  WIFI_connection();
  //Start OTA Client
  Arduino_OTA();
  //Start UDP
  startudp();

  /////FTP Setup, ensure SPIFFS is started before ftp;  /////////
/*if (FTPEnable) {
      ftpSrv.begin("esp8266","esp8266");    //username, password for ftp.  set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)
      Serial.println(F("FTP Running!"));
      }    
*/
//Start MQTT stuff
 mqttclient.setServer(mqtt_server, 1883);
 mqttclient.setCallback(mqtt_callback);
 calibration_target_topic = "tele/"+mqtt_ID+"/cal/#";  //sets up what we listen for to receive calibration offset value                                              
 calibration_target_topicT = "tele/"+mqtt_ID+"/cal/t";  //sets up what we listen for to receive calibration offset value                                              
 calibration_target_topicH = "tele/"+mqtt_ID+"/cal/h";  //sets up what we listen for to receive calibration offset value
 calibration["CalMode"] = Calibration;             
 reconnect();
///// **********************************************************************
MountFS(); //Mount LittleFS


if (ClearEEPROMnow)
  {
  //RESET SAVED VALUES TO MIDDLE AVERAGE NUMBERS  
  clearConfig();
  //END - RESET EEPROM CONTENT - ONLY EXECUTE ONE TIME - AFTER COMMENT
  }
  
  if (!loadConfig()) {
    Serial.println(F("Failed to load config"));
  } else {
    Serial.println(F("Config loaded"));
  
}
///// **********************************************************************
  if (raincount1h==0)
  {
    count1h = millis();
  }
 
  
  
  currentMillis = millis();
  ntp_currentMillis = currentMillis;
  
  count5sec = millis();
  count60sec = millis();
  count5min = millis();
  count10min = millis();
  lastReconnectAttempt = 0;
  
//Interupt call backs for rain and wind speed
  attachInterrupt(WS_Pin, rpm, FALLING);
  attachInterrupt(RS_Pin, rain, FALLING);
 //start interupt
  sei();
  pulseswind = 0;
  pulsesgust = 0;
  
//Enable NTP auto update
  setSyncInterval(23); //initial sync interval
  setSyncProvider(getNtpTime); //this will trigger a NTP request when called

 updateTwitterStatus("WX STN Rebooted "+build_timestamp(2)+" @jiberjaber");
 StatusMsg();  

  }
