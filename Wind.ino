//------------------------------------------------------Wind Direction--------------------------------------------------------------

float DirWind(){

//int VaneValue;// raw analog value from wind vane 
//int Direction;// translated 0 - 360 direction 
//int CalDirection;// converted value with offset applied 


float winddirstate = map(analogRead(WD_Pin),0,1023,0,360); 
winddirstate = winddirstate + VaneOffset; 

if(winddirstate > 360) winddirstate = winddirstate - 360; 
if(winddirstate < 0) winddirstate = winddirstate + 360; 

/*
  if ( (winddirstate >= 210) && (winddirstate <= 220) ) {
    winddir_eu = 0;
  }
  if ( (winddirstate >= 0) && (winddirstate <= 25) ) {
    winddir_eu = 45;
  }
  if ( (winddirstate >= 25) && (winddirstate <= 75) ) {
    winddir_eu = 90;
  }
  if ( (winddirstate >= 75) && (winddirstate <= 120) ) {
    winddir_eu = 135;
  }
  if ( (winddirstate >= 120) && (winddirstate <= 150) ) {
    winddir_eu = 180;
  }
  if ( (winddirstate >= 150) && (winddirstate <= 180) ) {
    winddir_eu = 225;
  }
  if ( (winddirstate >= 180) && (winddirstate <= 205) ) {
    winddir_eu = 270;
  }
  if ( (winddirstate >= 205) && (winddirstate <= 210) ) {
    winddir_eu = 315;
  }
*/

  if (Wind_debug_dir) {
  Serial.print(F("Wind Dir: "));
  Serial.print(winddir_eu);
  Serial.print(F("   Pin Status: "));
  Serial.println(winddirstate);
  }
//  return winddir_eu;
  return winddirstate;

}


//---------------------------------------------------------Wind Speed----------------------------------------------

float speedwind()
{
     // cli();
     float pulseswindrmp = ( pulseswind / 60.00f );
     calcwindspeed = ( pulseswindrmp * 2.250f );
    if (calcwindspeed > tempwindgust)
    {
      tempwindgust = calcwindspeed;
    }
  if (debug) {
     Serial.print(F("Total pulseswindrmp:  "));
     Serial.print(pulseswindrmp);
     Serial.print(F("   Wind Speed:  "));
     Serial.println(calcwindspeed);
  }
     pulseswind = 0;
     //sei();
     return calcwindspeed;
     
}

//---------------------------------------------------------Wind Gust----------------------------------------------

float speedgust()
{
      //cli();
     float pulsesgustrmp = ( pulsesgust / 5.00f );
     calcgustspeed = ( pulsesgustrmp * 2.250f );
    if (calcgustspeed > tempwindgust)
    {
      tempwindgust = calcgustspeed;
    }
  if (debug) {
     Serial.print(F("Total pulsesgustrmp:  "));
     Serial.print(pulsesgustrmp);
     Serial.print(F("    Gust Speed:  "));
     Serial.println(calcgustspeed);
  }
     pulsesgust = 0;
     //sei();
     return calcgustspeed;
}

void Wind_Direction_cardinal(float wind_dir)
{
if (wind_dir <  12)  Wind_DirC ="N  ";
else if (wind_dir <  34)  Wind_DirC ="NNE";
else if (wind_dir <  57)  Wind_DirC ="NE ";
else if (wind_dir <  79)  Wind_DirC ="ENE";
else if(wind_dir <  102)  Wind_DirC ="E  ";
else if(wind_dir <  124)  Wind_DirC ="ESE";
else if(wind_dir <  147)  Wind_DirC ="SE ";
else if(wind_dir <  169)  Wind_DirC ="SSE";
else if(wind_dir <  192)  Wind_DirC ="S  ";
else if(wind_dir <  214)  Wind_DirC ="SSW";
else if(wind_dir <  237)  Wind_DirC ="SW ";
else if(wind_dir <  259)  Wind_DirC ="WSW";
else if(wind_dir <  282)  Wind_DirC ="W  ";
else if(wind_dir <  304)  Wind_DirC ="WNW";
else if(wind_dir <  327)  Wind_DirC ="NW ";
else if(wind_dir <  349)  Wind_DirC ="NNW";
else if(wind_dir <  361)  Wind_DirC ="N ";

}
