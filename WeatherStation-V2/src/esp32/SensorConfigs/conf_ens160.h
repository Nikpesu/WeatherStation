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
        for (int i = 0; i < sizeof(SensorSuffix)/sizeof(SensorSuffix[0]); i++)
            sendMqtt("homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config","",true);
            return;
    }

    if(sensorStart==1)
    {
        myENS.begin();
        myENS.setOperatingMode(SFE_ENS160_STANDARD);

        
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
            ////ENS160_task = taskManager.scheduleFixedRate(refreshTime,ens160SetupSend,TIME_SECONDS);
            //ENS160_running=1;
            runningTasks=1;
        }
        else
            ens160SetupSend();
    }
    else 
    {
        ens160Read();

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