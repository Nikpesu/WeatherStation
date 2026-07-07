void bmp580SetupSend();
void bmp580Reconfigure()
{
    if(!BMP580_toggle)
    {
        reconfigure=1;
        bmp580SetupSend();
        reconfigure=0;
    }

    bmp580 = BMP581();
}

void bmp580Read()
{
  // BMP580/581 share the Bosch BMP5 register set; the SparkFun BMP581 driver
  // returns pressure in Pa and temperature in °C. Altitude is derived from the
  // pressure with the standard barometric formula (sea-level 1013.25 hPa).
  bmp5_sensor_data data = {0, 0};
  if(bmp580.getSensorData(&data) == BMP5_OK)
  {
    bmp580_temp  = data.temperature;
    bmp580_press = data.pressure / 100.0f;                 // Pa -> hPa
    bmp580_alt   = 44330.0f * (1.0f - pow(bmp580_press / 1013.25f, 1.0f / 5.255f));
  }
  else
  {
    bmp580_temp = bmp580_press = bmp580_alt = nan("");
  }
}

void bmp580SetupSend()
{
    String sensor = "BMP580";
    String SensorSuffix[] = {"temperature","pressure","altitude"};
    String deviceClass[] = {"temperature","pressure","distance"};  // https://www.home-assistant.io/integrations/homeassistant/#device-class
    String unitOfMeasurement[] = {"°C","hPa","m"};
    float *sensorVariables[] ={&bmp580_temp, &bmp580_press, &bmp580_alt};
    if(reconfigure){
        for (int i = 0; i < sizeof(SensorSuffix)/sizeof(SensorSuffix[0]); i++)
            sendMqtt("homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config","",true);
            return;
    }


    if(sensorStart==1)
    {
        bmp580.beginI2C(BMP581_I2C_ADDRESS_DEFAULT); // 0x47 (use _SECONDARY 0x46 if the board straps it differently)

        char status_topic[mqtt_messageRoot.length() + 1];
        mqtt_messageRoot.toCharArray(status_topic, mqtt_messageRoot.length() + 1);

        for (int i = 0; i < sizeof(unitOfMeasurement)/sizeof(unitOfMeasurement[0]); i++)
            sendMqtt(
                "homeassistant/sensor/" + mdns_hostname+"_"+UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config",
                makeMqttSensorTopic(i, sensor, status_topic, &deviceClass[0], &SensorSuffix[0],  &unitOfMeasurement[0]),
                true
            );


        if(!lowPowerMode_toggle)
            runningTasks=1;
        else
        {
            sensorStart = 0;
            bmp580SetupSend();
            sensorStart = 1;
        }
    }
    else
    {
        bmp580Read();
        applySensorOffset(9);

        String msg="{";
        for (int i=0; i<sizeof(sensorVariables)/sizeof(sensorVariables[0]); i++)
        {
            msg+="\""+SensorSuffix[i]+"\":"+jsonFloat(*(sensorVariables[i]))+",";
        }
        msg.remove(msg.length() - 1);
        msg+="}";

        sendMqtt(mqtt_messageRoot+"/"+sensor, msg,true);

    }
}
