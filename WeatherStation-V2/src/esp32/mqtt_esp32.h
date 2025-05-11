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
    Serial.println("["+runningTime()+"] Attempting MQTT connection...");
    // Create a random client ID
    String clientId = mdns_hostname+ESP.getEfuseMac();

    // Attempt to connect
    Serial.println("["+runningTime()+"] WiFi Status: "+WiFi.status());
    if (client.connect(clientId.c_str(), mqtt_user.c_str(), mqtt_password.c_str(), mqtt_messageRoot.c_str(), 1, true, "{\"state\":\"offline\"}"))
    {
      client.publish(mqtt_messageRoot.c_str(), "{\"state\":\"online\"}", true);  // Send initial online message (retained)
      Serial.println("["+runningTime()+"] SUCCESS!");
    }
    else
    {
      Serial.print("["+runningTime()+"] failed, rc="+client.state());
      break;
    }
  }
}

void sendMqtt(String topic, String msg, bool retain)
{ 
  if (!client.connected()) mqttConnect();  // Only send if connected
  if (!client.connected())
  {

    
    Serial.print("["+runningTime()+"] Couldnt send: \t"+topic);
    return;
  } 

  // Convert String to char arrays
  const char* topicChar = topic.c_str();
  const char* msgChar = msg.c_str();

  client.publish(topicChar, msgChar, retain);

  char status_topic[mqtt_messageRoot.length() + 1];
  mqtt_messageRoot.toCharArray(status_topic, mqtt_messageRoot.length() + 1);
  client.publish(status_topic, "{\"state\":\"online\"}", true);  // Send initial online message (retained)

  Serial.println("["+runningTime()+"] Sent topic: \t"+topic);
  int pos=0;
  int msgLen=160;
  if(msg.length()>msgLen){
    while(msg.length()>pos)
    {
      int len=msgLen;
      String tmpmsg=msg;
      if(msg.length()<pos+msgLen)
      {
        len=pos-msg.length()-1;
      }
      tmpmsg=msg.substring(pos,pos+len);
      Serial.println("\t\t"+tmpmsg);
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

