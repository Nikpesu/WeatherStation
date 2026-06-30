#include "SensorConfigs/conf_aht2x.h"
#include "SensorConfigs/conf_bmp280.h"
#include "SensorConfigs/conf_bmp580.h"
#include "SensorConfigs/conf_ens160.h"
#include "SensorConfigs/conf_pm1006k.h"
#include "SensorConfigs/conf_pmsx003.h"
#include "SensorConfigs/conf_scd4x.h"
#include "SensorConfigs/conf_sgp30.h"
#include "SensorConfigs/conf_sht31.h"
#include "SensorConfigs/conf_sps30.h"

void allSetupSend()
{
    if(AHT2x_toggle) aht2xSetupSend();
    if(BMP280_toggle) bmp280SetupSend();
    if(BMP580_toggle) bmp580SetupSend();
    if(ENS160_toggle)	ens160SetupSend();
    if(PM1006K_toggle) pm1006kSetupSend();
    if(PMSx003_toggle) pmsx003SetupSend();
    if(SCD4x_toggle) scd4xSetupSend(); 
    if(SGP30_toggle) sgp30SetupSend(); 
    if(SHT31_toggle) sht31SetupSend();
    if(SPS30_toggle) sps30SetupSend();
}

void allReconfigure()
{
  aht2xReconfigure();
  bmp280Reconfigure();
  bmp580Reconfigure();
  ens160Reconfigure();
  pm1006kReconfigure();
  pmsx003Reconfigure();
  scd4xReconfigure(); 
  sgp30Reconfigure(); 
  sht31Reconfigure();
  sps30Reconfigure();
  allSetupSend();
}

void sensorsBegin()
{
  sensorStart=1;
  //Wire.begin(sda,scl,100000);

  if(runningTasks)
  {
    allReconfigure();
    runningTasks=0;
  }

  allSetupSend();

  // (re)init the status-indicator LEDs AFTER the sensor UARTs are set up, so the
  // RMT channel is the last thing attached to the LED pin. Idempotent: only
  // re-touches the hardware on first init or an actual pin change.
  ledsBegin();

  sensorStart=0;

  if(lowPowerMode_toggle)
  {
    delay(1000); sleepAndReset();
  }

}
unsigned long lastSensorTriggerTime=millis();
void sensorLoop()
{
  unsigned long currTime=millis();
  if(currTime - lastSensorTriggerTime >= (unsigned long)refreshTime * 1000)
  {
    allSetupSend();
    lastSensorTriggerTime=millis();
  }
}

int getAbsoluteHumidity(float temperature, float humidity)
{
  // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity);                                                                // [mg/m^3]
  return absoluteHumidityScaled;
}