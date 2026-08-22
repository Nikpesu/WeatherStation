void sgp30SetupSend();
void sgp30Reconfigure()
{
    if(!SGP30_toggle)
    {
        reconfigure=1;
        sgp30SetupSend();
        reconfigure=0;
    }

    //taskManager.cancelTask(SGP30_task);
    sgp30=Adafruit_SGP30();

}

void sgp30Read()
{
    if(!isnan(sht31_temp) && !isnan(sht31_hum))
        sgp30.setHumidity(getAbsoluteHumidity(sht31_temp, sht31_hum));
    else 
        sgp30.setHumidity(0);


    sgp30.IAQmeasure();
    sgp30.IAQmeasureRaw();

    sgp30_co2 = sgp30.eCO2;
    sgp30_tvoc = sgp30.TVOC;
    sgp30_h2 = sgp30.rawH2;
    sgp30_eth = sgp30.rawEthanol;
}

void sgp30SetupSend()
{
    String sensor = "SGP30";
    String SensorSuffix[] = {"eCO2","TVOC","rawH2","rawEthanol"};
    String deviceClass[] = {"carbon_dioxide","volatile_organic_compounds","None", "None"};  // temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
    String unitOfMeasurement[] = {"ppm","ppb","ppm","ppm"};
    float *sensorVariables[] ={&sgp30_co2, &sgp30_tvoc, &sgp30_h2, &sgp30_eth};
        if(reconfigure){
        sendMqttSensorDiscoveryClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        sendMqttSensorValuesClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        return;
    }

    if(sensorStart==1)
    {
        sgp30.begin();

    
        String status_topic = mqttStatusTopic();
        // Sending MQTT Discovery messages for each PM sensor (pm1_0, pm2_5, pm10_0)
        
        sendMqttSensorDiscovery(sensor, status_topic, deviceClass, SensorSuffix, unitOfMeasurement, sizeof(unitOfMeasurement)/sizeof(unitOfMeasurement[0]));
        
        

        if(!lowPowerMode_toggle)
        {
            //SGP30_task = taskManager.scheduleFixedRate(refreshTime,sgp30SetupSend,TIME_SECONDS);
            runningTasks=1;
        }
        else
        {
            sensorStart = 0;
            sgp30SetupSend();
            sensorStart = 1;
        }
    }
    else 
    {
        sgp30Read();
        applySensorOffset(6);

        sendMqttSensorValues(sensor, SensorSuffix, sensorVariables, sizeof(sensorVariables)/sizeof(sensorVariables[0]));

    }
}