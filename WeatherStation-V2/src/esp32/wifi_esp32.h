

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


void httpWiFi()
{
  WiFi.scanDelete();
  int networkNum=WiFi.scanNetworks(false, true);
  
  String networkJson="[";
  for(int i=0; i<networkNum;i++)
  {
    networkJson+="{\"ssid\":\""+WiFi.SSID(i)+"\",";
    networkJson+="\"rssi\":\""+(String)WiFi.RSSI(i)+rssiToChart(WiFi.RSSI(i))+"\"}"+(i<networkNum-1? ",":"");
  }

  networkJson+="]";
  
  server.send(200, "application/json",networkJson);
}

void httpSensors()
{
  String sensorsJson="[";
  for(int i=0; i<SENSOR_COUNT; i++)
  {

    sensorsJson+="{\"sensorID\":\""+toggleIDName[i][0]+"\",";
    sensorsJson+="\"sensorName\":\""+toggleIDName[i][1]+"\",";
    sensorsJson+="\"sensorToggle\":" + (String)( (bool)*(toggles[i]) ? "true" : "false" ) +"}"+(i<SENSOR_COUNT-1? ",":"");
    

  }
  sensorsJson+="]";
  server.send(200, "application/json",sensorsJson);

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
    String ispis=htmlStart;
    ispis+="<body> <h1>CONFIG:</h1>";
    ispis+="<br><br>Wifi_SSID: "+ wifi_ssid;
    ispis+="<br>mqtt_server: "+ mqtt_server+":"+mqtt_port;
    ispis+="<br>mqtt_user: "+ mqtt_user;
    ispis+="<br>mqtt_messageRoot: "+ mqtt_messageRoot;
    ispis+="<br>mqtt_pass: "+ mqtt_password;
    //Sensor toggles
    for(int i=0; i<SENSOR_COUNT; i++)
    {//toggleIDName: i0-Sensor_Toggle i1-SensorName
      ispis+="<br>"+toggleIDName[i][1]+": "+ *toggles[i];
    }
    ispis+="<br>mdns_hostname: "+ mdns_hostname;
    ispis+="<br>hotspot_ssid: "+ hotspot_ssid;
    ispis+="<br>hotspot_pass: "+ hotspot_pass;
    ispis+="<br>refreshTime: "+ (String)refreshTime;
    ispis+="<br>lowPowerMode_toggle: "+ (String)lowPowerMode_toggle;
    ispis=ispis+"<a href=\"http://"+IpAddress2String(WiFi.localIP())+"/\"><br><input id=\"subm\" type=\"button\" value=\"Back\"></a>";
    ispis+="</body></html>";
  server.send(200, "text/html", ispis);
}

void httpDefault()
{
  String ipda = mdns_hostname;
  ipda += ".local";
  Serial.println(ipda);
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
  htmlContent.replace("\"wifi_ssid\" value=\"\"", "\"wifi_ssid\" value=\"" + wifi_ssid + "\"");
  htmlContent.replace("\"wifi_pass\" value=\"\"", "\"wifi_pass\" value=\"" + (String)NO_PASS_CHANGE + "\"");
  htmlContent.replace("\"mqtt_server\" value=\"\"", "\"mqtt_server\" value=\"" + mqtt_server + "\"");
  htmlContent.replace("\"mqtt_port\" value=\"\"", "\"mqtt_port\" value=\"" + (String)mqtt_port + "\"");
  htmlContent.replace("\"mqtt_user\" value=\"\"", "\"mqtt_user\" value=\"" + mqtt_user + "\"");
  htmlContent.replace("\"mqtt_password\" value=\"\"", "\"mqtt_password\" value=\"" + (String)NO_PASS_CHANGE + "\"");
  htmlContent.replace("\"mqtt_messageRoot\" value=\"\"", "\"mqtt_messageRoot\" value=\"" + mqtt_messageRoot + "\"");
  htmlContent.replace("\"mdns_hostname\" value=\"\"", "\"mdns_hostname\" value=\"" + mdns_hostname + "\"");
  htmlContent.replace("\"hotspot_ssid\" value=\"\"", "\"hotspot_ssid\" value=\"" + hotspot_ssid + "\"");
  htmlContent.replace("\"hotspot_pass\" value=\"\"", "\"hotspot_pass\" value=\"" + (String)NO_PASS_CHANGE + "\"");

  if(lowPowerMode_toggle) htmlContent.replace("id=\"lowPowerMode_toggle\"", "id=\"lowPowerMode_toggle\" checked");

  htmlContent.replace("\"refreshTime\" value=\"\"", "\"refreshTime\" value=\"" + (String)refreshTime + "\"");
  server.send(200, "text/html", htmlContent);

}

