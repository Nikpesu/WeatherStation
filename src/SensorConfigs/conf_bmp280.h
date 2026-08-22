void bmp280SetupSend();
void bmp280Reconfigure()
{
    if(!BMP280_toggle)
    { 
        reconfigure=1;
        bmp280SetupSend();
        reconfigure=0;
    }

    ///taskManager.cancelTask(BMP280_task);
    //BMP280_running=0;
    bmp280 = Adafruit_BMP280();
}

void bmp280Read()
{
  bmp280_press = bmp280.readPressure()/100;
  bmp280_temp = bmp280.readTemperature();
  bmp280_alt = bmp280.readAltitude(1013.25);
    
  //String dbg="bmp dbg"+(String)bmp280_press+" "+(String)bmp280_alt+" "+(String)bmp280_temp;
  //Serial.println(dbg); 
}

void bmp280SetupSend()
{
    String sensor = "BMP280";
    String SensorSuffix[] = {"temperature","pressure","altitude"};
    String deviceClass[] = {"temperature","pressure","distance"};  // temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
    String unitOfMeasurement[] = {"°C","hPa","m"};
    float *sensorVariables[] ={&bmp280_temp, &bmp280_press, &bmp280_alt};
        if(reconfigure){
        sendMqttSensorDiscoveryClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        sendMqttSensorValuesClear(sensor, SensorSuffix, sizeof(SensorSuffix)/sizeof(SensorSuffix[0]));
        return;
    }

            
    if(sensorStart==1)
    {
        bmp280.begin(0x76);

        bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
            Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
            Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
            Adafruit_BMP280::FILTER_X16,      /* Filtering. */
            Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */  
    

        String status_topic = mqttStatusTopic();
        // Sending MQTT Discovery messages for each PM sensor (pm1_0, pm2_5, pm10_0)
        
        sendMqttSensorDiscovery(sensor, status_topic, deviceClass, SensorSuffix, unitOfMeasurement, sizeof(unitOfMeasurement)/sizeof(unitOfMeasurement[0]));
        

        if(!lowPowerMode_toggle)
        {
            //BMP280_task = taskManager.scheduleFixedRate(refreshTime,bmp280SetupSend,TIME_SECONDS);
            //BMP280_running=1;
            runningTasks=1;
        }
        else
        {
            sensorStart = 0;
            bmp280SetupSend();
            sensorStart = 1;
        }
    }
    else 
    {
        bmp280Read();
        applySensorOffset(1);

        sendMqttSensorValues(sensor, SensorSuffix, sensorVariables, sizeof(sensorVariables)/sizeof(sensorVariables[0]));

    }
}