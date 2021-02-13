// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;

String conv_dp(float floatvalue,int dp) {

  char str[6];     //result string 5 positions + \0 at the end
//
// convert float to fprintf type string
// format 5 positions with dp decimal places
//
dtostrf(floatvalue, 4, dp, str );
//
// str contains the result. 
//
  return String(str);
}

/////////////////////////////////////////////////////////////////////////////////////////////
void ThingTweetSend()
{
String squalk = build_timestamp(2);
squalk += "T " + conv_dp(BMP1.TempC + temp_offset,1 )+"C ("+ (String)temptrend+"), ";
if (baro_eu > 0) {
squalk += "Baro " + conv_dp(baro_eu,0) +"hPa ("+ (String)barotrend+"), "; 
}
squalk += ""+conv_dp(windspeed_eu,0);
squalk += "mph " +(String) Wind_DirC;
squalk += ",Rain "+conv_dp(rain24h_eu,0)+"mm ";
squalk += "@jiberjaber #Arduino #Chelmsford #Weather";

//"Current weather at"+build_timestamp(2)+" Temp " + String(BMP1.TempC + temp_offset ) +"C, Dew " + String(BMP1.DewPoint)+ "C, Hum "+String(BMP1.Humidity)+"%, Baro "+baro_eu+ "hPa ("+barotrend+"), Wind "+windspeed+"mph " +(String) Wind_DirC ;
//squalk += ", Rain today "+String(rain24h_eu)+"mm @jiberjaber #MoulshamLodge #Chelmsford #Weather";

 updateTwitterStatus(squalk);
/*  
 
  10:00: Morning; Cloudy/Recent rain, Temp 2.2°C, (min o/night 0.5°C), Dew 2.2°C, Hum 100%, Baro 995.7hPa (Rising rapidly), Wind 4.1mph WNW, Rain today  2.8mm
 if (baro_eu == ave_baro_eu) {barotrend = "Steady";};

Tweeted:  15:03:Current Condx: Temp 3.59Âø°°°°C, Dew3.59ÂÂ°C, Hum 100.00%, Barohnd 24.07 mph NNW,Rain today 4.80mm @jiberjaber #Chelmsford #Weather #test
*/                    
}


/////////////////////////////////////////////////////////////////////
void updateTwitterStatus(String tsData)
{
if (!TweetEnable) {
  Serial.println(F("Twitter Service not enabled"));
  return;
}

  if (client.connect(ThingSpeakServer, 80))
  { 
    // Create HTTP POST Data
    tsData = "api_key="+thingtweetAPIKey+"&status="+tsData;
            
    client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
    
    lastConnectionTime = millis();
    
    if (client.connected())
    {
      Serial.print(F("Tweeted: "));
      Serial.println(tsData);
      failedCounter = 0;
    }
    else
    {
      failedCounter++;
  
      Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
    }
    
  }
  else
  {
    failedCounter++;
    
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
  }
client.stop();
}
