void SendCalMQTT(void) {
  HumCal["raw"] =  round2(BMP1.Humidity);
  HumCal["offset"] = hum_offset;
  HumCal["corrected"] =  round2(BMP1.Humidity + hum_offset);
                   
  TempCal["raw"] =  round2(BMP1.TempC);
  TempCal["offset"] = temp_offset;
  TempCal["corrected"] =  round2(BMP1.TempC + temp_offset);    
  
  serializeJson(calibration, Serial);
  Serial.println();
  String mqttpayload;                
  serializeJson(calibration, mqttpayload);
  mqttclient_pub("tele/"+mqtt_ID+"/INFO3",mqttpayload, false); 
}

double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}

void Sensors_Debug() {
  if (Sensors_debug) {
           Serial.println(F("Other Sensor each 5sec: "));
           tempout_eu = tempout_sum / sensor_count;
           humidity_eu = humidity_sum / sensor_count;
           dewpout_eu = ( tempout_eu - ((100.00f - humidity_eu)/5.00f) );
           baro_eu = baro_sum / sensor_count;
           winddir_eu = winddir_sum / sensor_count;
           uvindex_eu = uvindex_sum / sensor_count;
           uvviz_eu = uvviz_sum / sensor_count;
           Serial.print(F("Sensor Count: "));
           Serial.println(sensor_count);
           
           Serial.print(F("Temp Sum: "));
           Serial.print(tempout_sum);
           Serial.print(F(" Temp: "));
           Serial.println(tempout_eu);
           
           Serial.print(F("Dew Point: "));
           Serial.print(dewpout_eu);
           Serial.print(F(" Humidity: "));
           Serial.println(humidity_eu);
           
           Serial.print(F("Pressure Sum: "));
           Serial.print(baro_sum);
           Serial.print(F(" Pressure: "));
           Serial.println(baro_eu);
           Serial.print(F("Wind DirSum: "));
           Serial.print(winddir_sum);
           Serial.print(F(" Wind Dir: "));
           Serial.println(winddir_eu);
           Serial.print(F("Solar Viz Sum: "));
           Serial.print(uvviz_sum);
           Serial.print(F(" Solar Rad: "));
           Serial.println(uvviz_eu);
           Serial.print(F("UV Sum: "));
           Serial.print(uvindex_sum);
           Serial.print(F(" UV Index: "));
           Serial.println(uvindex_eu);
          }
}
long int check_valid(void)
{
 //DateTime now = rtc.now();
  myclock = rtc.now();
  return myclock.unixtime();
}

String build_timestamp(int wu)
{
 //DateTime now = rtc.now();
 myclock = rtc.now();
 String logt = " ";
 if (myclock.unixtime() == 1696376785) { 
                                   Serial.println(F("Could not find a valid DS3231 RTC, using on-board time!")); 
                                   //build timestamps out of on-board RTC which should be running from NTP - send 'now' for wunderground timestamp 
                                     }
switch(wu){
  case 0:  //standard log time //Status Message version YYYY-MM-DDTHH:MM:SS eg: 2018-02-09T10:26:02
             logt = String(myclock.year())+ "-";
             if (myclock.month() <= 9)  logt += "0" + String(myclock.month())  + "-";   else logt += String(myclock.month()) + "-";
             if (myclock.day() <= 9)    logt += "0" + String(myclock.day())    + "T";   else logt += String(myclock.day()) + "T";
             if (myclock.hour() <= 9)   logt += "0" + String(myclock.hour())   + ":";   else logt += String(myclock.hour()) + ":";
             if (myclock.minute() <= 9) logt += "0" + String(myclock.minute()) + ":";   else logt += String(myclock.minute()) + ":";
             if (myclock.second() <= 9) logt += "0" + String(myclock.second())      ;   else logt += String(myclock.second());
             return logt;
             break;
  case 1: // Weather underground version [YYYY-MM-DD HH:MM:SS (mysql format)] In Universal Coordinated Time (UTC) Not local time eg: 2018-02-09+09%3A53%3A54
             if (timeStatus()<1) return "now";;
             logt = String(year())+ "-";
             if (month() <= 9)  logt += "0" + String(month())  + "-";   else logt += String(month()) + "-";
             if (day() <= 9)    logt += "0" + String(day())    + "+";   else logt += String(day()) + "+";                                   
             if (myclock.hour() <= 9)   logt += "0" + String(myclock.hour())   + "%3A";   else logt += String(myclock.hour()) + "%3A";
             if (myclock.minute() <= 9) logt += "0" + String(myclock.minute()) + "%3A";   else logt += String(myclock.minute()) + "%3A";
             if (second() <= 9) logt += "0" + String(second())      ;   else logt += String(second());                                  
             return logt;
             break;
  case 2: //tweet version             
             if (myclock.hour() <= 9)   logt += "0" + String(myclock.hour())   + ":";   else logt += String(myclock.hour()) + ":";
             if (myclock.minute() <= 9) logt += "0" + String(myclock.minute()) + ":";   else logt += String(myclock.minute()) + ":";
             
             return logt;
             break;   
          }
 return logt;

}

