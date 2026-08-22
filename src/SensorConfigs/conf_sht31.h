void sht31SetupSend();
void sht31Reconfigure()
{
    if(!SHT31_toggle)
    {
        reconfigure=1;
        sht31SetupSend();
        reconfigure=0;
    }

    //taskManager.cancelTask(SHT31_task);
    sht31=Adafruit_SHT31();

}

void sht31Read()
{
  sht31_temp = sht31.readTemperature();
  sht31_hum = sht31.readHumidity();
    
}

void sht31SetupSend()
{
    String sensor = "SHT31";
    String SensorSuffix[] = {"temperature","humidity"};
    String deviceClass[] = {"temperature","humidity"};  // temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
    String unitOfMeasurement[] = {"°C","%"};
    float *sensorVariables[] ={&sht31_temp, &sht31_hum};
        if(reconfigure){
        sendMqttSensorDiscoveryClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        sendMqttSensorValuesClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        return;
    }

    if(sensorStart==1)
    {
        
        sht31.begin(0x44);
        sht31.heater(false);
    

        
        String status_topic = mqttStatusTopic();
        // Sending MQTT Discovery messages for each PM sensor (pm1_0, pm2_5, pm10_0)
        
        sendMqttSensorDiscovery(sensor, status_topic, deviceClass, SensorSuffix, unitOfMeasurement, sizeof(unitOfMeasurement)/sizeof(unitOfMeasurement[0]));
        

        if(!lowPowerMode_toggle)
        {
            //SHT31_task = taskManager.scheduleFixedRate(refreshTime,sht31SetupSend,TIME_SECONDS);
            runningTasks=1;
        }
        else
        {
            sensorStart = 0;
            sht31SetupSend();
            sensorStart = 1;
        }
    }
    else 
    {
        sht31Read();
        applySensorOffset(7);

        sendMqttSensorValues(sensor, SensorSuffix, sensorVariables, sizeof(sensorVariables)/sizeof(sensorVariables[0]));

    }
}