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
void SCD4XSend()
{
  SCD4XRead();
  sendMqtt((mqtt_messageRoot + "/" + "SCD4x/temp"), (String)scd4x_temp, true);
  sendMqtt((mqtt_messageRoot + "/" + "SCD4x/hum"), (String)scd4x_hum, true);
  sendMqtt((mqtt_messageRoot + "/" + "SCD4x/CO2"), (String)scd4x_co2, true);
  sendMqtt((mqtt_messageRoot + "/" + "SCD4x/status"), (String)scd4x.readMeasurement(), true);

}

void PMSx003Send()
{
  PMSx003Read();
  String parametri[12] = {"pm1.0", "pm2.5", "pm10", "n0p3", "n0p5", "n1p0", "n2p5", "n5p0", "n10p0", "temp", "hum", "hcho"};
  for(int i=0; i<9; i++)
  {
    sendMqtt((mqtt_messageRoot + "/" + "PMSx003/"+parametri[i]), (String)pmsx003Data[i], true);
  }
  for(int i=0; i<3; i++)
  {
    sendMqtt((mqtt_messageRoot + "/" + "PMSx003/"+parametri[i+9]), (String)pmsx003Extra[i], true);
  }

}