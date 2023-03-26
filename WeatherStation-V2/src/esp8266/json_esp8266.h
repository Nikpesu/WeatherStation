void loadConfig()
{
  // Check if JSON exists, if not make a new one
  if (!LittleFS.exists(JSON_FILE_PATH))
  {
    Serial.println("novi config -- loadConfig");
    saveNewConfig();
    Serial.println("novi config napravljen -- loadConfig");
  }

  // Open the JSON file for reading
  File configFile = LittleFS.open(JSON_FILE_PATH, "r");
  Serial.println("otvoren config -- loadConfig");

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
      // Retrieve the values of the variables from the JSON object
      wifi_ssid = (String)json["wifi_ssid"];
      wifi_pass = (String)json["wifi_pass"];
      mqtt_server = (String)json["mqtt_server"];
      mqtt_port = json["mqtt_port"];
      mqtt_user = (String)json["mqtt_user"];
      mqtt_messageRoot = (String)json["mqtt_messageRoot"];
      mdns_hostname = (String)json["mdns_hostname"];
      hotspot_ssid = (String)json["hotspot_ssid"];
      hotspot_pass = (String)json["hotspot_pass"];
      BMP280_toggle = json["BMP280_toggle"];
      SHT31_toggle = json["SHT31_toggle"];
      SGP30_toggle = json["SGP30_toggle"];
      mqtt_password = (String)json["mqtt_password"];
    }
    else
    {
      Serial.println("Failed to parse config file1 -- loadConfig");
    }

    Serial.println("f -- loadConfig");
  }
  else
  {
    Serial.println("Failed to open config file2  -- loadConfig");
  }
  Serial.println("gotovo -- loadConfig");
}
// Function to save the configuration to the JSON file
bool saveConfig()
{
  // Create a JSON object
  DynamicJsonDocument json(JSON_OBJECT_SIZE);
  Serial.println("-- started saving to JSON -- saveConfig");
  // Set the values of the variables in the JSON object
  // Serial.println(ESP.getFreeHeap());

  json["wifi_ssid"] = wifi_ssid;
  json["wifi_pass"] = wifi_pass;
  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_port;
  json["mqtt_user"] = mqtt_user;
  json["mqtt_password"] = mqtt_password;
  json["mqtt_messageRoot"] = mqtt_messageRoot;
  json["BMP280_toggle"] = BMP280_toggle;
  json["SHT31_toggle"] = SHT31_toggle;
  json["SGP30_toggle"] = SGP30_toggle;
  json["mdns_hostname"] = mdns_hostname;
  json["hotspot_ssid"] = hotspot_ssid;
  json["hotspot_pass"] = hotspot_pass;
  serializeJson(json, Serial);
  Serial.print("   copied to JSON -- saveConfig");
  // Open the JSON file for writing
  File configFile = LittleFS.open(JSON_FILE_PATH, "w");

  Serial.print("   opened config file -- saveConfig");
  if (configFile)
  {
    // Serialize the JSON object into a buffer
    serializeJson(json, configFile);
    configFile.close();
    Serial.print("-- copied to JSON -- saveConfig");
    return true;
  }
  else
  {
    Serial.println("Failed to open config file for writing -- saveConfig");
    return false;
  }
}

void saveNewConfig()
{
  wifi_ssid = "";
  wifi_pass = "";
  mqtt_server = "";
  mqtt_port = 1883;
  mqtt_user = "";
  mqtt_password = "";
  mqtt_messageRoot = "";
  BMP280_toggle = true;
  SHT31_toggle = false;
  SGP30_toggle = true;
  mdns_hostname = "weatherstation" + (String)rand();
  hotspot_ssid = mdns_hostname;
  hotspot_pass = "";

  saveConfig();
}
void readConfig()
{
  loadConfig();

  Serial.println(wifi_ssid);
  Serial.println(wifi_pass);
  Serial.println(mqtt_server);
  Serial.println(mqtt_port);
  Serial.println(mqtt_user);
  Serial.println(mqtt_password);
  Serial.println(mqtt_messageRoot);
  Serial.println(BMP280_toggle);
  Serial.println(SHT31_toggle);
  Serial.println(SGP30_toggle);
  Serial.println(mdns_hostname);
  Serial.println(hotspot_ssid);
  Serial.println(hotspot_pass);
}