

boolean reconnect() {
 String MQTTstrbuffer;
 char MQTTmsg[50];
  Serial.print(F("Attempting MQTT connection..."));
  String lwtTopic = "tele/"+mqtt_ID+"/LWT";
  char MQTTtopic[lwtTopic.length()+1];
  lwtTopic.toCharArray(MQTTtopic,lwtTopic.length()+1); 
 
  if (mqttclient.connect(WIFIID, "22", "22", MQTTtopic, 1, true, "Offline")) 
  {
     // Once connected, publish an announcement...
      mqttclient.publish(MQTTtopic,"Online",1);
      Serial.print(MQTTtopic);Serial.println(" Online");
      
      // ... and resubscribe

      mqttclient.subscribe(calibration_target_topic.c_str());
      mqttclient.subscribe("home/garden/pressure/#");
       
  } else
  {
   Serial.print(F("failed, rc="));
   Serial.print(mqttclient.state());
   Serial.println(" try again later!");
  }
return mqttclient.connected();
}


// handles message arrived on subscribed topic(s)
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  char message_buff[100];
  int i = 0;

  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
    }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
  
if (MQTTdebug) {
  Serial.print("Message arrived:  topic: " + String(topic));
  Serial.print("  Length: " + String(length,DEC));
    Serial.println("  Payload: " + msgString);
}

 //Have we got temperature calibration?  
   if (strcmp(topic,calibration_target_topicT.c_str())==0) {    
    if (debug) { Serial.print(F("Calibration Offset received: "));Serial.print(msgString);Serial.println(F("°C"));}    
    if(msgString.toFloat() >= 99) {Serial.println(F("Leaving Calibration Mode"));Calibration = false;calibration["CalMode"] = Calibration;SendCalMQTT();  }
    else 
    {
      Serial.println(F("Entering Calibration Mode"));
      calibration["CalMode"] = "T";
      if(Calibration) {eepromctr=19;}
      Calibration = true;
      temp_offset = msgString.toFloat();
    }
  };

//Have we got Humidity calibration?  
   if (strcmp(topic,calibration_target_topicH.c_str())==0) {    
    if (debug) { Serial.print(F("Calibration Offset received: "));Serial.print(msgString);Serial.println(F("%"));}    
    if(msgString.toFloat() >= 999) {Serial.println(F("Leaving Calibration Mode"));Calibration = false;calibration["CalMode"] = Calibration; SendCalMQTT();  }
    else 
    {
      Serial.println(F("Entering Calibration Mode"));
      calibration["CalMode"] = "H";
      if(Calibration) {eepromctr=19;}
      Calibration = true;
      hum_offset = msgString.toFloat();
    }
  };

  if (strcmp(topic,"home/garden/pressure/avg3h")==0) {     
    if (MQTTdebug) {Serial.print("Pressure 3h ave is ");Serial.println(msgString);}
    //Pressure_avg10m = msgString;     
    }; 
  
  if (strcmp(topic,"home/garden/pressure/current")==0) {     
    if (MQTTdebug) {Serial.print("Pressure is ");Serial.print(msgString);Serial.println("hPa");}
    //Pressure = msgString;     
    baro_eu = msgString.toFloat();
    Serial.print("Baro is ");Serial.println(baro_eu);
    };
}



void mqttclient_pub(String MQTTTOPIC, String MQTTMSG, boolean mqttdebug = true)
{
  if (!client.connected()) {
      reconnect(); 
      }
  char MQTTmsg[MQTTMSG.length()+1];
  char MQTTtopic[50];
  MQTTMSG.toCharArray(MQTTmsg,MQTTMSG.length()+1); 
  MQTTTOPIC.toCharArray(MQTTtopic,50); 
  if(mqttclient.publish(MQTTtopic,MQTTmsg,false))  //do not publish retained message so all can see teh latest values
    {
    MQTTTOPIC += "=";
    MQTTTOPIC += MQTTMSG; 
    MqttCount++;
    } else
    {
    MQTTTOPIC += " failed to publish!";  
    }  
    if (mqttdebug) {Serial.println(MQTTTOPIC);}
  
}

void mqtt_pub(){
  StatusMsg(); //Send Sonoff style status message
  mqttclient_pub("home/garden/wind/dir_eu",(String) winddir_eu,true);
  Wind_Direction_cardinal(winddir_eu);
  mqttclient_pub("home/garden/wind/dirC",(String) Wind_DirC,true);
     
if (avg2m)
      {
      mqttclient_pub("home/garden/wind/speed_2m",windspd_avg2m,true); mqttclient_pub("home/garden/wind/dir_2m",winddir_avg2m,true);
      }
if (avg10m)
      {
      mqttclient_pub("home/garden/wind/gust_10m",windgust_avg10m,true); mqttclient_pub("home/garden/wind/dir_10m",winddir_avg10m,true);
      } 

  mqttclient_pub("home/garden/wind/speed",windspeed);
  mqttclient_pub("home/garden/wind/gust",windgust);
  
    if (DSPresent) {
                    mqttclient_pub("home/garden/TempC",String(BMP1.TempC + temp_offset ));
                    mqttclient_pub("home/garden/dewpoint_eu",String(BMP1.DewPoint));  
                    mqttclient_pub("home/garden/Humidity",String(BMP1.Humidity+hum_offset));    
                    
//MQTT WeeWX Transmission
                    WeeWX["barometer"] = round2(baro_eu);
                    WeeWX["outTemp"] = round2(BMP1.TempC + temp_offset);
                    WeeWX["outHumidity"] = round2(BMP1.Humidity+hum_offset);
                    WeeWX["windSpeed"] = round2(windspeed_eu);
                    WeeWX["windDir"] = (int) winddir_eu;
                    WeeWX["windGust"] = round2(windgust_eu);
                    WeeWX["windGustDir"] = (int) winddir_eu;
                    WeeWX["rain"] = round2(WeeWX_RainAmount*0.2f);
//                    WeeWX["rain"] = rain24h_eu;
                    WeeWX["rainRate"] = rain1h_eu;
                    //WeeWX["RSSI"] = WiFi.RSSI()*-1;
                    WeeWX["RSSI"] = abs(WiFi.RSSI());
                    WeeWX["signal"] = WifiGetRssiAsQuality(WiFi.RSSI());
                   // WeeWX["signal"] = 150 - (5/3) * abs(WiFi.RSSI());
                   
                    String WeeWXString;
                    serializeJson(WeeWX, WeeWXString);
                    mqttclient_pub("home/WeeWX", WeeWXString, false);
                    //Reset observation rain counter (pulses)
                    WeeWX_RainAmount = 0;

                    //MQTT ENV Transmission
                    env["temp"] = round2(BMP1.TempC + temp_offset);
                    env["hum"] = round2(BMP1.Humidity);
                    env["tempraw"] = round2(BMP1.TempC);
                    String envString;
                    serializeJson(env, envString);
                    mqttclient_pub("home/env/garden", envString, false);
                    }       ;

  //Rain data
  mqttclient_pub("home/garden/rain/1h_eu",String(rain1h_eu)); 
  mqttclient_pub("home/garden/rain/24h_eu",String(rain24h_eu));      
}
