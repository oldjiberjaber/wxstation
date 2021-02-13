/*
//Error and Logging codes
const int Sys_boot= 1;
const int NTP_success= 2;
const int NTP_fail= 3;
const int WU_success= 4;
const int WU_fail= 5;
const int Sensor_BMP1fail= 6;
const int Sensor_UV1fail= 7;
const int I2C_RTCfail = 8;
const int I2C_RTCpower = 9;
const int I2C_RTCsync = 10;
const int INT_Raindrop = 11;
const int Sys_Boottime = 12;


*/

 /*-------- Log to file ----------*/
 void log(const int logcode)
{   
 // open file for writing
//  File fl = SPIFFS.open("/log.txt", "a");
//  if (!fl) {
//      Serial.println(F("file open failed"));
//  }

// build timestamp
String logt = build_timestamp(0);

switch (logcode) {
    case Sys_boot:
      logt += " - INFO: System UP, Arduino ESP8266 Code compiled on: ";
      logt +=  COMPILE_DATE;
      logt +=  " ";
      logt +=  COMPILE_TIME;
      break;
    case NTP_success:
      logt += " - INFO: NTP attempt success";
      break;
    case NTP_fail:
      logt += " - ERROR: NTP attempt failed";
      break;
    case WU_success:
//      logt += " - INFO: WU connect success";
      break;
    case WU_fail:
      logt += " - ERROR: WU connect failed " ;
      logt += wu_fail; 
      logt += " (" ;
      logt += wu_count ;
      logt += " attempts) " ;
      logt += (wu_fail/wu_count)*100; 
      logt +=" % failures";
      break;
    case I2C_BMP1fail:
      logt += " - ERROR: BMP Sensor not detected";
      break;
    case I2C_UV1fail:
      logt += " - ERROR: UV Sensor not detected";
      break;
    case I2C_RTCfail:
      logt += " - ERROR: RTC not detected";
      break;
    case I2C_RTCpower:
      logt += " - ERROR: RTC has been powered off";
      break;
    case I2C_RTCsync:
      logt += " - INFO: RTC has been synced to NTP";
      break;
    case INT_Raindrop:
      logt += " - INFO: Rain Guage interupt";
      break;
    case Sys_Boottime:
      long int lastboottime;
      float timesincelastboot;
      //eepromstring=eepromGet("boottime");
      //lastboottime=eepromstring.toInt();
      //Serial.print(F("Last Boottime VALUE FROM EEPROM: "));
      //Serial.println(eepromstring.toInt());
      //eepromstring = String(check_valid(),10);
      timesincelastboot = (check_valid() - lastboottime)/60;
      //eepromSet("boottime", eepromstring);
      Serial.print(F("time since last boot (mins): "));
      Serial.println(timesincelastboot);
      logt += " - INFO: Time since last reboot (mins): "; 
      logt += timesincelastboot;
      //eepromstring = "0.00";      
      break;
    default: 
      logt += "Code = UNKNOWN " + logcode;
      // if nothing else matches, do the default
      // default is optional
    break;
    }

  // write success to connect to file
  // write string to file 
//    fl.println(logt);
//    fl.close();

}
