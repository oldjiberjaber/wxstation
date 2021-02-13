
const char* ThingSpeakServer = "api.thingspeak.com";

void ThingSpeakSend()
{
  if (client.connect(ThingSpeakServer,80)) 
    { // "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(winddir_eu);
    postStr +="&field2=";
    postStr += String(windspeed_eu);
    postStr +="&field3=";
    postStr += String(windgust_eu);
    
    if (DSPresent)
      {
        postStr +="&field4=";
        getOneWireTemp();
        postStr += String((BMP1.TempC + temp_offset ));      
      }
      if (BMP1.Present)
      {
        postStr +="&field5=";
        postStr += String(BMP1.Pressure/100);
        postStr +="&field6=";
        postStr += String(BMP1.counter);
      }
    if (UV1.Present)
      {
        postStr +="&field7=";
        postStr += String(UV1.UVindex);
        
      }
    if (BMP1.Present)
      {
// Uncomment if using BME Humidity Sensor
        postStr +="&field8=";     postStr += String(BMP1.Humidity);
      }
    postStr += "\r\n\r\n";
    
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    
    Serial.print(F("Temperature: "));
    Serial.print(BMP1.TempC);     Serial.print(F(" deg C"));
// Uncomment if using BME Humidity Sensor
    Serial.print(F(" Humidity: "));   Serial.print(BMP1.Humidity);
    Serial.print(F(" Pressure (Pa): "));
    Serial.println(BMP1.Pressure);
    Serial.println(F("sent to Thingspeak"));
    }
    client.stop();
    
}
