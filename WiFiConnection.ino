void WIFI_connection(void)
{ 
//WIFI_OFF will disable WiFi completely.
//WIFI_STA will disable the AP but allow the esp to operate as a station.
//default to WIFI_AP_STA and act as both.

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);  //Start trying to connect to WiFi
  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array); ++i){ sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]); }
         ID_MAC = mqtt_ID;
        ID_MAC += "=";
        ID_MAC += MAC_char; 
        
  Serial.println("");
  Serial.print(F("Attempting connection to "));Serial.println(ssid);
  unsigned int current_millis_wifi = millis();
  
  while (WiFi.status() != WL_CONNECTED) 
  { 
    //Reboot if no connection in 10 seconds
    if (millis()-current_millis_wifi >=  40000) { Serial.println(F("Connection Failed! Rebooting...")); ESP.restart();};
    // Wait for connection, print '.' for each 500ms waiting
    delay(500); Serial.print(F("."));
  }
  
  // We've connected, announce it
  Serial.println("");
  Serial.print(F("Connected to "));Serial.println(ssid);
  Serial.print(F("IP address: ")); Serial.println(WiFi.localIP());
  Serial.print(F("RSSI: ")); Serial.println(WiFi.RSSI());

if (MDNS.begin(WIFIID)) {Serial.println(F("MDNS responder started"));}
//if (MDNS.begin("esp8266DS")) {Serial.println(F("MDNS responder started"));}

Serial.println(F("Hello World"));
  

}

int WifiGetRssiAsQuality(int rssi)
{
  int quality = 0;

  if (rssi <= -100) {
    quality = 0;
  } else if (rssi >= -50) {
    quality = 100;
  } else {
    quality = 2 * (rssi + 100);
  }
  return quality;
}
