/*
 * ROM = 28 FF 10 38 2E 4 0 3B
  Chip = DS18B20
  Data = 1 7C 1 55 0 7F FF C 10 42  CRC=42
  Temperature = 23.75 Celsius, No more addresses.

getOneWireTemp(&DT[i].addr0,&DT[i].Temperature,&ds)
 */


boolean getOneWireTemp(){
    double temperature;
    float heatindex;
    float dewpoint;
    float humidity;
    ds2438.update();
    if (ds2438.isError() || ds2438.getVoltage(DS2438_CHA) == 0.0) {
        Serial.println(F("Error reading from DS2438 device"));
        return false;
    } else {
        temperature = ds2438.getTemperature();
        heatindex = temperature;
        float rh = (ds2438.getVoltage(DS2438_CHA) / ds2438.getVoltage(DS2438_CHB) - 0.16) / 0.0062;
        humidity = (float)(rh / (1.0546 - 0.00216 * temperature));
        if (humidity < 0.0) {
            humidity = 0.0;
        } else if (humidity > 100.0) {
                    Serial.println();
            //humidity = 100.0;
        }
        float tempK = temperature + 273.15;
        dewpoint = tempK / ((-0.0001846 * log(humidity / 100.0) * tempK) + 1.0) - 273.15;
        if (temperature >= 26.7 && humidity >= 40.0) {
            float t = temperature * 9.0 / 5.0 + 32.0; // heat index formula assumes degF
            rh = humidity;
            float heatindexF = -42.38 + 2.049 * t + 10.14 * rh + -0.2248 * t * rh + -0.006838 * t * t
                               + -0.05482 * rh * rh + 0.001228 * t * t * rh + 0.0008528 * t * rh * rh
                               + -0.00000199 * t * t * rh * rh;
            heatindex = (heatindexF - 32.0) * 5.0 / 9.0;
        }
        if (heatindex < temperature)
            heatindex = temperature;
    }
     if(debug_DS2438) {
                    Serial.println();
                    Serial.print(F("Temperature = "));
                    Serial.print(temperature, 1);
                    Serial.print(F("C ("));
                    Serial.print(temperature * 9.0 / 5.0 + 32.0, 1);
                    Serial.print(F("F), Dew Point = "));
                    Serial.print(dewpoint, 1);
                    Serial.print(F("C ("));
                    Serial.print(dewpoint * 9.0 / 5.0 + 32.0, 1);
                    Serial.print(F("F), Relative Humidity = "));
                    Serial.print(humidity, 0);
                    Serial.println(F("%."));
                    }
  BMP1.DewPoint=dewpoint;
  BMP1.TempC = temperature;
  BMP1.Humidity = humidity;
  return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
//Matt G.
//onion@doorcreekorchard.com
//July 2008
boolean getOneWireTempDS18B20()
 {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  float celsius;
  if(debug_DS18B20) {
                    Serial.println(F(""));
                    Serial.print(F("ROM ="));
                    for( i = 0; i < 8; i++) {
                                            Serial.write(' ');
                                            Serial.print(DSaddr[i], HEX);
                                            }
                    }
  if (OneWire::crc8(DSaddr, 7) != DSaddr[7]) {
      Serial.println(F("CRC is not valid!"));
      return false;
  }
  
 
  // the first ROM byte indicates which chip
  //    Serial.println(F("  Chip = DS18B20"));
      type_s = 0;
  
  ds.reset();
  ds.select(DSaddr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(500);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(DSaddr);    
  ds.write(0xBE);         // Read Scratchpad

  if(debug_DS18B20) {
                    Serial.println();
                    Serial.print(F("  Data = "));
                    Serial.print(present, HEX);
                    Serial.print(F(" "));
                    }
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
                          data[i] = ds.read();
                          if(debug_DS18B20) {
                                            Serial.print(data[i], HEX);
                                            Serial.print(F(" "));
                                            } 
                          }

if (OneWire::crc8(data, 8) != data[8]) {
      Serial.println(F("DATA CRC is not valid!"));
      return false;
  }

if(debug_DS18B20) {
                    Serial.print(F(" CRC="));
                    Serial.print(OneWire::crc8(data, 8), HEX);
                    Serial.println();
                    }
  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  if(debug_DS18B20) {
                    Serial.print(F("  Temperature = "));
                    Serial.print(celsius);
                    Serial.println(F(" Celsius"));
                    }
  BMP1.TempC = celsius;
  return true;
 }

//Search for DS Sensor and return true if found
bool Find_Sensors(void)
{  
  byte searchaddr[8]; 
  ds.reset_search();
  delay(500);
    if ( ds.search(searchaddr)) //Search for DS18B20 devices
    {
 /*     Serial.print(F("DS Sensors: "));
            Serial.println(searchaddr[0]);
            Serial.println(searchaddr[1]);
            Serial.println(searchaddr[2]);
            Serial.println(searchaddr[3]);
            Serial.println(searchaddr[4]);
            Serial.println(searchaddr[5]);
            Serial.println(searchaddr[6]);
            Serial.println(searchaddr[7]);
            
            Serial.println();
            Serial.println(DS2438_address[1]);
*/            if (searchaddr[1] == DS2438_address[1]) 
                {
                 //sensor present 
                // Serial.println(F("FOUND!"));
                 return true;
                 
                }
                
}            else
                {
                  return false;
                }
}
