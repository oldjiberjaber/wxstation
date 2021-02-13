void Tasks60s(void) {
  Serial.println(F(""));
     Serial.println(F("Store and convert all sensor value fo WU each 60sec"));

     //reset Daily Rain each 24h
     if ((myclock.hour() >= 23) && (myclock.minute() >= 55))
     {
      Serial.println(F("Reset Daily Rain"));
      raincount = 0; 
      rain24h_eu = 0.00;
     }

     //get all value of sensor
     winddir_eu = (int) winddir_sum / sensor_count;
     windspeed_eu = speedwind();
     
     //wind gust for 60sec 
     windgust_eu = tempwindgust;
     tempout_eu = tempout_sum / sensor_count;
// Uncomment if using BME Humidity Sensor
     humidity_eu = humidity_sum / sensor_count;
// Uncomment if using BME Humidity Sensor
     dewpout_eu = ( tempout_eu - ((100.00f - humidity_eu)/5.00f) );
// Uncomment if using BME Humidity Sensor
//   BMP1.DewPoint = dewpout_eu;
//MQTT derived baro_eu
//baro_eu = baro_sum / sensor_count;
     uvindex_eu = uvindex_sum / sensor_count;
     uvviz_eu = uvviz_sum / sensor_count; 
//     rain1h_eu = 0.80f * raincount1h;
//     rain24h_eu = 0.800f * raincount;
//Davis Rain Guage is 0.2mm per activation
     rain1h_eu = 0.20f * raincount1h;
     rain24h_eu = 0.200f * raincount;
//1mm = 0.0393701inch
//0.2mm = 0.00787402inch          
     
     //make conversion to US for Wunderground
     
     //windspeed = windspeed_eu * 0.62138f;
     //windgust = windgust_eu * 0.62138f;
     windspeed = windspeed_eu ;
     windgust = windgust_eu ;
     winddir = winddir_eu;
     tempout = (( tempout_eu * 1.8 ) + 32);
// Uncomment if using BME Humidity Sensor
     dewpout =  (( dewpout_eu * 1.8 ) + 32); 
 

     humidity = humidity_eu;

     baro = 0.02952998751 * baro_eu;
     
     rain1h = rain1h_eu / 25.40 ;
     rain24h = rain24h_eu / 25.40 ;

    windspeed_eu_avg2m.addValue(windspeed_eu);
    winddir_eu_avg2m.addValue(winddir_eu);
    winddir_eu_avg10m.addValue(winddir_eu);
    windgust_eu_avg10m.addValue(tempwindgust);
    baro_eu_avg.addValue(baro_eu);
    temp_avg10m.addValue(tempout_eu);

    if (averagecounter>=2) { //2m average values
      avg2m = true; 
      windspd_avg2m = windspeed_eu_avg2m.getAverage();
      winddir_avg2m = (int) winddir_eu_avg2m.getAverage();
      }

    if (averagecounter>=10) //10m average values
    { 
     avg10m = true; 
     windgust_avg10m = windgust_eu_avg10m.getAverage();
     winddir_avg10m = (int) winddir_eu_avg10m.getAverage(); 
    }

     temp_avg = temp_avg10m.getAverage();
      //Trending DOWN 
     if (tempout_eu > temp_avg) {temptrend = "Rising";};
      //Trending UP  
     if (tempout_eu < temp_avg) {temptrend = "Falling"; };
      //Trending STEADY 
     if (tempout_eu == temp_avg) {temptrend = "Steady";};
//     Serial.print(F("Current Temp ="));Serial.print(tempout_eu);Serial.print(F(" current ave ="));
//     Serial.print(temp_avg); Serial.print(F(" Trend =")); Serial.println(temptrend);
     
     //**************************************************
     baro_avg3h = baro_eu_avg.getAverage(); 
     //ave_baro_eu = (int) baro_eu_avg.getAverage(); 
     ave_baro_eu = baro_eu_avg.getAverage(); 
      //Trending DOWN 
     if (baro_eu > ave_baro_eu) {barotrend = "Rising";};
      //Trending UP  
     if (baro_eu < ave_baro_eu) {barotrend = "Falling"; };
      //Trending STEADY 
     if (baro_eu == ave_baro_eu) {barotrend = "Steady";};
//     Serial.print(F("Current baro ="));Serial.print(baro_eu);Serial.print(F(" current ave ="));
//     Serial.print(ave_baro_eu); Serial.print(F(" Trend =")); Serial.println(barotrend);
     
  Serial.println(F("Send Data to WU each 60sec"));


  savedvalues["baroaverage"] = round2(ave_baro_eu);
  savedvalues["tempaverage"] = round2(temp_avg);
  savedvalues["raincount"] = round2(raincount);
  savedvalues["raincount1h"] = round2(raincount1h);
  savedvalues[ "windspeed_2m"] = round2(windspeed_eu_avg2m.getAverage());
  savedvalues["winddir_2m"] = round2(winddir_eu_avg2m.getAverage());
  savedvalues["winddir_10m"] = round2(winddir_eu_avg10m.getAverage());
  savedvalues["windgust_10m"] = round2(windgust_eu_avg10m.getAverage());
  savedvalues["temp_offset"] = round2(temp_offset);                       
  savedvalues["hum_offset"] = round2(hum_offset);      

  //STORE IN EEPROM
    if (eepromctr > 9) {
                        Serial.println(F("Save Values"));
                        saveConfig();
                        eepromctr=0;
                        }
                        else {                                           
                            Serial.println(F("EEPROM Count: "));         
                            Serial.print(eepromctr);         
                            Serial.println(F(" - EEPROM NOT WRITTEN"));         
                            eepromctr++;
                             }
                        
              

//END - STORE IN EEPROM
/*
  //STORE RAINCOUNT IN EEPROM
  Serial.println(F("SET EEPROM"));
  eepromstring = String(raincount,2);
  eepromSet("raincount", eepromstring);
  eepromstring = String(raincount1h,2);
  eepromSet("raincount1h", eepromstring);
  eepromstring = String(ave_baro_eu,2);
  eepromSet("baroavg", eepromstring);
  eepromstring = String(temp_avg,2);
  eepromSet("tempavg", eepromstring);
  
  //END - STORE RAINCOUNT IN EEPROM
*/
// Commented out below for RTC testing!
//Send data to wunderground

     if (WUEnable){ senddata();
                  } else {
                  Serial.println(F("WUnderground Service not enabled"));                  
                  }

//Send data to Thingspeak
//     ThingSpeakSend();
     
     if (MQTTEnable) {
                     Serial.println(F("Publish to MQTT Server"));
                     mqtt_pub();
                  } else {
                  Serial.println(F("MQTT Publish Service not enabled"));                  
                  }
              
              SendCalMQTT();  //Send calibration values
      /**********************************************************************************************************************/
     
     tempwindgust = 0;
     winddir_sum = 0.00f;
     tempout_sum = 0.00f;
     humidity_sum = 0.00f;
     baro_sum = 0.00f;
     uvindex_sum = 0.00f;
     uvviz_sum = 0.00f;
     sensor_count = 0.00f;
     pulsesgust = 0; //reset wind gust data
     averagecounter++;

}
