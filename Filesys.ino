//**************************************
/*
 * *** Variables to store to filesystem ***
 * 
{
 "time":"2020-12-30T17:32:30",
 "baroaverage":9999.99,
 "targettemp":99.99
}

{
"raincount":99.00,
"raincount1h": 99.00,
"baroavg": 1500.00,
"tempaverage":99.99,
"windspeed_2m":999.99,
"winddir_2m":360.00,
"winddir_10m":360.00,
"windgust_10m":999.99
}

 * 
 * 
 * 
 */
//**************************************

bool MountFS(void)
{
 Serial.println(F("Mounting FS..."));
  if (!LittleFS.begin()) {
    Serial.println(F("Failed to mount file system"));
    #ifdef LOLINDISP  
    tft.println(F("Failed to mount file system"));
    #endif    
    return false;
  }
  return true;
}






//**************************************

bool loadConfig() {
  File configFile = LittleFS.open("/savedvals.json", "r");
  if (!configFile) {
    Serial.println(F("Failed to open file"));
  return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println(F("file size is too large"));
  return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  //StaticJsonDocument<512> readvalues;
  DynamicJsonDocument readvalues(512);
  auto error = deserializeJson(readvalues, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }
  
String mqttpayload;                
serializeJson(readvalues, mqttpayload);
mqttclient_pub("tele/"+mqtt_ID+"/INFO1",mqttpayload, false); 

//Fill in running average with loaded values...
  for(int A=1; A<=11; A++) 
  {
  //hum.addValue( (float) readvalues["humidityaverage"] );
  temp_avg10m.addValue( (float) readvalues["tempaverage"] ) ;      
  windspeed_eu_avg2m.addValue( (float) readvalues["windspeed_2m"] ) ;
  winddir_eu_avg2m.addValue( (float) readvalues["winddir_2m"] ) ;
  winddir_eu_avg10m.addValue( (float) readvalues["winddir_10m"] ) ;
  windgust_eu_avg10m.addValue( (float) readvalues["windgust_10m"] ) ;
  for(int A=1; A<=18; A++) 
    {
    baro_eu_avg.addValue( (float) readvalues["baroaverage"] );
    Serial.print(F("."));
    }
  Serial.print(F("."));
  }
Serial.println(F("."));

//RunningAverage 
//windspeed_eu_avg2m(2), 
//winddir_eu_avg2m(2), 
//winddir_eu_avg10m(10), 
//windgust_eu_avg10m(10),
//baro_eu_avg(181),
//temp_avg10m(10); 

raincount = (float) readvalues["raincount"]; // 99.99
raincount1h = (float) readvalues["raincount1h"]; // 99.99
float baroaverage = (float) readvalues["baroaverage"]; // 9999.99
float tempaverage = (float) readvalues["tempaverage"]; // 99.99
float windspeed_2m = (float) readvalues[ "windspeed_2m"];//:999.99,
float windsdir_2m = (float) readvalues["winddir_2m"];//:360.00,
float winddir_10m = (float) readvalues["winddir_10m"];//:360.00,
float windgust_10m = (float) readvalues["windgust_10m"];//:999.99
temp_offset = (float) readvalues["temp_offset"];
hum_offset = (float) readvalues["hum_offset"];


  // Real world application would store these values in some variables for later use.
if (debug_FS) {  
              Serial.print(F("Loaded rainc: "));
              Serial.println(raincount);
              Serial.print(F("Loaded rainc1h: "));
              Serial.println(raincount1h);        
              Serial.print(F("Loaded baroaverage: "));
              Serial.print(baroaverage);
              Serial.print("/");
              Serial.println(baro_eu_avg.getAverage());
              Serial.print(F("Loaded tempaverage: "));
              Serial.print(tempaverage);
              Serial.print("/");
              Serial.println(temp_avg10m.getAverage());

              
              Serial.print(F("Loaded windspeed_2m : "));
              Serial.print(windspeed_2m );
              Serial.print("/");
              Serial.println(windspeed_eu_avg2m.getAverage());
              Serial.print(F("Loaded windsdir_2m : "));
              Serial.print(windsdir_2m );
              Serial.print("/");
              Serial.println(winddir_eu_avg2m.getAverage());
              Serial.print(F("Loaded winddir_10m : "));
              Serial.print(winddir_10m );
              Serial.print("/");
              Serial.println(winddir_eu_avg10m.getAverage());
              Serial.print(F("Loaded windgust_10m : "));
              Serial.print(windgust_10m );
              Serial.print("/");
              Serial.println(windgust_eu_avg10m.getAverage());              
              }
  return true;
}


//**************************************
bool saveConfig() {
  
/*    
 *     
  savedvalues["baroaverage"] = (String) 1000.00;
  savedvalues["tempaverage"] = (String)19.99;
  savedvalues["raincount"] = (String)99.99;
  savedvalues["raincount1h"] = (String)99.99;
  savedvalues[ "windspeed_2m"] = (String)999.99;
  savedvalues["winddir_2m"] = (String)360.00;
  savedvalues["winddir_10m"] = (String)360.00;
  savedvalues["windgust_10m"] = (String)999.99;
*/ 
  File configFile = LittleFS.open("/savedvals.json", "w");
  if (!configFile) {
    Serial.println(F("Failed to open file for writing"));
    return false;
  }

  serializeJson(savedvalues, configFile);
  if (debug_FS) {  
                serializeJson(savedvalues, Serial);
                Serial.println();
                }
String mqttpayload;                
serializeJson(savedvalues, mqttpayload);
mqttclient_pub("tele/"+mqtt_ID+"/INFO2",mqttpayload, false);  
return true;
}

bool clearConfig() {

  savedvalues["baroaverage"] = (String) 1000.00;
  savedvalues["tempaverage"] = (String)19.99;
  savedvalues["raincount"] = (String)99.99;
  savedvalues["raincount1h"] = (String)99.99;
  savedvalues[ "windspeed_2m"] = (String)999.99;
  savedvalues["winddir_2m"] = (String)360.00;
  savedvalues["winddir_10m"] = (String)360.00;
  savedvalues["windgust_10m"] = (String)999.99;
 
  File configFile = LittleFS.open("/savedvals.json", "w");
  if (!configFile) {
    Serial.println(F("Failed to open file for writing"));
    return false;
  }

  serializeJson(savedvalues, configFile);
  if (debug_FS) {  
                serializeJson(savedvalues, Serial);
                Serial.println();
                }
  
return true;
}
