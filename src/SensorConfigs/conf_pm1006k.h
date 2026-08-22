void pm1006kSetupSend();
void pm1006kReconfigure()
{
    if(!PM1006K_toggle)
    {
        reconfigure=1;
        pm1006kSetupSend();
        reconfigure=0;
    }

    delete pm1006k;
    #if defined(ESP8266)
        pm1006k = new PM1006K(&pm1006Serial);
    #elif defined(ESP32)
        pm1006k = new PM1006K(&Serial1);
    #endif
    //taskManager.cancelTask(PM1006K_task);
}


void pm1006kRead()
{
    if (!pm1006k) return;
    // Replace this with your actual PM1006K reading logic
    // These variables must be populated with sensor values
    if (pm1006k->takeMeasurement())
    { 
        pm1006k_pm1_0 = pm1006k->getPM1_0();
        pm1006k_pm2_5 = pm1006k->getPM2_5();
        pm1006k_pm10_0 = pm1006k->getPM10();
    }
}

void pm1006kSetupSend()
{
    String sensor = "PM1006K";
    String SensorSuffix[] = {"pm1_0", "pm2_5", "pm10_0"};
    String deviceClass[] = {"pm1", "pm25", "pm10"};  // temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
    String unitOfMeasurement[] = {"µg/m³","µg/m³","µg/m³"};
    float *sensorVariables[] ={&pm1006k_pm1_0, &pm1006k_pm2_5, &pm1006k_pm10_0};
        if(reconfigure){
        sendMqttSensorDiscoveryClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        sendMqttSensorValuesClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        return;
    }

    if(sensorStart==1)
    {
        // https://github.com/kevinlutzer/Arduino-PM1006K/blob/main/examples/pm1006ktest/pm1006ktest.ino
        #if defined(ESP8266)
            pm1006Serial.begin(PM1006K::BAUD_RATE);
            pm1006k = new PM1006K(&pm1006Serial);
        #elif defined(ESP32)
            Serial1.begin(PM1006K::BAUD_RATE, SERIAL_8N1, (int)PM1006K_RX_PIN, (int)PM1006K_TX_PIN);
            pm1006k = new PM1006K(&Serial1); 
        #endif
        



        String status_topic = mqttStatusTopic();
        // Sending MQTT Discovery messages for each PM sensor (pm1_0, pm2_5, pm10_0)
        
        sendMqttSensorDiscovery(sensor, status_topic, deviceClass, SensorSuffix, unitOfMeasurement, sizeof(unitOfMeasurement)/sizeof(unitOfMeasurement[0]));

        // Handle sensor readings and refresh if not in low power mode
        if (!lowPowerMode_toggle)
        {
            ///PM1006K_task = taskManager.scheduleFixedRate(refreshTime, pm1006kSetupSend, TIME_SECONDS);
            runningTasks=1;
        }
        else
        {
            sensorStart = 0;
            pm1006kSetupSend(); // read once if in low power
            sensorStart = 1;
        }
    }    
    else 
    {
        pm1006kRead();
        applySensorOffset(3);

        sendMqttSensorValues(sensor, SensorSuffix, sensorVariables, sizeof(sensorVariables)/sizeof(sensorVariables[0]));

    }
}