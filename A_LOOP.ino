void loop(){
 ArduinoOTA.handle();
 mqttclient.loop();
/*
if (FTPEnable) {   
  ftpSrv.handleFTP();        //make sure in loop you call handleFTP()!!  
  }
*/
//None Blocking MQTT reconnect every 20 seconds
  if (millis() - lastReconnectAttempt > 20000) 
  {
  // Attempt to reconnect
  if (!mqttclient.connected()) 
    {
    if (reconnect()) {
        lastReconnectAttempt = 0;
    }
    }
    lastReconnectAttempt = millis();
  } 

//Check we have a NTP update, if so, back off update frequency  
if(!ntp_OK) if (timeStatus()>1 ) {
        log(NTP_success);
        sync_rtc();
        log(I2C_RTCsync);
        setSyncInterval(1807); //every 30+ mins only do this once per boot else it will reset the timer each time
        Serial.println(F("NTP interval increased, RTC synced to NTP"));
        ntp_OK = 1;  //toggle so we dont reset the NTP auto timer
        digitalWrite(LED, 1);
        }
        
if (millis()-ntp_currentMillis >=  ntp_interval)
    {
      if (timeStatus()<2) {
        Serial.println(F("NTP call fail detected"));
        setSyncInterval(23); //only do this once per boot else it will reset the timer each time
        digitalWrite(LED, 0);
        ntp_OK = 0;
        log(NTP_fail);
        }
    ntp_currentMillis = millis();
    }
// 5 Second task interval
 if ( (millis() - count5sec) >= 5000)
    {
    Tasks5s();        
    count5sec = millis();
    }
// 60 Second task interval    
  if ( (millis() - count60sec) >= 60000)
    { 
    temp_counter++;  
     Tasks60s();
     count5sec = millis(); //reset wind gust 5sec counter
     count60sec = millis();
     //ESP.restart();
    }

    // 5 min task interval    
  //if ( (millis() - count5min) >= 300000)
  //if ( (millis() - count5min) >= 110000)
  if ( (millis() - count5min) >= 60000) //1 min
    { 
     Serial.println(F("WOW Send"));
  //   wowsenddata();
     count5min = millis();
     
    }

// 92 minute task interval
//about every 92m
  if ( (millis() - count10min) >= (5520000))
    { 
      Serial.println(F("**********************************************************************************************"));
      ThingTweetSend();
      Serial.println(F("**********************************************************************************************"));
      count10min = millis();
    }

// Hourly task interval
  if ( ((millis() - count1h) >= (60000*60*1)) && (raincount1h != 0))
    {
     Serial.println(F("Reset hourly rain each hour"));  
     raincount1h = 0;
     rain1h_eu = 0.00;
    }
// Weekly task interval
  if ( millis() >= (60000*60*24*3))
    {
     Serial.println(F("task each week"));    
     
     ESP.restart();
    }
 
 // 5 Second task interval
if(Calibration){
  if ( (millis() - count15sec) >= 15000)

    {
    //Do 15sec tasks here
              HumCal["raw"] =  round2(BMP1.Humidity);
              HumCal["offset"] = hum_offset;
              HumCal["corrected"] =  round2(BMP1.Humidity + hum_offset);
                   
              TempCal["raw"] =  round2(BMP1.TempC);
              TempCal["offset"] = temp_offset;
              TempCal["corrected"] =  round2(BMP1.TempC + temp_offset);    
    SendCalMQTT();  //Send calibration values
    count15sec = millis();
    }
}
 
 }