String build_uptime(void)
{
 //bootclock is the time we booted in Unixtime format
 uptimeclock = rtc.now().unixtime();
 uptimeclock = uptimeclock - boottime;
 TimeElements up_clock;
 breakTime(uptimeclock, up_clock);
 
 String uptime_str = "";
 
  //Status Message version DDTHH:MM:SS eg: 09T10:26:02
             uptime_str += String(up_clock.Day-1)  + "T";
             uptime_str += String(up_clock.Hour)   + ":";
             uptime_str += String(up_clock.Minute) + ":";
             uptime_str += String(up_clock.Second)      ;
             return uptime_str;
}











// call this when we have a valid NTP sync
void sync_rtc() 
{
  
  rtc.adjust(DateTime(year(), month(), day(), hour(), minute(), second())); 
 
  
}

 
 
time_t get_rtc()
{
 // DateTime now = rtc.now();
  myclock = rtc.now();
 Serial.println();
  Serial.print(myclock.year(), DEC);
    Serial.print('/');
    Serial.print(myclock.month(), DEC);
    Serial.print('/');
    Serial.print(myclock.day(), DEC);
    Serial.print(F(" "));
    Serial.print(myclock.hour(), DEC);
    Serial.print(':');
    Serial.print(myclock.minute(), DEC);
    Serial.print(':');
    Serial.print(myclock.second(), DEC);
    Serial.println();

  tmElements_t tmSet;
  tmSet.Year = myclock.year() - 1970;
  tmSet.Month = myclock.month();
  tmSet.Day = myclock.day();
  tmSet.Hour = myclock.hour();
  tmSet.Minute = myclock.minute();
  tmSet.Second = myclock.second();
  return makeTime(tmSet);         //convert to time_t
//  return clock.unixtime();
}


/*
 * 
 * 
 * 
 */

//---------------------------------------------------------NTP request----------------------------------------------

unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println(F("sending NTP packet..."));
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

//---------------------------------------------------------NTP Time----------------------------------------------

unsigned long ntptime()
{
  WiFi.hostByName(ntpServerName, timeServerIP); 
  sendNTPpacket(timeServerIP);
  delay(1000);
  
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println(F("no NTP packet yet"));
  }
  else {
    Serial.print(F("NTP packet received, length="));
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    // now convert NTP time into everyday time:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    epoch = secsSince1900 - seventyYears;

    if (debug) {
    
      Serial.print(F("Seconds since Jan 1 1900 = "));
      Serial.println(secsSince1900);
  
      // print Unix time:
      Serial.print(F("Unix time = "));
      Serial.println(epoch);
  
  
      // print the hour, minute and second:
      Serial.print(F("The local time (UTC) is "));        // UTC, to use -4 by (epoch-(3600*4))
      Serial.print(((epoch)  % 86400L) / 3600); // print the hour (86400 equals secs per day)
      Serial.print(':');
      if ( (((epoch) % 3600) / 60) < 10 ) {
        // In the first 10 minutes of each hour, we'll want a leading '0'
        Serial.print('0');
      }
      Serial.print(((epoch)  % 3600) / 60); // print the minute (3600 equals secs per hour)
      Serial.print(':');
      if ( ((epoch) % 60) < 10 ) {
        // In the first 10 seconds of each minute, we'll want a leading '0'
        Serial.print('0');
      }
      Serial.println((epoch) % 60); // print the second
    }
  }
}

int localhour()
{
  return (((epoch)  % 86400L) / 3600);
}

int localmin()
{
  return (((epoch)  % 3600) / 60);
}


int localsec()
{
  return ((epoch) % 60);
}
