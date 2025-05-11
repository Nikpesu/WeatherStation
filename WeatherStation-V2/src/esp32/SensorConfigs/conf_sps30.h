void sps30SetupSend();
void sps30Reconfigure()
{
    if(!SPS30_toggle)
    {
        reconfigure=1;
        sps30SetupSend();
        reconfigure=0;
    }

    //taskManager.cancelTask(SPS30_task);

    ///sps30=Adafruit_SPS30();
    //sps30=SPS30();

    sps30_stop_measurement();
    sensirion_i2c_release();

}

void sps30Read()
{
        uint16_t data_ready;
        int16_t ret;
        struct sps30_measurement m;
        ret=sps30_read_data_ready(&data_ready);

        if(ret)
        {
            sps30_read_measurement(&m);
            sps30_pm1_0=m.mc_1p0;
            sps30_pm2_5=m.mc_2p5;
            sps30_pm10_0=m.mc_10p0;
            sps30_stop_measurement();
            sensirion_i2c_release();

            sensirion_i2c_init();
            sps30_start_measurement();
        }
      //  if(sps30.dataAvailable())
      //  {
      //      sps30_pm1_0=sps30.numPM1;
      //      sps30_pm2_5=sps30.numPM25;
      //      sps30_pm10_0=sps30.numPM10;
      //  }
}

void sps30SetupSend()
{
    String sensor = "SPS30";
    String SensorSuffix[] = {"pm1_0", "pm2_5", "pm10_0"};
    String deviceClass[] = {"pm1", "pm25", "pm10"};  // temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
    String unitOfMeasurement[] = {"µg/m³","µg/m³","µg/m³"};
    float *sensorVariables[] ={&sps30_pm1_0, &sps30_pm2_5, &sps30_pm10_0};
    if(reconfigure){
        for (int i = 0; i < sizeof(SensorSuffix)/sizeof(SensorSuffix[0]); i++)
            sendMqtt("homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config","",true);
            return;
    }

    if(sensorStart==1)
    {
        // https://github.com/kevinlutzer/Arduino-sps30/blob/main/examples/sps30test/sps30test.ino
        // sps30.begin();

        sensirion_i2c_init();
        sps30_set_fan_auto_cleaning_interval_days(4); //every 4 days clean
        //sps30_start_manual_fan_cleaning();
        sps30_start_measurement();

        
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
                "\"object_id\": \""  + sensor + "_" + SensorSuffix[i]+ "_" + mdns_hostname  + "\"," // sps30_pm1_0_wstestHostname
                "\"object_name\": \""  + sensor + "_" + SensorSuffix[i]+ "_" + mdns_hostname  + "\"," // sps30_pm1_0_wstestHostname
                "\"name\": \""  + sensor + " " + SensorSuffix[i]+ "\"," // sps30_pm1_0_wstestHostname
                "\"origin\": {"
                    "\"name\": \"Weatherstation\","
                    "\"sw\": \"" + SWversion + "\"," //1.0
                    "\"url\": \"https://github.com/Nikpesu/WeatherStation\""
                "}"
                ","
                "\"unit_of_measurement\": \""+ unitOfMeasurement[i] +"\"," // C*, RH%...
                "\"unique_id\": \"" + UniqueDeviceID + "_" + sensor + "_" + mdns_hostname + "_" + SensorSuffix[i]+  "\"," // xxxxx_sps30_wstestHostname_pm1_0
                "\"value_template\": \"{{ value_json."+SensorSuffix[i]+" }}\"," // value_json.pm1_0 ...
                "\"state_topic\": \""+mqtt_messageRoot+"/"+sensor+"\"," // wstest1/sps30
                "\"state_class\": \"measurement\""
                "}",
                true
            );
        }

        // Handle sensor readings and refresh if not in low power mode
        if (!lowPowerMode_toggle)
        {
            //SPS30_task = taskManager.scheduleFixedRate(refreshTime,sps30SetupSend,TIME_SECONDS);
            runningTasks=1;
        }
        else
        {
            if (SPS30_toggle)
                sps30SetupSend(); // read once if in low power
            }
    }
    else 
    {
        sps30Read();

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