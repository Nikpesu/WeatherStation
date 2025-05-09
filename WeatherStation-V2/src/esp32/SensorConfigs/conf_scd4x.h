void scd4xSetupSend();
void scd4xReconfigure()
{
    if(!SCD4x_toggle)
    {
        reconfigure=1;
        scd4xSetupSend();
        reconfigure=0;
    }

    taskManager.cancelTask(SCD4x_task);
        
    scd4x=SensirionI2cScd4x();
    //TODO add SCD4x sensor def
    //taskManager.cancelTask(BMP280_task);
    //bmp280 = Adafruit_BMP280();

}

void scd4xRead()
{
    uint16_t co2;
    scd4x.readMeasurement(co2, scd4x_temp, scd4x_hum);
    scd4x_co2=co2;

    Serial.print("CO2 concentration [ppm]: ");
    Serial.print(scd4x_co2);
    Serial.println();
    Serial.print("Temperature [°C]: ");
    Serial.print(scd4x_temp);
    Serial.println();
    Serial.print("Relative Humidity [RH]: ");
    Serial.print(scd4x_hum);
    Serial.println();

}

void scd4xSetupSend()
{
    String sensor = "SCD4x";
    String SensorSuffix[] = {"CO2","humidity","temperature"};
    String deviceClass[] = {"carbon_dioxide","humidity","temperature"};  // temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
    String unitOfMeasurement[] = {"ppm","%","°C"};
    float *sensorVariables[] ={&scd4x_co2, &scd4x_hum, &scd4x_temp};
    if(reconfigure){
        for (int i = 0; i < sizeof(SensorSuffix)/sizeof(SensorSuffix[0]); i++)
            sendMqtt("homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config","",true);
            return;
    }

    if(sensorStart==1)
    {
        
        //TODO add SCD4x sensor def
            scd4x.begin(Wire, 0x62);//SCD40 0x62 | SCD41 0x62
            scd4x.startPeriodicMeasurement();
        
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
            SCD4x_task = taskManager.scheduleFixedRate(refreshTime,scd4xSetupSend,TIME_SECONDS);
            runningTasks=1;
        }
        else
            scd4xSetupSend();
    }
    else 
    {
        scd4xRead();

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