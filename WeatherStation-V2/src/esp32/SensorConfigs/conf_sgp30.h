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
    if(!(sht31_temp==nan("") or sht31_hum==nan("")))
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
        for (int i = 0; i < sizeof(SensorSuffix)/sizeof(SensorSuffix[0]); i++)
            sendMqtt("homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config","",true);
            return;
    }

    if(sensorStart==1)
    {
        sgp30.begin();

    
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
            //SGP30_task = taskManager.scheduleFixedRate(refreshTime,sgp30SetupSend,TIME_SECONDS);
            runningTasks=1;
        }
        else
            sgp30SetupSend();
    }
    else 
    {
        sgp30Read();

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