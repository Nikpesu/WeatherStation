void scd4xSetupSend();
void scd4xReconfigure()
{
    if(!SCD4x_toggle)
    {
        reconfigure=1;
        scd4xSetupSend();
        reconfigure=0;
    }

    ///taskManager.cancelTask(SCD4x_task);
        
    scd4x=SCD4x();
    //scd4x=SensirionI2cScd4x();
    //taskManager.cancelTask(BMP280_task);
    //bmp280 = Adafruit_BMP280();

}

void scd4xRead()
{
    scd4x.readMeasurement();
    scd4x_co2=scd4x.getCO2();
    scd4x_temp=scd4x.getTemperature();
    scd4x_hum=scd4x.getHumidity();

}

void scd4xSetupSend()
{
    String sensor = "SCD4x";
    String SensorSuffix[] = {"CO2","humidity","temperature"};
    String deviceClass[] = {"carbon_dioxide","humidity","temperature"};  // temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
    String unitOfMeasurement[] = {"ppm","%","°C"};
    float *sensorVariables[] ={&scd4x_co2, &scd4x_hum, &scd4x_temp};
        if(reconfigure){
        sendMqttSensorDiscoveryClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        sendMqttSensorValuesClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        return;
    }

    if(sensorStart==1)
    {
        
        scd4x.begin();//SCD40 0x62 | SCD41 0x62
        scd4x.startPeriodicMeasurement();
    
        String status_topic = mqttStatusTopic();
        // Sending MQTT Discovery messages for each PM sensor (pm1_0, pm2_5, pm10_0)
        
        sendMqttSensorDiscovery(sensor, status_topic, deviceClass, SensorSuffix, unitOfMeasurement, sizeof(unitOfMeasurement)/sizeof(unitOfMeasurement[0]));
        

        if(!lowPowerMode_toggle)
        {
            //SCD4x_task = taskManager.scheduleFixedRate(refreshTime,scd4xSetupSend,TIME_SECONDS);
            runningTasks=1;
        }
        else
        {
            sensorStart = 0;
            scd4xSetupSend();
            sensorStart = 1;
        }
    }
    else 
    {
        scd4xRead();
        applySensorOffset(5);

        sendMqttSensorValues(sensor, SensorSuffix, sensorVariables, sizeof(sensorVariables)/sizeof(sensorVariables[0]));

    }
}