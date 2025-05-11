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
        for (int i = 0; i < sizeof(SensorSuffix)/sizeof(SensorSuffix[0]); i++)
            sendMqtt("homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config","",true);
            return;
    }

    if(sensorStart==1)
    {
        
        sht31.begin(0x44);
        sht31.heater(false);
    

        
            char status_topic[mqtt_messageRoot.length() + 1];
            mqtt_messageRoot.toCharArray(status_topic, mqtt_messageRoot.length() + 1);
            // Sending MQTT Discovery messages for each PM sensor (pm1_0, pm2_5, pm10_0)
            
            for (int i = 0; i < sizeof(unitOfMeasurement)/sizeof(unitOfMeasurement[0]); i++)
            {
                sendMqtt("homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config",
                    "{"
                    "\"availability\": ["
                        "{"
                        "\"topic\": \""+String(status_topic)+"\","
                        "\"value_template\": \"{{ value_json.state }}\""
                        "}"
                    "],"
                    "\"device\": {"
                        "\"hw_version\": \""+HWversion+"\","
                        "\"identifiers\": ["
                            "  \"Weatherstation_"+UniqueDeviceID+"\""
                        "],"
                        "\"manufacturer\": \""+Manufacturer+"\","
                        "\"model\": \""+Model+"\","
                        "\"model_id\": \""+ModelID+"\","
                        "\"name\": \""+ mdns_hostname +"\","
                        "\"sw_version\": \""+SWversion+"\","
                        "\"via_device\": \""+device+"\""
                        "}"
                    ","
                    "\"device_class\": \"" + deviceClass[i] + "\"," // temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
                    "\"enabled_by_default\": true,"
                    "\"object_id\": \""  + sensor + "_" + SensorSuffix[i]+ "_" + mdns_hostname  + "\"," // PM1006K_pm1_0_wstestHostname
                    "\"name\": \""  + sensor + " " + SensorSuffix[i]+ "\"," // PM1006K_pm1_0_wstestHostname
                    "\"origin\": {"
                        "\"name\": \"Weatherstation\","
                        "\"sw\": \"" + SWversion + "\"," //1.0
                        "\"url\": \"https://github.com/Nikpesu/WeatherStation\""
                    "}"
                    ","
                    "\"unit_of_measurement\": \""+ unitOfMeasurement[i] +"\"," // C*, RH%...
                    "\"unique_id\": \"" + UniqueDeviceID + "_" + sensor + "_" + mdns_hostname + "_" + SensorSuffix[i]+  "\"," // xxxxx_PM1006K_wstestHostname_pm1_0
                    "\"value_template\": \"{{ value_json."+SensorSuffix[i]+" }}\"," // value_json.pm1_0 ...
                    "\"state_topic\": \""+mqtt_messageRoot+"/"+sensor+"\"," // wstest1/PM1006K
                    "\"state_class\": \"measurement\""
                    "}",
                    true
                );
        }
        

        if(!lowPowerMode_toggle)
        {
            //SHT31_task = taskManager.scheduleFixedRate(refreshTime,sht31SetupSend,TIME_SECONDS);
            runningTasks=1;
        }
        else
            sht31SetupSend();
    }
    else 
    {
        sht31Read();

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