void httpData()
{
  DynamicJsonDocument jsonDoc(JSON_OBJECT_SIZE);
  DeserializationError error = deserializeJson(jsonDoc, server.arg(0));
  String oldSSID=wifi_ssid;
  String oldWiFiPass=wifi_pass;
  int oldMqttPort=mqtt_port;
  String oldMqttIP=mqtt_server;
  String oldMqttPass=mqtt_password;
  String oldMqttUser=mqtt_user;
  String oldHotspotSSid=hotspot_ssid;
  String oldHotspotPass=hotspot_pass;
  if (error)
  {
    Serial.println("!! Failed to parse JSON -- http data");
    
    server.send(418, "application/json", "{\"message\": \"bad!\"}");
    return;
  }
  else
  {
    wifi_ssid = jsonDoc["wifi_ssid"].as<String>();
    if(jsonDoc["wifi_pass"].as<String>()!=(String)NO_PASS_CHANGE) wifi_pass = jsonDoc["wifi_pass"].as<String>();
    else wifi_pass=oldWiFiPass;
    mqtt_server = jsonDoc["mqtt_server"].as<String>();
    mqtt_port = jsonDoc["mqtt_port"];
    mqtt_user = jsonDoc["mqtt_user"].as<String>();
    if(jsonDoc["mqtt_password"].as<String>()!=(String)NO_PASS_CHANGE) mqtt_password = jsonDoc["mqtt_password"].as<String>();
    else mqtt_password=oldMqttPass;
    mqtt_messageRoot = jsonDoc["mqtt_messageRoot"].as<String>();
    mdns_hostname = jsonDoc["mdns_hostname"].as<String>();
    hotspot_ssid = jsonDoc["hotspot_ssid"].as<String>();
    if(jsonDoc["hotspot_pass"].as<String>()!=(String)NO_PASS_CHANGE) hotspot_pass = jsonDoc["hotspot_pass"].as<String>();	
    else hotspot_pass=oldHotspotPass;
    refreshTime = jsonDoc["refreshTime"];


    //Sensor toggles
    for(int i=0; i<SENSOR_COUNT; i++)
    {//toggleIDName: i0-Sensor_Toggle i1-SensorName
      *toggles[i]=jsonDoc[toggleIDName[i][0]];
    }


    serializeJson(jsonDoc, Serial);

    Serial.println("-- done pasrsing JSON -- http data");

    saveConfig();
    jsonDoc.clear();
    loadConfig();
    readConfig();

    delay(200);
    if(wifiConnectionType)
    {
      if(wifi_ssid=="")
      {
        server.send(200, "application/json", "{\"message\": \"restarting device and turning on hotspot\"}");
        delay(200);
        rst();
      }

      if(oldSSID!=wifi_ssid || oldWiFiPass!=wifi_pass)
      {
        server.send(200, "application/json", "{\"message\": \"restarting sensors and connecting to new WiFi!\"}");
        delay(200);
        wifiStart();
        sensorsBegin();// reload sensors
      }
      else
      {
        server.send(200, "application/json", "{\"message\": \"restarting sensors\"}");
        delay(200);
        sensorsBegin();// reload sensors
      }
      
    }
    else 
    {
      server.send(200, "application/json", "{\"message\": \"restarting device\"}");
      delay(200);
      rst();
    }
  }
}
long  int timeout=0;
void wifiStart()
{
  A:
  timeout=millis()+10000;
  Serial.println();
  Serial.println(wifi_ssid.c_str());
  Serial.println(wifi_pass.c_str());

  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(mdns_hostname.c_str());
  WiFi.hostname(mdns_hostname);
  //WiFi.hostname("weatherstation");
  if(!WiFi.status()==WL_CONNECTED) WiFi.disconnect();
  WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
  Serial.println("-----WI-FI----");
  while (!WiFi.isConnected())
  {
    delay(500);
    Serial.print(".");
    if(millis()>timeout) goto A;
  }
  Serial.println("");
  if (MDNS.begin(mdns_hostname.c_str()))
  {
    // Start mDNS

    Serial.println("-- MDNS started");
  }
  Serial.print("Hostname: ");
  String da=WiFi.getHostname();
  Serial.println(da);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

  server.on("/", httpHome);
  server.on("/data", httpData);
  server.on("/restart", httpRestart);
  server.on("/titles", httpTitles);
  server.on("/sensors", httpSensors);
  server.on("/currentConfig", httpConfig);
  server.on("/wifi", httpWiFi);
  server.onNotFound(httpDefault);
  server.begin();

  mdns_service_add(mdns_hostname.c_str(), "_http", "_tcp", 80, NULL, 0);
  
  //MDNS.addService("http", "tcp", 80);
  ArduinoOTA.setMdnsEnabled(false);
  ArduinoOTA.begin();

  Serial << "ready" << endl;
}

void apStart()
{
  Serial.println("--apstart--");
  Serial.println();

    WiFi.persistent(false);
    WiFi.disconnect(true);
    WiFi.softAP(hotspot_ssid.c_str(), hotspot_pass.c_str());

    WiFi.begin();
    Serial.println("-----HOTSPOT-----");
    Serial.println("");
    Serial.print("SSID:");
    Serial.println(WiFi.softAPSSID());
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
  

  if (MDNS.begin(mdns_hostname.c_str()))
  {
    // Start mDNS

    Serial.println("-- MDNS started");
  }

  server.on("/", httpHome);
  server.on("/data", httpData);
  server.on("/restart", httpRestart);
  server.on("/titles", httpTitles);
  server.on("/sensors", httpSensors);
  server.on("/currentConfig", httpConfig);
  server.on("/wifi", httpWiFi);
  server.onNotFound(httpDefault);
  server.begin();

  dnsServer.start(53, "*", WiFi.softAPIP());

  Serial << "ready" << endl;
}
