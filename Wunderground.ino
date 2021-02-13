//---------------------------------------------------------SEND TO WU----------------------------------------------
//Protocol: http://wiki.wunderground.com/index.php/PWS_-_Upload_Protocol
/*
 * list of fields:
action [action=updateraw] -- always supply this parameter to indicate you are making a weather observation upload
ID [ID as registered by wunderground.com]
PASSWORD [Station Key registered with this PWS ID, case sensitive]
dateutc - [YYYY-MM-DD HH:MM:SS (mysql format)] In Universal Coordinated Time (UTC) Not local time
winddir - [0-360 instantaneous wind direction]
windspeedmph - [mph instantaneous wind speed]
windgustmph - [mph current wind gust, using software specific time period]
windgustdir - [0-360 using software specific time period]
windspdmph_avg2m  - [mph 2 minute average wind speed mph]
winddir_avg2m - [0-360 2 minute average wind direction]
windgustmph_10m - [mph past 10 minutes wind gust mph ]
windgustdir_10m - [0-360 past 10 minutes wind gust direction]
humidity - [% outdoor humidity 0-100%]
dewptf- [F outdoor dewpoint F]
tempf - [F outdoor temperature] 
 * for extra outdoor sensors use temp2f, temp3f, and so on
rainin - [rain inches over the past hour)] -- the accumulated rainfall in the past 60 min
dailyrainin - [rain inches so far today in local time]
baromin - [barometric pressure inches]
weather - [text] -- metar style (+RA)
clouds - [text] -- SKC, FEW, SCT, BKN, OVC
soiltempf - [F soil temperature]
 * for sensors 2,3,4 use soiltemp2f, soiltemp3f, and soiltemp4f
soilmoisture - [%]
* for sensors 2,3,4 use soilmoisture2, soilmoisture3, and soilmoisture4
leafwetness  - [%]
+ for sensor 2 use leafwetness2
solarradiation - [W/m^2]
UV - [index]
visibility - [nm visibility]
indoortempf - [F indoor temperature F]
indoorhumidity - [% indoor humidity 0-100]
Pollution Fields:
AqNO - [ NO (nitric oxide) ppb ]
AqNO2T - (nitrogen dioxide), true measure ppb
AqNO2 - NO2 computed, NOx-NO ppb
AqNO2Y - NO2 computed, NOy-NO ppb
AqNOX - NOx (nitrogen oxides) - ppb
AqNOY - NOy (total reactive nitrogen) - ppb
AqNO3 -NO3 ion (nitrate, not adjusted for ammonium ion) UG/M3
AqSO4 -SO4 ion (sulfate, not adjusted for ammonium ion) UG/M3
AqSO2 -(sulfur dioxide), conventional ppb
AqSO2T -trace levels ppb
AqCO -CO (carbon monoxide), conventional ppm
AqCOT -CO trace levels ppb
AqEC -EC (elemental carbon) – PM2.5 UG/M3
AqOC -OC (organic carbon, not adjusted for oxygen and hydrogen) – PM2.5 UG/M3
AqBC -BC (black carbon at 880 nm) UG/M3
AqUV-AETH  -UV-AETH (second channel of Aethalometer at 370 nm) UG/M3
AqPM2.5 - PM2.5 mass - UG/M3 
AqPM10 - PM10 mass - PM10 mass
AqOZONE - Ozone - ppb
softwaretype - [text] ie: WeatherLink, VWS, WeatherDisplay
 */
//-------------------------------------------------------------------------------------------------------
void senddata()
{
String wutime = build_timestamp(1);  //defince string and default to 'now'
    wu_count++;
  
 // Create timestamp
// if no NTP sync, use "now"
//if (timeStatus()<1) wutime = "now";
 
//If NTP sync ue time from clock
  Serial.print(F("UTC Time:"));
  Serial.println(wutime);

// We now create a URL for the request
  String url = "/weatherstation/updateweatherstation.php?ID=";  url += WUID;
  url += "&PASSWORD=";  url += WUPASS;
  url += "&dateutc=" + wutime +"&winddir=";  url += winddir;
  url += "&windspeedmph=";  url += windspeed;
  url += "&windgustmph=";   url += windgust;

if (avg2m)
      {
      url += "&windspdmph_avg2m=";       url += windspd_avg2m;
      url += "&winddir_avg2m=";          url += winddir_avg2m;     
      }
if (avg10m)
      {
      url += "&windgustmph_10m=";       url +=windgust_avg10m; 
      url += "&windgustdir_10m=";       url +=winddir_avg10m; 
      } 
 
if (DSPresent) 
      {
        url += "&tempf=";        url += tempout;
      }
 if (BMP1.Present)
      {
        
// Uncomment if using Humidity Sensor
        url += "&dewptf=";       url += dewpout;
        url += "&baromin=";        url += baro;
      } else {
        // We might have got a baro reading from MQTT
        if(baro_eu > 0) {
          url += "&baromin=";        
          url += baro; 
        }
      }
url += "&dewptf=";       url += dewpout;

//Humidity Sensor
  url += "&humidity=";       url += humidity;
  url += "&rainin=";  url += rain1h;
  url += "&dailyrainin=";  url += rain24h; 
              

 if (UV1.Present)
      { 
//        url += "&solarradiation=";
//        url += uvviz_eu;
        url += "&UV=";         url += uvindex_eu;
      }
  url += "&weather=&clouds=&softwaretype=Arduino-ESP8266&action=updateraw";
  
 
    
 
  Serial.print(F("Requesting URL: "));
  Serial.println(url);

 Serial.println(F("Send to WU Sensor Values"));
  Serial.print(F("connecting to "));
  Serial.println(host);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
   Serial.println(F("connection failed"));
//    startwifi();

if (millis()-wu_down_timer >=  wu_interval) {
// write failure to connect to file
  // open file for writing
  File f = SPIFFS.open("/wu_data.txt", "a");
  if (!f) {Serial.println(F("file open failed"));}
// write to file just in case
// write string to file 
    f.println(url);
    f.close();
    wu_down_timer = millis();  //reset this timer every time we have a success
  
}

    
    wu_fail++;
    log(WU_fail);
    return;
  }

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
 
  Serial.println();
  Serial.println(F("closing connection"));
  client.stop();

// write success to connect to file
  // write to file just in case
  // write string to file 
  //  f.println(url);
  //  f.close();
    wu_down_timer = millis();  //reset this timer every time we have a success
//    log(WU_success);

   //wifi_set_sleep_type(NONE_SLEEP_T);
  //wifi_set_sleep_type(MODEM_SLEEP_T);
//  wifi_set_sleep_type(LIGHT_SLEEP_T);
}
