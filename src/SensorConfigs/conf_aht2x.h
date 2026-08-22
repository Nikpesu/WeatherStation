void aht2xSetupSend();
void aht2xReconfigure()
{
    if(!AHT2x_toggle)
    {
        reconfigure=1;
        aht2xSetupSend();
        reconfigure=0;
    }

    //taskManager.cancelTask(AHT2x_task);
    //AHT2x_running=0;
    aht2x=AHTxx(AHTXX_ADDRESS_X38, AHT2x_SENSOR);
}

void aht2xRead()
{    
  if(aht2x.readHumidity()!=255)
  {
    aht2x_temp=(float)aht2x.readTemperature(AHTXX_USE_READ_DATA); 
    aht2x_hum=(float)aht2x.readHumidity(AHTXX_USE_READ_DATA); 
  }
  else 
  {
    aht2x_hum=nan("");
    aht2x_temp=nan("");
  }
    
}

void aht2xSetupSend()
{
    String sensor = "AHT2x";
    String SensorSuffix[] = {"temperature","humidity"};
    String deviceClass[] = {"temperature","humidity"};  // temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
    String unitOfMeasurement[] = {"°C","%"};
    float *sensorVariables[] ={&aht2x_temp, &aht2x_hum};

        if(reconfigure){
        sendMqttSensorDiscoveryClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        sendMqttSensorValuesClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        return;
    }

    if(sensorStart==1)
    {
        aht2x.begin();
        
        String status_topic = mqttStatusTopic();
        // Sending MQTT Discovery messages for each PM sensor (pm1_0, pm2_5, pm10_0)
        
        sendMqttSensorDiscovery(sensor, status_topic, deviceClass, SensorSuffix, unitOfMeasurement, sizeof(unitOfMeasurement)/sizeof(unitOfMeasurement[0]));

        if(!lowPowerMode_toggle)
        {
            //AHT2x_task = taskManager.scheduleFixedRate(refreshTime,aht2xSetupSend,TIME_SECONDS);
            //AHT2x_running=1;
            runningTasks=1;
        }
        else
        {
            sensorStart = 0;
            aht2xSetupSend();
            sensorStart = 1;
        }
    }
    else 
    {
        aht2xRead();
        applySensorOffset(0);

        sendMqttSensorValues(sensor, SensorSuffix, sensorVariables, sizeof(sensorVariables)/sizeof(sensorVariables[0]));

    }
}