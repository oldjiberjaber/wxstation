 bool ReadI2C(void)
 {

getOneWireTemp();
    if (BMP1.Present)
    {
//BMP Temp now read from DS sensor
//        BMP1.TempC = bme.readTemperature();
        BMP1.Pressure = bme.readPressure();
// Uncomment if using BME Humidity Sensor
//    BMP1.Humidity = bme.readHumidity();
        BMP1.counter++;
    }
    if (I2C_debug_BME280) {
      Serial.print(F("Detected? ")); if (BMP1.Present) Serial.println(F("Yes")); else Serial.println(F("No"));
      Serial.print(F("I2C Read Temperature = "));
//      Serial.print((int) BMP1.TempC);
      Serial.print(bme.readTemperature());
      Serial.println(F(" *C"));
      Serial.print(F("I2C Read Pressure = "));
      //Serial.print(BMP1.Pressure);
      Serial.print(bme.readPressure());
      Serial.println(F(" Pa"));
// Uncomment if using BME Humidity Sensor
//      Serial.print(bme.readHumidity()); Serial.println(F(" %"));
      }
//Solar radiation = [W/m^2]
    if (UV1.Present)
      {
          UV1.UVindex = uv.readUV();
          UV1.UVindex = UV1.UVindex/100;
          UV1.Vis = uv.readVisible();
          UV1.IR = uv.readIR();
          UV1.counter++;
      }
    
      if (I2C_debug_uv) {
          Serial.print(F("Detected? ")); 
          if (UV1.Present) Serial.println(F("Yes")); else Serial.println(F("No"));
          Serial.print(F("Vis: ")); Serial.println(UV1.Vis);
          Serial.print(F("IR: ")); Serial.println(UV1.IR);
          Serial.print(F("UV: "));  Serial.println(UV1.UVindex);
          }
   
      
  // the index is multiplied by 100 so to get the
  // integer index, divide by 100!
  //  UVindex /= 100.0;  
  
  return 1;
 }
