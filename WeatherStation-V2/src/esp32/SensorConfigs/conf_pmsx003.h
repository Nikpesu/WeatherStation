void pmsx003SetupSend();
void pmsx003Reconfigure()
{
    if(!PMSx003_toggle)
    {
        reconfigure=1;
        pmsx003SetupSend();
        reconfigure=0;
    }

    taskManager.cancelTask(PMSx003_task);
        
    
    //TODO add PMSx003 sensor def
    //Serial1.begin(PM1006K::BAUD_RATE, SERIAL_8N1, PM1006K_RX_PIN, PM1006K_TX_PIN);
    //pmsx003 = new PM1006K(&Serial1);
}


void pmsx003Read()
{

        //TODO add PMSx003 sensor read
}

void pmsx003SetupSend()
{
    String sensor = "PMSx003";
    String SensorSuffix[] = {"pm1_0", "pm2_5", "pm10_0"};
    String deviceClass[] = {"pm1", "pm25", "pm10"};  // temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
    String unitOfMeasurement[] = {"µg/m³","µg/m³","µg/m³"};
    float *sensorVariables[] ={&pmsx003_pm1_0, &pmsx003_pm2_5, &pmsx003_pm10_0};
    if(reconfigure){
        for (int i = 0; i < sizeof(SensorSuffix)/sizeof(SensorSuffix[0]); i++)
            sendMqtt("homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config","",true);
            return;
    }

    if(sensorStart==1)
    {
        // https://github.com/kevinlutzer/Arduino-pmsx003/blob/main/examples/pmsx003test/pmsx003test.ino

        //TODO add PMSx003 sensor def


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
                "\"object_id\": \""  + sensor + "_" + SensorSuffix[i]+ "_" + mdns_hostname  + "\"," // pmsx003_pm1_0_wstestHostname
                "\"object_name\": \""  + sensor + "_" + SensorSuffix[i]+ "_" + mdns_hostname  + "\"," // pmsx003_pm1_0_wstestHostname
                "\"name\": \""  + sensor + " " + SensorSuffix[i]+ "\"," // pmsx003_pm1_0_wstestHostname
                "\"origin\": {"
                    "\"name\": \"Weatherstation\","
                    "\"sw\": \"" + SWversion + "\"," //1.0
                    "\"url\": \"https://github.com/Nikpesu/WeatherStation\""
                "}"
                ","
                "\"unit_of_measurement\": \""+ unitOfMeasurement[i] +"\"," // C*, RH%...
                "\"unique_id\": \"" + UniqueDeviceID + "_" + sensor + "_" + mdns_hostname + "_" + SensorSuffix[i]+  "\"," // xxxxx_pmsx003_wstestHostname_pm1_0
                "\"value_template\": \"{{ value_json."+SensorSuffix[i]+" }}\"," // value_json.pm1_0 ...
                "\"state_topic\": \""+mqtt_messageRoot+"/"+sensor+"\"," // wstest1/pmsx003
                "\"state_class\": \"measurement\""
                "}",
                true
            );
        }

        // Handle sensor readings and refresh if not in low power mode
        if (!lowPowerMode_toggle)
        {
            PMSx003_task= taskManager.scheduleFixedRate(refreshTime, pmsx003SetupSend, TIME_SECONDS);
            runningTasks=1;
        }
        else
            pmsx003SetupSend(); // read once if in low power
    }
    else 
    {
        pmsx003Read();

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