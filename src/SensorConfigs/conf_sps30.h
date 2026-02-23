void sps30SetupSend();

void sps30Reconfigure() {
    if (!SPS30_toggle) {
        reconfigure = 1;
        sps30SetupSend();
        reconfigure = 0;
    }
    sps30.begin(Wire, SPS30_I2C_ADDR_69);
    sps30.startMeasurement(SPS30_OUTPUT_FORMAT_OUTPUT_FORMAT_FLOAT);
}

void sps30Read() {
    #ifdef NO_ERROR
    #undef NO_ERROR
    #endif
    #define NO_ERROR 0

    static char errorMessage[64];
    int16_t error;
    uint16_t dataReadyFlag = 0;
    
    // Internal variables for the sensor's float output
    float mc1p0, mc2p5, mc4p0, mc10p0;
    float nc0p5, nc1p0, nc2p5, nc4p0, nc10p0;
    float typicalParticleSize;

    // 1. Start measurement (In float mode)
    sps30.startMeasurement(SPS30_OUTPUT_FORMAT_OUTPUT_FORMAT_FLOAT);
    
    // 2. Poll for data ready (max 2.5 seconds)
    int tries = 0;
    while (tries < 25) {
        error = sps30.readDataReadyFlag(dataReadyFlag);
        if (error == NO_ERROR && dataReadyFlag == 1) break; 
        delay(100);
        tries++;
    }

    if (error != NO_ERROR || dataReadyFlag == 0) {
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println("[" + runningTime() + "] SPS30 error (Timeout/Ready): " + errorMessage);
        
        // Set values to NaN so MQTT sends null rather than old data
        sps30_pm1_0 = sps30_pm2_5 = sps30_pm10_0 = nan("");
    } else {
        // 3. Read the values
        error = sps30.readMeasurementValuesFloat(mc1p0, mc2p5, mc4p0, mc10p0,
                                                 nc0p5, nc1p0, nc2p5, nc4p0,
                                                 nc10p0, typicalParticleSize);
        if (error != NO_ERROR) {
            errorToString(error, errorMessage, sizeof errorMessage);
            Serial.println("[" + runningTime() + "] SPS30 error (Read): " + errorMessage);
        } else {
            // FIX: Use 'mc' (Mass Concentration) for PM values, not 'nc' (Number Concentration)
            sps30_pm1_0 = mc1p0;
            sps30_pm2_5 = mc2p5;
            sps30_pm10_0 = mc10p0;
        }
    }

    // 4. Stop measurement to save the fan/laser life (ideal for low power)
    //sps30.stopMeasurement();
}

void sps30SetupSend() {
    String sensor = "SPS30";
    String SensorSuffix[] = {"pm1_0", "pm2_5", "pm10_0"};
    String deviceClass[] = {"pm1", "pm25", "pm10"};
    String unitOfMeasurement[] = {"µg/m³", "µg/m³", "µg/m³"};
    
    float *sensorVariables[] = {&sps30_pm1_0, &sps30_pm2_5, &sps30_pm10_0};

    if (reconfigure) {
        for (int i = 0; i < 3; i++)
            sendMqtt("homeassistant/sensor/" + mdns_hostname + "_" + UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config", "", true);
        return;
    }

    if (sensorStart == 1) {
        sps30.begin(Wire, SPS30_I2C_ADDR_69);
        sps30.stopMeasurement(); // Start with a clean slate

        char status_topic[mqtt_messageRoot.length() + 1];
        mqtt_messageRoot.toCharArray(status_topic, mqtt_messageRoot.length() + 1);

        for (int i = 0; i < 3; i++) {
            sendMqtt(
                "homeassistant/sensor/" + mdns_hostname + "_" + UniqueDeviceID + "/" + sensor + "_" + SensorSuffix[i] + "/config",
                makeMqttSensorTopic(i, sensor, status_topic, &deviceClass[0], &SensorSuffix[0], &unitOfMeasurement[0]),
                true
            );
        }

        if (!lowPowerMode_toggle) {
            runningTasks = 1;
        } else {
            if (SPS30_toggle) sps30SetupSend();
        }
    } else {
        sps30Read();
        String msg = "{";
        for (int i = 0; i < 3; i++) {
            msg += "\"" + SensorSuffix[i] + "\":" + String(*sensorVariables[i]) + ",";
        }
        msg.remove(msg.length() - 1);
        msg += "}";
        sendMqtt(mqtt_messageRoot + "/" + sensor, msg, true);
    }
}