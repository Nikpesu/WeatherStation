void pmsx003SetupSend();
void pmsx003Reconfigure()
{
    if(!PMSx003_toggle)
    {
        reconfigure=1;
        pmsx003SetupSend();
        reconfigure=0;
    }

    //taskManager.cancelTask(PMSx003_task);
        
    
    //Serial1.begin(PM1006K::BAUD_RATE, SERIAL_8N1, PMSX003_RX_pin, PMSX003_TX_pin);
    //pmsx003 = new PM1006K(&Serial1);
    pmsx003=SerialPM(pmsx003Type, PMSX003_RX_pin, PMSX003_TX_pin);
}


void pmsx003Read()
{
    /*
    pm01 	uint16_t 	PM 	<= 1.0 µm 	µg/m³ 	PM1.0, ultra fine particles
    pm25 	uint16_t 	PM 	<= 2.5 µm 	µg/m³ 	PM2.5, fine particles
    pm10 	uint16_t 	PM 	<= 10 µm 	µg/m³ 	PM10
    n0p3 	uint16_t 	NC 	>= 0.3 µm 	#/100 cm³ 	
    n0p5 	uint16_t 	NC 	>= 0.5 µm 	#/100 cm³ 	
    n1p0 	uint16_t 	NC 	>= 1.0 µm 	#/100 cm³ 	
    n2p5 	uint16_t 	NC 	>= 2.5 µm 	#/100 cm³ 	
    n5p0 	uint16_t 	NC 	>= 5.0 µm 	#/100 cm³ 	
    n10p0 	uint16_t 	NC 	>= 10 µm 	#/100 cm³ 	
    pm 	uint16_t[3] 	PM 	<= 1,2.5,10 µm 	µg/m³ 	array containing pm01,pm25,pm10
    nc 	uint16_t[6] 	NC 	>= 0.3,0.5,1,5,10 µm 	#/100cm³ 	array containing n0p3,..,n10p0
    data 	uint16_t[9] 	PM/NC 			all PM/NC data pm01,..,n10p0
    temp 	float 	T 		°C 	temperature
    rhum 	float 	RH 		% 	relative humidity
    hcho 	float 	HCHO 		mg/m³ 	formaldehyde concentration
    extra 	float[3] 	T/RH/HCHO 			array containing temp,rhum,hcho
    */
    pmsx003.read();
    if (pmsx003)
    { 
        if (pmsx003.has_number_concentration())
        {
            pmsx003_pm1_0=pmsx003.pm01;
            pmsx003_pm2_5=pmsx003.pm25;
            pmsx003_pm10_0=pmsx003.pm10;
        }
        if (pmsx003.has_temperature_humidity())
        {
            pmsx003_temp=pmsx003.temp;
            pmsx003_hum=pmsx003.rhum;
        }
        if (pmsx003.has_formaldehyde())
            pmsx003_hcho=pmsx003.hcho;
    }
}

String pmsx003TypeToString(PMS inSensor)
{
    if(inSensor==PLANTOWER_24B) return "PMS3003";
    if(inSensor==PLANTOWER_24B) return "PMS1003, PMS5003, PMS7003, PMSA003";
    if(inSensor==PLANTOWER_32B_S) return "PMS5003S";
    if(inSensor==PLANTOWER_32B_T) return "PMS5003T";
    if(inSensor==PLANTOWER_40B) return "PMS5003ST";
    return "PMS3003";
}

PMS pmsx003TypeFromString(String inSensor)
{
    if(inSensor=="PMS3003") return PLANTOWER_24B;
    if(inSensor=="PMS1003, PMS5003, PMS7003, PMSA003") return PLANTOWER_24B;
    if(inSensor=="PMS5003S") return PLANTOWER_32B_S;
    if(inSensor=="PMS5003T") return PLANTOWER_32B_T;
    if(inSensor=="PMS5003ST") return PLANTOWER_40B;
    return PLANTOWER_24B;
}

void pmsx003SetupSend()
{
    String sensor = "PMSx003";
    String SensorSuffix[] = {"pm1_0", "pm2_5", "pm10_0"};//,"temperature","humidity","formaldehyde"};// temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
    String deviceClass[] = {"pm1", "pm25", "pm10"};//, "temperature", "humidity", "None"};
    String unitOfMeasurement[] = {"µg/m³","µg/m³","µg/m³"};//, "°C", "%", "mg/m³"};
    
    
    float *sensorVariables[] ={&pmsx003_pm1_0, &pmsx003_pm2_5, &pmsx003_pm10_0};//, &pmsx003_temp, &pmsx003_hum, &pmsx003_hcho};
    if(reconfigure){
        for (int i = 0; i < sizeof(SensorSuffix)/sizeof(SensorSuffix[0]); i++)
            sendMqtt("homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config","",true);
            return;
    }

    if(sensorStart==1)
    {
        // https://github.com/kevinlutzer/Arduino-pmsx003/blob/main/examples/pmsx003test/pmsx003test.ino

        //TODO add PMSx003 sensor def
        pmsx003.init();


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
            //PMSx003_task= taskManager.scheduleFixedRate(refreshTime, pmsx003SetupSend, TIME_SECONDS);
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