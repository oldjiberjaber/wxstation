
/*
 * 
 * 
 * {"Time":"2018-02-09T10:26:02","Uptime":"0T16:55:14","Vcc":3.466,"POWER":"OFF","Wifi":{"AP":1,"SSId":"Jiberjaber_cm","RSSI":72,"APMac":"04:F0:21:23:31:14"}}
 * 
 * 
 */
void StatusMsg()
{
String statresult;
if (!mqttclient.connected()) 
    {
      if (reconnect()) 
      {
          lastReconnectAttempt = 0;
      }
    }
    lastReconnectAttempt = millis();
stateResult["Version"] = SW_VERSION;
stateResult["Time"] = build_timestamp(0);
stateResult["Uptime"] = build_uptime();
stateResult["UptimeSec"] = uptimeclock;
stateResult["Heap"] = ESP.getFreeHeap();
//stateResult["SleepMode"] = "n/a";
//stateResult["Sleep"] = 0;
//stateResult["LoadAvg"] = 0;
stateResult["MqttCount"] = MqttCount;

Wifi["AP"] = 1;
Wifi["SSId"] =  ssid;
Wifi["BSSId"] = WiFi.softAPmacAddress();
Wifi["RSSI"] = abs(WiFi.RSSI());
Wifi["Signal"] = WifiGetRssiAsQuality(WiFi.RSSI());  //Signal Quality 0% (bad) - 100% (good)
Wifi["Channel"] = WiFi.channel();
//Wifi["LinkCount"] = 1; //number of Wifi reconnects (not used, we reset on disconnect so set to 1)
//Wifi["Downtime"] = "0T00:00:01"; //duration of uptime without wifi signal(not used, we reset on disconnect so set to 1)

serializeJson(stateResult, statresult);
Serial.println(statresult);  
mqttclient_pub("tele/"+mqtt_ID+"/STATE",statresult);

}
