

String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}
String rssiToChart(int8_t value){
  if(value>-70) return "[▂▃▅▇]";
  else if(value>-85) return "[▂▃▅▁]";
  else if(value>-100) return "[▂▃▁▁]";
  else return "[▂▁▁▁]";
}

void httpfields()
{			
  //const data = [{"fieldID":"fieldID","labelText":"labelText","value":"value","placeholder":"placeholder"}];
  
  updateFieldsToString();
  String sendingValue="[";
  for(int i=0; i<FIELD_COUNT;i++)
  {
    sendingValue+="{\"fieldID\":\""+fieldsIDNameTypePlaceholder[i][0]+"\",";
    sendingValue+="\"labelText\":\""+fieldsIDNameTypePlaceholder[i][1]+"\",";
    sendingValue+="\"type\":\""+fieldsIDNameTypePlaceholder[i][2]+"\",";
    sendingValue+="\"placeholder\":\""+fieldsIDNameTypePlaceholder[i][3]+"\",";
    if(fieldsIDNameTypePlaceholder[i][2]=="password")
      sendingValue+="\"value\":\""+((String)NO_PASS_CHANGE)+"\"}";
    else
      sendingValue+="\"value\":\""+((String)*(fields[i]))+"\"}";
    sendingValue+=(i<FIELD_COUNT-1? ",":"");
  }

  sendingValue+="]";
  
  server.send(200, "application/json",sendingValue);
}
void httpsensorAndFieldsIDs()
{
  String sendingValue="[";
  for(int i=0; i<SENSOR_COUNT; i++)
  {
    sendingValue+="\""+toggleIDName[i][0]+"\""+",";
  }
  for(int i=0; i<FIELD_COUNT; i++)
  {
    sendingValue+="\""+fieldsIDNameTypePlaceholder[i][0]+"\""+(i<FIELD_COUNT-1? ",":"");
  }
  sendingValue+="]";
  server.send(200, "application/json",sendingValue);

}
void httpWiFi()
{
  WiFi.scanDelete();
  int networkNum=WiFi.scanNetworks(false, true);
  
  String sendingValue="[";
  for(int i=0; i<networkNum;i++)
  {
    sendingValue+="{\"ssid\":\""+WiFi.SSID(i)+"\",";
    sendingValue+="\"rssi\":\""+(String)WiFi.RSSI(i)+rssiToChart(WiFi.RSSI(i))+"\"}"+(i<networkNum-1? ",":"");
  }

  sendingValue+="]";
  
  server.send(200, "application/json",sendingValue);
}
void httpSensors()
{
  String sendingValue="[";
  for(int i=0; i<SENSOR_COUNT; i++)
  {

    sendingValue+="{\"sensorID\":\""+toggleIDName[i][0]+"\",";
    sendingValue+="\"sensorName\":\""+toggleIDName[i][1]+"\",";
    sendingValue+="\"sensorToggle\":" + (String)( (bool)*(toggles[i]) ? "true" : "false" ) +"}"+(i<SENSOR_COUNT-1? ",":"");
    

  }
  sendingValue+="]";
  server.send(200, "application/json",sendingValue);

}
void httpTitles()
{
  String title3;

  if(wifiConnectionType) 
    title3=runningTime() + " | RSSI: " + WiFi.RSSI() + rssiToChart(WiFi.RSSI());
  else 
    title3=runningTime();

  server.send(200, "application/json", 
    "{"
    "\"title1\":\""+mdns_hostname+"\","
    "\"title2\":\"SSID: "+ (wifiConnectionType ? WiFi.SSID() : WiFi.softAPSSID())+" | IP: "+(wifiConnectionType ? WiFi.localIP() : WiFi.softAPIP()).toString()+"\","
    "\"title3\":\"Uptime:"+title3+"\""
    "}");
}
void httpRestart()
{
  server.send(200, "application/json", "{\"restart\":\"success\"}");
  delay(200);
  rst();
}
void httpConfig()
{
  
  updateFieldsToString();
  String sendingValue=htmlStart+"<body> <h1>CONFIG:</h1>";
  for(int i=0; i<FIELD_COUNT;i++)
  {
    sendingValue+="<br>"+fieldsIDNameTypePlaceholder[i][0]+": ";
    if(fieldsIDNameTypePlaceholder[i][2]=="password")
      sendingValue+="redacted";
    else
      sendingValue+=((String)*(fields[i]));
  }
  
  for(int i=0; i<SENSOR_COUNT;i++)
  {
    sendingValue+="<br>"+toggleIDName[i][0]+": "+(String)( (bool)*(toggles[i]) ? "true" : "false" );
  }

  sendingValue+="<br><a href=\"http://"+IpAddress2String(WiFi.localIP())+"/\"><br><input id=\"subm\" type=\"button\" value=\"Back\"></a></body></html>";
  server.send(200, "text/html", sendingValue);
}
void httpDefault()
{
  String ipda = mdns_hostname;
  ipda += ".local";
  Serial.println("["+runningTime()+"] mDNS hostname: "+ipda);
  server.sendHeader("Location", "http://" + ipda, true); // Redirect to the full URL
  server.send(302, "text/plain", "");
  server.client().stop();
}
void httpHome()
{
  //loadConfig();
  File file = LittleFS.open("/index.html", "r");
  String htmlContent = file.readString();
  file.close();
  server.send(200, "text/html", htmlContent);

}

