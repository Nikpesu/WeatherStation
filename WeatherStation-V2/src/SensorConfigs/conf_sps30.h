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
        if (ret < 0) 
            Serial.println("["+runningTime()+"] SPS30 error: "+ret);
        if(ret==0)
        {
            sps30_read_measurement(&m);
            sps30_pm1_0=m.mc_1p0;
            sps30_pm2_5=m.mc_2p5;
            sps30_pm10_0=m.mc_10p0;
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
        int16_t ret;        
        
        sensirion_i2c_init(Wire);

        if (sps30_probe() != 0) {
            Serial.print("SPS sensor probing failed\n");
        }

        ret = sps30_set_fan_auto_cleaning_interval_days(4);
        if (ret) {
            Serial.print("["+runningTime()+"] SPS30 error setting the auto-clean interval: ");
            Serial.println(ret);
        }

        ret = sps30_start_measurement();
        if (ret < 0) {
            Serial.print("["+runningTime()+"] SPS30 error starting measurement\n");
        }

        
        char status_topic[mqtt_messageRoot.length() + 1];
        mqtt_messageRoot.toCharArray(status_topic, mqtt_messageRoot.length() + 1);
        // Sending MQTT Discovery messages for each PM sensor (pm1_0, pm2_5, pm10_0)
        
        for (int i = 0; i < sizeof(unitOfMeasurement)/sizeof(unitOfMeasurement[0]); i++)
            sendMqtt(
                "homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config", 
                makeMqttSensorTopic(i, sensor, status_topic, &deviceClass[0], &SensorSuffix[0],  &unitOfMeasurement[0]), 
                true
            );
        

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