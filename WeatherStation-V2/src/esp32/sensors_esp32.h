void sensorsBegin()
{
  bmp280.begin();
  sgp30.begin();
  sht31.begin(0x44);
  sht31.heater(false);
  myENS.begin();	
  myENS.setOperatingMode(SFE_ENS160_STANDARD);
  //sgp30.setIAQBaseline(0x8CEE, 0x8C10);                // set it manualy if you want... https://github.com/adafruit/Adafruit_SGP30/blob/master/examples/sgp30test/sgp30test.ino
  bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                     Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                     Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                     Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                     Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */  
                     
                     
  taskManager.scheduleFixedRate(3,sht31Read,TIME_SECONDS);
  delay(1000);
  taskManager.scheduleFixedRate(3,sgp30Read,TIME_SECONDS);
  delay(1000);
  taskManager.scheduleFixedRate(3,bmp280Read,TIME_SECONDS);

  taskManager.scheduleFixedRate(refreshTime,sht31Send,TIME_SECONDS);
  taskManager.scheduleFixedRate(refreshTime,sgp30Send,TIME_SECONDS);
  taskManager.scheduleFixedRate(refreshTime,bmp280Send,TIME_SECONDS);
                       
  if(!lowPowerMode_toggle)
  {
    if(SHT31_toggle) taskManager.scheduleFixedRate(refreshTime,sht31Send,TIME_SECONDS);
    if(SGP30_toggle) taskManager.scheduleFixedRate(refreshTime,sgp30Send,TIME_SECONDS);
    if(BMP280_toggle) taskManager.scheduleFixedRate(refreshTime,bmp280Send,TIME_SECONDS);
    if(AHT2x_toggle) taskManager.scheduleFixedRate(refreshTime,AHT2xSend,TIME_SECONDS);
    if(ENS160_toggle) taskManager.scheduleFixedRate(refreshTime,ENS160Send,TIME_SECONDS);
  }
  else 
  {
    if(SHT31_toggle) sht31Send();
    if(SGP30_toggle) sgp30Send();
    if(BMP280_toggle) bmp280Send();
    if(AHT2x_toggle) AHT2xSend();
    if(ENS160_toggle) ENS160Send();
    delay(1000); sleepAndReset();
  }

}

void sht31Read()
{
  sht31_temp = sht31.readTemperature();
  sht31_hum = sht31.readHumidity();
}
int getAbsoluteHumidity(float temperature, float humidity)
{
  // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity);                                                                // [mg/m^3]
  return absoluteHumidityScaled;
}
void sgp30Read()
{
  sgp30.setHumidity(getAbsoluteHumidity(sht31_temp+3, sht31_hum+10));
  sgp30.IAQmeasure();
  sgp30.IAQmeasureRaw();
  sgp30_co2 = sgp30.eCO2;
  sgp30_tvoc = sgp30.TVOC;
  sgp30_h2 = sgp30.rawH2;
  sgp30_eth = sgp30.rawEthanol;
}
void bmp280Read()
{
  bmp280_press = bmp280.readPressure();
  bmp280_temp = bmp280.readTemperature();
  bmp280_alt = bmp280.readAltitude(1013.25);
  String dbg=(String)bmp280_press+" "+(String)bmp280_alt+" "+(String)bmp280_temp+"|||"+(String)sht31_hum+" "+(String)sht31_temp+" "+(String)sht31.isHeaterEnabled()+"|||"+(String)sgp30_co2+" "+(String)sgp30_eth+" "+(String)sgp30_h2+" "+(String)sgp30_tvoc;
  Serial.println(dbg);
}

void AHT2xRead()
{
  if(aht20.readHumidity()!=255)
  {
    aht2x_temp=(float)aht20.readTemperature(AHTXX_USE_READ_DATA); 
    aht2x_hum=(float)aht20.readHumidity(AHTXX_USE_READ_DATA); 
  }
  else 
  {
    aht2x_hum=(float)-1;
    aht2x_temp=(float)-1;
  }
}
void ENS160Read()
{
  if(myENS.getFlags()!=0) 
  {
    ens160_eco2=(float)-1;
    ens160_aqi=(float)-1;
    ens160_tvoc=(float)-1;
  }
  else 
  {
      ens160_aqi=(float)myENS.getAQI();
      ens160_tvoc=(float)myENS.getTVOC();
      ens160_eco2=(float)myENS.getECO2();
  }

}