//-------------------------------------------------------Interupt Wind and Rain---------------------------------------------------------


void rpm() {
    long thisTime=micros()-wind_last;
  wind_last=micros();
  if(thisTime>500)
  {
    
  pulseswind++;
  pulsesgust++;
    if (Wind_debug_spd) {
  Serial.print(F("Interupt Nb wind turn:  "));
  Serial.println(pulseswind);
    }
  }
}

// Interrupt routine
void rain() {
  long thisTime=micros()-rain_last;
  rain_last=micros();
  if(thisTime>1000)
  {
    if (raincount1h==0)
  {
    count1h = millis();
  }
    raincount1h = raincount1h + 1.00f;
    raincount = raincount + 1.00f;
    WeeWX_RainAmount++;
    
    //log(INT_Raindrop);
    
    if (Rain_debug) {
      Serial.print(F("Interupt Nb rain drop:  "));
      Serial.println(raincount);
    }
  }
}
