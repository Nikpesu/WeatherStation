void callback(char *topic, byte *payload, unsigned int length) {}

void mqttSetup()
{
  IPAddress ipaddr;
  ipaddr.fromString(mqtt_server);
  client.setServer(ipaddr, mqtt_port);
  client.setCallback(callback);
  mqttSend();
}

void mqttSend()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    String clientUser = (String)mqtt_messageRoot, clientPass = (String)mqtt_password;
    if (client.connect(clientId.c_str(), clientUser.c_str(), clientPass.c_str()))
    {
      char wsChar[mqtt_messageRoot.length() + 1];
      mqtt_messageRoot.toCharArray(wsChar, mqtt_messageRoot.length() + 1);
      client.publish(wsChar, "1", true);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      break;
    }
  }
}

void sendMqtt(String topic, String msg, bool retain)
{
  char topicChar[topic.length()+1];
  topic.toCharArray(topicChar, topic.length()+1);
  char msgChar[msg.length()+1];
  msg.toCharArray(msgChar, msg.length()+1);
  mqttSend();
  client.publish(topicChar, msgChar, retain);
}

void sht31Send()
{
  sht31Read();
  sendMqtt((mqtt_messageRoot + "/" + "SHT31/temp"), (String)sht31_temp, true);
  sendMqtt((mqtt_messageRoot + "/" + "SHT31/hum"), (String)sht31_hum, true);
  sendMqtt((mqtt_messageRoot + "/" + "SHT31/status"), (String)sht31.readStatus(), true);
}

void sgp30Send()
{
  sgp30Read();
  sendMqtt((mqtt_messageRoot + "/" + "SGP30/TVOC"), (String)sgp30_tvoc, true);
  sendMqtt((mqtt_messageRoot + "/" + "SGP30/eCO2"), (String)sgp30_co2, true);
  sendMqtt((mqtt_messageRoot + "/" + "SGP30/ETH"), (String)sgp30_eth, true);
  sendMqtt((mqtt_messageRoot + "/" + "SGP30/H2"), (String)sgp30_h2, true);
}

void bmp280Send()
{
  bmp280Read();
  sendMqtt((mqtt_messageRoot + "/" + "BMP280/temp"), (String)bmp280_temp, true);
  sendMqtt((mqtt_messageRoot + "/" + "BMP280/press"), (String)bmp280_press, true);
  sendMqtt((mqtt_messageRoot + "/" + "BMP280/alt"), (String)bmp280_alt, true);
  sendMqtt((mqtt_messageRoot + "/" + "BMP280/status"), (String)bmp280.getStatus(), true);
}

void AHT2xSend()
{
  AHT2xRead();
  sendMqtt((mqtt_messageRoot + "/" + "AHT2x/temp"), (String)aht2x_temp, true);
  sendMqtt((mqtt_messageRoot + "/" + "AHT2x/hum"), (String)aht2x_hum, true);
  sendMqtt((mqtt_messageRoot + "/" + "AHT2x/status"), (String)aht20.getStatus(), true);
}
void ENS160Send()
{
  ENS160Read();
  sendMqtt((mqtt_messageRoot + "/" + "ENS160/AQI"), (String)ens160_aqi, true);
  sendMqtt((mqtt_messageRoot + "/" + "ENS160/TVOC"), (String)ens160_tvoc, true);
  sendMqtt((mqtt_messageRoot + "/" + "ENS160/ECO2"), (String)ens160_eco2, true);
  sendMqtt((mqtt_messageRoot + "/" + "ENS160/status"), (String)myENS.getFlags(), true); //Gas Sensor Status Flag (0 - Standard, 1 - Warm up, 2 - Initial Start Up): 
}