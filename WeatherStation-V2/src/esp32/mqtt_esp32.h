void callback(char *topic, byte *payload, unsigned int length) {}

void mqttSetup()
{
  IPAddress ipaddr;
  ipaddr.fromString(mqtt_server);
  client.setServer(ipaddr, mqtt_port);
  client.setCallback(callback);
  client.setBufferSize(1024);
}

void mqttConnect()
{
  mqttSetup();
  if(client.connected()) client.disconnect();
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.println("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = mdns_hostname+ESP.getEfuseMac();

    // Attempt to connect
    Serial.println(WiFi.status());
    if (client.connect(clientId.c_str(), mqtt_user.c_str(), mqtt_password.c_str(), mqtt_messageRoot.c_str(), 1, true, "{\"state\":\"offline\"}"))
    {
      client.publish(mqtt_messageRoot.c_str(), "{\"state\":\"online\"}", true);  // Send initial online message (retained)
      Serial.println(" SUCCESS!");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      break;
    }
  }
}

void sendMqtt(String topic, String msg, bool retain)
{ 
  if (!client.connected()) mqttConnect();  // Only send if connected
  if (!client.connected())
  {
    Serial.print("[");
    Serial.print(runningTime());
    Serial.print("] Couldnt send: ");
    Serial.println(topic);
    return;
  } 
  Serial.print("[");
  Serial.print(runningTime());
  Serial.print("] Sent topic: ");
  Serial.println(topic);
  Serial.println(msg);

  // Convert String to char arrays
  const char* topicChar = topic.c_str();
  const char* msgChar = msg.c_str();

  client.publish(topicChar, msgChar, retain);

  char status_topic[mqtt_messageRoot.length() + 1];
  mqtt_messageRoot.toCharArray(status_topic, mqtt_messageRoot.length() + 1);
  client.publish(status_topic, "{\"state\":\"online\"}", true);  // Send initial online message (retained)

  //Serial.println(topic);
  //Serial.println(msg);
}

