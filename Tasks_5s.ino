void Tasks5s(void) {
       int dirwind_temp;
     //Call speedgust() to store actual wind gust
     float gust5stmp=speedgust();
     Serial.print(build_timestamp(2));
     Serial.print(F("T:"));    
     Serial.print(tempout_eu);    
     Serial.print(F("A:"));    
     Serial.print(temp_avg);    
     Serial.print(F("("));    
     Serial.print(temptrend);    
     Serial.print(F(")-"));    
     Serial.print(F("G5s:"));    
     Serial.print(gust5stmp);    
     Serial.print(F(","));
     Serial.print(dirwind_temp);
     Serial.print(F("/"));
     mqttclient_pub("home/garden/wind/gust_5s",(String) gust5stmp,false);
     dirwind_temp = DirWind();
     winddir_sum = dirwind_temp +winddir_sum;
     //Add actual wind direction to average after 60sec
     mqttclient_pub("home/garden/wind/dir_5s",(String) dirwind_temp,false);
     Wind_Direction_cardinal(dirwind_temp);
     mqttclient_pub("home/garden/wind/dirC_5s",(String) Wind_DirC,false);
     Serial.print(Wind_DirC);
    
//     humidity_sum = humidity_sum + ( bme.readHumidity() + humi_offset );
//     baro_sum = baro_sum + ( bme.readSealevelPressure(altitudepws)/100.00f );
//    baro_sum = baro_sum + (bme.readPressure()/100.00f);
    
    sensor_count = sensor_count + 1.00f;

     if (ReadI2C());
         {
         tempout_sum = tempout_sum + (BMP1.TempC + temp_offset );
// Uncomment if using BME Humidity Sensor
         humidity_sum = humidity_sum + (BMP1.Humidity + hum_offset);
         baro_sum = baro_sum + (BMP1.Pressure/100.00f);
  //     baro_sum = baro_sum ;
         uvindex_sum = uvindex_sum + UV1.UVindex;
         uvviz_sum = uvviz_sum + UV1.Vis;    
         }

        Sensors_Debug();
     
     if(avg2m) {
      Serial.print(F("-S2m:"));
      Serial.print(windspd_avg2m);
      Serial.print(F(","));
      Serial.print(winddir_avg2m );
      Serial.print(F("/"));
      Wind_Direction_cardinal((int)winddir_eu_avg2m.getAverage());
      Serial.print(Wind_DirC);
/*
    windspeed_eu_avg2m.getAverage()
    winddir_eu_avg2m.getAverage()
    winddir_eu_avg10m.getAverage()
    windgust_eu_avg10m.getAverage()
 * 
 */
      
      }
     if(avg10m) {
      Serial.print(F("-G10m:"));
      Serial.print(windgust_avg10m);
      Serial.print(F(","));
      Serial.print(winddir_avg10m);
      Serial.print(F("/"));
      Wind_Direction_cardinal((int)winddir_eu_avg10m.getAverage());
      Serial.print(Wind_DirC);
        
      };
     Serial.println(F(""));

}