void httpData()
{
  DynamicJsonDocument jsonDoc(JSON_OBJECT_SIZE);
  DeserializationError error = deserializeJson(jsonDoc, server.arg(0));
  //serializeJson(jsonDoc, Serial);
  String oldWiFiPass=wifi_pass;
  String oldWiFiSSID=wifi_ssid;
  if (error)
  {
    Serial.println("["+runningTime()+"] Failed to parse JSON; httpData");
    
    server.send(418, "application/json", "{\"message\": \"bad!\"}");
    return;
  }
  else
  {
      
    if((mdns_hostname!=jsonDoc["mdns_hostname"].as<String>()|| mqtt_messageRoot!=jsonDoc["mqtt_messageRoot"].as<String>() || jsonDoc["wifi_ssid"].as<String>()=="") && wifiConnectionType )
    {
      for(int i=0; i<SENSOR_COUNT; i++)
      { //toggleIDName: i0-Sensor_Toggle i1-SensorName
        *toggles[i]=0;
      }
      
      sensorsBegin();//remove sensors from hassio
    }
    //Sensor toggles
    for(int i=0; i<SENSOR_COUNT; i++)
    {//toggleIDName: i0-Sensor_Toggle i1-SensorName
      *toggles[i]=jsonDoc[toggleIDName[i][0]];
    }
    for(int i=0; i<FIELD_COUNT;i++)
    {
      String value = jsonDoc[fieldsIDNameTypePlaceholder[i][0]].as<String>();
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
    updateFieldsToNative();



    Serial.println("["+runningTime()+"] done pasrsing JSON; httpData\t");
    saveConfig();
    jsonDoc.clear();
    loadConfig();
    

    if(wifiConnectionType)
    {
      if(wifi_ssid=="")
      {
        server.send(200, "application/json", "{\"message\": \"restarting device and turning on hotspot\"}");
        Serial.println("["+runningTime()+"] restarting device and turning on hotspot");
        rst();
      }


      if(oldWiFiSSID!=wifi_ssid || oldWiFiPass!=wifi_pass || (String)WiFi.getHostname()!=mdns_hostname)
      {
        server.send(200, "application/json", "{\"message\": \"restarting sensors and connecting to new WiFi! with "+mdns_hostname+".local hostname\",\"new_mdns\":\"http://"+mdns_hostname+".local\"}");
        Serial.println("["+runningTime()+"] restarting sensors and connecting to new WiFi! With "+mdns_hostname+".local hostname");
        delay(200);
        wifiStart();
        sensorsBegin();
      }
      else
      {
        server.send(200, "application/json", "{\"message\": \"restarting sensors\"}");
        Serial.println("["+runningTime()+"] restarting sensors");
        sensorsBegin();
      }
      
    }
    else 
    {
      server.send(200, "application/json", "{\"message\": \"restarting device\"}");
      Serial.println("["+runningTime()+"] restarting device");
      rst();
    }
  }
}
void httpServicesStart()
{
  server.on("/", httpHome);
  server.on("/data", httpData);
  server.on("/restart", httpRestart);
  server.on("/titles", httpTitles);
  server.on("/sensors", httpSensors);
  server.on("/currentConfig", httpConfig);
  server.on("/wifi", httpWiFi);
  server.on("/fields", httpfields);
  server.on("/sensorAndFieldsIDs", httpsensorAndFieldsIDs);
  server.onNotFound(httpDefault);
  server.begin();
}
long  int timeout=0;
void wifiStart()
{
  A:
  timeout=millis()+10000;
  

  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(mdns_hostname);
  WiFi.setHostname(mdns_hostname.c_str());
  //WiFi.hostname("weatherstation");
  if(!WiFi.status()==WL_CONNECTED) WiFi.disconnect();
  WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
  Serial.print("["+runningTime()+"] WI-FI starting");
  while (!WiFi.isConnected())
  {
    delay(500);
    Serial.print(".");
    if(millis()>timeout) goto A;
  }

  Serial.println("\n["+runningTime()+"] WI-FI started!");
  if (MDNS.begin(mdns_hostname.c_str()))
    Serial.println("["+runningTime()+"] MDNS started!");
    
  Serial.println("["+runningTime()+"] WI-FI info:");
  Serial.println("\t\tHostname: " + (String)WiFi.getHostname()+".local");
  Serial.println("\t\tIP: "+ WiFi.localIP().toString());
  Serial.println("\t\tRRSI: "+ (String)WiFi.RSSI());
  httpServicesStart();

  #if defined(ESP8266)
    MDNS.addService("http", "tcp", 80);
  #elif defined(ESP32)
    mdns_service_add(mdns_hostname.c_str(), "_http", "_tcp", 80, NULL, 0);
  #endif

  //mdns_service_add(mdns_hostname.c_str(), "_http", "_tcp", 80, NULL, 0);
  //ArduinoOTA.setMdnsEnabled(false);
  ArduinoOTA.begin();

}

void apStart()
{
  Serial.println("["+runningTime()+"] AP starting");

  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.softAP(hotspot_ssid.c_str(), hotspot_pass.c_str());

  WiFi.begin();
  
  Serial.println("["+runningTime()+"] AP started!");
  if (MDNS.begin(mdns_hostname.c_str()))
    Serial.println("["+runningTime()+"] MDNS started!");
  Serial.println("["+runningTime()+"] HOTSPOT info:");
  Serial.println("\t\t SSID: "+ (String)WiFi.softAPSSID());
  Serial.println("\t\t IP: "+ (String)WiFi.softAPIP().toString());



  httpServicesStart();


  dnsServer.start(53, "*", WiFi.softAPIP());

}
