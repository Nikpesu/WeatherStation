// JSON-safe float-to-string: NaN/Inf → "null", otherwise the number.
inline String jsonFloat(float v)
{
  if (isnan(v) || isinf(v)) return "null";
  return String(v);
}

void callback(char *topic, byte *payload, unsigned int length) {}

void mqttSetup()
{
  client.setServer(mqtt_server.c_str(), mqtt_port);
  client.setCallback(callback);
  client.setBufferSize(1024);
}

bool mqttHasServerAndPort()
{
  return mqtt_server.length() > 0 && mqtt_port > 0;
}

bool mqttAttemptConnect(bool force)
{
  static unsigned long lastRetryAt = 0;
  static bool missingConfigWarned = false;

  mqttSetup();

  if (!mqttHasServerAndPort())
  {
    if (!missingConfigWarned)
    {
      Serial.println("[" + runningTime() + "] mqtt doest have ip/port");
      missingConfigWarned = true;
    }
    return false;
  }

  missingConfigWarned = false;

  if (client.connected())
  {
    if (!force)
    {
      return true;
    }
    client.disconnect();
  }

  if (!force)
  {
    unsigned long now = millis();
    if (lastRetryAt != 0 && (now - lastRetryAt) < 15000)
    {
      return false;
    }
    lastRetryAt = now;
  }
  else
  {
    lastRetryAt = millis();
  }

  Serial.println("[" + runningTime() + "] Attempting MQTT connection... WiFi: " + WiFi.status());
  String clientId = mdns_hostname + UniqueDeviceID;
  if (client.connect(clientId.c_str(), mqtt_user.c_str(), mqtt_password.c_str(), mqtt_messageRoot.c_str(), 1, true, "{\"state\":\"offline\"}"))
  {
    client.publish(mqtt_messageRoot.c_str(), "{\"state\":\"online\"}", true);
    Serial.println("[" + runningTime() + "] MQTT connected!");
    return true;
  }
  else
  {
    Serial.println("[" + runningTime() + "] MQTT failed, rc=" + client.state());
    return false;
  }
}

void mqttConnect()
{
  mqttAttemptConnect(true);
}

void sendMqtt(String topic, String msg, bool retain)
{ 
  if (!client.connected()) mqttAttemptConnect(false);  // Only send if connected
  if (!client.connected())
  {

    
    Serial.print("["+runningTime()+"] Couldnt send: \t"+topic);
    return;
  } 

  // Convert String to char arrays
  const char* topicChar = topic.c_str();
  const char* msgChar = msg.c_str();

  if (!client.publish(topicChar, msgChar, retain))
  {
    Serial.println("["+runningTime()+"] MQTT publish failed: \t"+topic);
    return;
  }

  char status_topic[mqtt_messageRoot.length() + 1];
  mqtt_messageRoot.toCharArray(status_topic, mqtt_messageRoot.length() + 1);
  client.publish(status_topic, "{\"state\":\"online\"}", true);  // Send initial online message (retained)

  Serial.println("["+runningTime()+"] Sent topic: \t"+topic);
  int pos=0;
  int msgLen=160;
  if(msg.length()>msgLen){
    while(pos < (int)msg.length())
    {
      int len=msgLen;
      if(pos+msgLen > (int)msg.length())
        len=msg.length()-pos;
      Serial.println("\t\t"+msg.substring(pos,pos+len));
      pos+=msgLen;
    }
  }
  else
  {
    Serial.println("\t\t"+msg);
  }
  //Serial.println(topic);
  //Serial.println(msg);
}
String makeMqttSensorTopic(int i, String sensor, String status_topic, String *deviceClass, String *SensorSuffix,  String *unitOfMeasurement)
{
  return  "{"
          "\"availability\": ["
            "{"
            "\"topic\": \""+String(status_topic)+"\","
            "\"value_template\": \"{{ value_json.state }}\""
            "}"
          "],"
          "\"device\": {"
            "\"configuration_url\": \"http://"+mdns_hostname+".local\","
            "\"hw_version\": \""+HWversion+"\","
            "\"identifiers\": ["
              "  \"Weatherstation_"+UniqueDeviceID+"\""
            "],"
            "\"manufacturer\": \""+Manufacturer+"\","
            "\"model\": \""+Model+"\","
            "\"model_id\": \""+ModelID+"\","
            "\"name\": \""+ mdns_hostname +"\","
            "\"sw_version\": \""+SWversion+"\","
            //"\"via_device\": \""+(String)device+"\","
            "\"suggested_area\": \""+suggested_area+"\""
          "}"
          ","
          "\"device_class\": \"" + (String)*(deviceClass+i) + "\"," // temperature, pressure, humidity, pm1, pm25...   https://www.home-assistant.io/integrations/homeassistant/#device-class
          "\"enabled_by_default\": true,"
          // HA turns object_id into the entity_id: sensor.<object_id> ->
          // e.g. sensor.sht31_temperature_wsnikosoba
          "\"object_id\": \""  + sensor + "_" + (String)*(SensorSuffix+i)+ "_" + mdns_hostname  + "\"," // sps30_pm1_0_wstestHostname
          "\"object_name\": \""  + sensor + "_" + (String)*(SensorSuffix+i)+ "_" + mdns_hostname  + "\"," // sps30_pm1_0_wstestHostname
          "\"name\": \""  + sensor + " " + (String)*(SensorSuffix+i)+ "\"," // sps30_pm1_0_wstestHostname
          "\"origin\": {"
            "\"name\": \"Weatherstation\","
            "\"sw\": \"" + SWversion + "\"," //1.0
            "\"url\": \"https://github.com/Nikpesu/WeatherStation\""
          "}"
          ","
          "\"unit_of_measurement\": \""+ (String)*(unitOfMeasurement+i) +"\"," // C*, RH%...
          // NOTE: bumping the DISCOVERY_UID_VER suffix forces Home Assistant to treat
          // these as brand-new entities, so a stale entity_id (kept in HA's registry
          // keyed by unique_id, even after deleting the device) is dropped and the
          // entity_id is rebuilt from object_id above. Bump it again if you ever need
          // another clean rename.
          "\"unique_id\": \"" + UniqueDeviceID + "_" + sensor + "_" + mdns_hostname + "_" + (String)*(SensorSuffix+i) + "_" + DISCOVERY_UID_VER + "\"," // xxxxx_sps30_wstestHostname_pm1_0_v2
          "\"value_template\": \"{{ value_json."+ (String)*(SensorSuffix+i)+" }}\"," // value_json.pm1_0 ...
          "\"state_topic\": \""+mqtt_messageRoot+"/"+sensor+"\"," // wstest1/sps30
          "\"state_class\": \"measurement\""
          "}";
}