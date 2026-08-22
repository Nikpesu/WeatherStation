void ens160SetupSend();
void ens160Reconfigure()
{
    if(!ENS160_toggle)
    {
        reconfigure=1;
        ens160SetupSend();
        reconfigure=0;
    }

    //taskManager.cancelTask(ENS160_task);
    //ENS160_running=1;
    myENS = SparkFun_ENS160();
}

void ens160Read()
{    
  if(myENS.getFlags()!=0) 
  {
    ens160_eco2=nan("");
    ens160_aqi=nan("");
    ens160_tvoc=nan("");
  }
  else 
  {
      ens160_aqi=(float)myENS.getAQI();
      ens160_tvoc=(float)myENS.getTVOC();
      ens160_eco2=(float)myENS.getECO2();
  }
}

void ens160SetupSend()
{
    String sensor = "ENS160";
    String SensorSuffix[] = {"eCO2","TVOC","AQI"};
    String deviceClass[] = {"carbon_dioxide","volatile_organic_compounds","aqi"};  // temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
    String unitOfMeasurement[] = {"ppm","ppb",""};
    float *sensorVariables[] ={&ens160_eco2, &ens160_tvoc, &ens160_aqi};
        if(reconfigure){
        sendMqttSensorDiscoveryClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        sendMqttSensorValuesClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        return;
    }

    if(sensorStart==1)
    {
        myENS.begin();
        myENS.setOperatingMode(SFE_ENS160_STANDARD);

        
        String status_topic = mqttStatusTopic();
        // Sending MQTT Discovery messages for each PM sensor (pm1_0, pm2_5, pm10_0)
            
        sendMqttSensorDiscovery(sensor, status_topic, deviceClass, SensorSuffix, unitOfMeasurement, sizeof(unitOfMeasurement)/sizeof(unitOfMeasurement[0]));
        

        if(!lowPowerMode_toggle)
        {
            ////ENS160_task = taskManager.scheduleFixedRate(refreshTime,ens160SetupSend,TIME_SECONDS);
            //ENS160_running=1;
            runningTasks=1;
        }
        else
        {
            sensorStart = 0;
            ens160SetupSend();
            sensorStart = 1;
        }
    }
    else 
    {
        ens160Read();
        applySensorOffset(2);

        sendMqttSensorValues(sensor, SensorSuffix, sensorVariables, sizeof(sensorVariables)/sizeof(sensorVariables[0]));

    }
}