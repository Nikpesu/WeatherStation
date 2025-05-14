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
        for (int i = 0; i < sizeof(SensorSuffix)/sizeof(SensorSuffix[0]); i++)
            sendMqtt("homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config","",true);
            return;
    }

    if(sensorStart==1)
    {
        aht2x.begin();
        
        char status_topic[mqtt_messageRoot.length() + 1];
        mqtt_messageRoot.toCharArray(status_topic, mqtt_messageRoot.length() + 1);
        // Sending MQTT Discovery messages for each PM sensor (pm1_0, pm2_5, pm10_0)
        
        for (int i = 0; i < sizeof(unitOfMeasurement)/sizeof(unitOfMeasurement[0]); i++)
            sendMqtt(
                "homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config", 
                makeMqttSensorTopic(i, sensor, status_topic, &deviceClass[0], &SensorSuffix[0],  &unitOfMeasurement[0]), 
                true
            );

        if(!lowPowerMode_toggle)
        {
            //AHT2x_task = taskManager.scheduleFixedRate(refreshTime,aht2xSetupSend,TIME_SECONDS);
            //AHT2x_running=1;
            runningTasks=1;
        }
        else
            aht2xSetupSend();
    }
    else 
    {
        aht2xRead();

        String msg="{";
        for (int i=0; i<sizeof(sensorVariables)/sizeof(sensorVariables[0]); i++)
        {
            msg+="\""+SensorSuffix[i]+"\":"+String((float)*(sensorVariables[i]))+",";
        }
        msg.remove(msg.length() - 1);
        msg+="}";

        sendMqtt(mqtt_messageRoot+"/"+sensor, msg,true);

    }
}