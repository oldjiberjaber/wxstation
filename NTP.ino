/*-------- NTP code ----------*/
/*********************************************
   NTP Specifics
 *********************************************/
/* Don't hardwire the IP address or we won't get the benefits of the pool.
 *  Lookup the IP address for the host name instead */


time_t getNtpTime()
{
//get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 
  while (udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println(F("Transmit NTP Request"));
  ntp_attempts++;
  sendNTPpacket(timeServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) 
  {
    int size = udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) 
    {
      Serial.print(F("Received NTP Response, length="));
      Serial.println(size);
      udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      ntp_sync = 1;
      ntp_success++;
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  
  }
  Serial.println(F("No NTP Response :-("));
  return 0; // return 0 if unable to get the time
}
