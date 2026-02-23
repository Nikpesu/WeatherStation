void loadConfig()
{
  // Check if JSON exists, if not make a new one
  if (!LittleFS.exists(JSON_FILE_PATH) || !digitalRead(RESET_CONFIG_PIN))
  {
    Serial.println("["+runningTime()+"] new config making; loadConfig");
    saveConfig();
    Serial.println("["+runningTime()+"] new config made; loadConfig");
  }

  // Open the JSON file for reading
  File configFile = LittleFS.open(JSON_FILE_PATH, "r");
  Serial.println("["+runningTime()+"] config opened; loadConfig");

  if (configFile)
  {
    // Read the file content into a buffer
    char buffer[JSON_OBJECT_SIZE];
    configFile.readBytes(buffer, JSON_OBJECT_SIZE);

    // Parse the JSON object
    DynamicJsonDocument json(JSON_OBJECT_SIZE);
    DeserializationError error = deserializeJson(json, buffer);
    

    if (!error)
    {
      //Sensor toggles
      for(int i=0; i<SENSOR_COUNT; i++)
      {//toggleIDName: i0-Sensor_Toggle i1-SensorName
        *toggles[i]=json[toggleIDName[i][0]];
      }

      for(int i=0; i<FIELD_COUNT;i++)
      {
        String value = json[fieldsIDNameTypePlaceholder[i][0]].as<String>();
        String nochange = (String)NO_PASS_CHANGE;

        if (fieldsIDNameTypePlaceholder[i][2] == "password") {
            if (value != nochange) {
                *(fields[i]) = value;
            } else {
            }
        } else {
            *(fields[i]) = value;
        }

      }

      for(int i=0; i<PINS_COUNT; i++)
      {
        *pins[i]=(json[pinIDName[i][0]].as<String>()).toInt();
      }
      updateFieldsToNative();
      //serializeJson(json, Serial); Serial.println();
    }
  else
  {
    Serial.println("["+runningTime()+"] Failed to parse config file1; loadConfig");
  }

    Serial.println("["+runningTime()+"] Config loaded; loadConfig");
  }
  else
  {
    Serial.println("["+runningTime()+"] Failed to open config file2; loadConfig");
  }
  Serial.println("["+runningTime()+"] done; loadConfig");
}
// Function to save the configuration to the JSON file
bool saveConfig()
{
  // Create a JSON object
  DynamicJsonDocument json(JSON_OBJECT_SIZE);
  Serial.println("["+runningTime()+"] started saving to JSON; saveConfig");
  // Set the values of the variables in the JSON object
  // Serial.println(ESP.getFreeHeap());

  //Sensor toggles
  for(int i=0; i<SENSOR_COUNT; i++)
  {//toggleIDName: [i][0]-Sensor_Toggle(sht31_toggle) [i][1]-SensorName(SHT31)
    json[toggleIDName[i][0]]= *toggles[i];
  }
  for(int i=0; i<FIELD_COUNT;i++)
  {
    json[fieldsIDNameTypePlaceholder[i][0]]= *(fields[i]);
  } 
  for(int i=0; i<PINS_COUNT;i++)
  {
    json[pinIDName[i][0]]= *(pins[i]);
  } 



  serializeJson(json, Serial); Serial.println();
  
  Serial.println("["+runningTime()+"] copied to JSON; saveConfig ");
  // Open the JSON file for writing
  File configFile = LittleFS.open(JSON_FILE_PATH, "w");

  Serial.println("["+runningTime()+"] opened config file; saveConfig");
  if (configFile)
  {
    // Serialize the JSON object into a buffer
    serializeJson(json, configFile);
    configFile.close();
    Serial.println("["+runningTime()+"] copied to JSON; saveConfig");
    return true;
  }
  else
  {
    Serial.println("["+runningTime()+"] Failed to open config file for writing; saveConfig");
    return false;
  }
}

void updateFieldsToString()
{
  mqtt_port_str = String(mqtt_port);
  lowPowerMode_toggle_str = String(lowPowerMode_toggle);
  refreshTime_str = String(refreshTime);
}

void updateFieldsToNative()
{
  mqtt_port=mqtt_port_str.toInt();
  lowPowerMode_toggle=(lowPowerMode_toggle_str=="true"? true : false);
  refreshTime = refreshTime_str.toInt();
}