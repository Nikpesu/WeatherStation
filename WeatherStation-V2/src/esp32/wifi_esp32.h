
String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}
void httpTitles()
{
  String signal;
  if(WiFi.RSSI()>-70) signal="[▂▃▅▇]";
  else if(WiFi.RSSI()>-85) signal="[▂▃▅▁]";
  else if(WiFi.RSSI()>-100) signal="[▂▃▁▁]";
  else signal="[▂▁▁▁]";;

  server.send(200, "application/json", 
    "{"
    "\"title1\":\""+mdns_hostname+"\","
    "\"title2\":\"SSID: "+ WiFi.SSID()+" | IP: "+WiFi.localIP().toString()+"\","
    "\"title3\":\"Uptime:"+runningTime()+" | RSSI: "+WiFi.RSSI()+signal+"\""
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
    ispis+="<br>BMP280_toggle: "+ (String)BMP280_toggle;
    ispis+="<br>SHT31_toggle: "+ (String)SHT31_toggle;
    ispis+="<br>SGP30_toggle: "+ (String)SGP30_toggle;
    ispis+="<br>AHT2x_toggle: "+ (String)AHT2x_toggle;
    ispis+="<br>ENS160_toggle: "+ (String)ENS160_toggle;
    ispis+="<br>SCD4x_toggle: "+ (String)SCD4x_toggle; 
    ispis+="<br>PMSx003_toggle: "+ (String)PMSx003_toggle; 
    ispis+="<br>PM1006K_toggle: "+ (String)PM1006K_toggle; 
    ispis+="<br>SPS30_toggle: "+ (String)SPS30_toggle; 
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

  if(BMP280_toggle) htmlContent.replace("id=\"BMP280_toggle\"", "id=\"BMP280_toggle\" checked");
  if(SHT31_toggle) htmlContent.replace("id=\"SHT31_toggle\"", "id=\"SHT31_toggle\" checked");
  if(SGP30_toggle) htmlContent.replace("id=\"SGP30_toggle\"", "id=\"SGP30_toggle\" checked");
  if(AHT2x_toggle) htmlContent.replace("id=\"AHT2x_toggle\"", "id=\"AHT2x_toggle\" checked");
  if(ENS160_toggle) htmlContent.replace("id=\"ENS160_toggle\"", "id=\"ENS160_toggle\" checked");
  if(SCD4x_toggle) htmlContent.replace("id=\"SCD4x_toggle\"", "id=\"SCD4x_toggle\" checked");
  if(PMSx003_toggle) htmlContent.replace("id=\"PMSx003_toggle\"", "id=\"PMSx003_toggle\" checked");
  if(PM1006K_toggle) htmlContent.replace("id=\"PM1006K_toggle\"", "id=\"PM1006K_toggle\" checked");
  if(SPS30_toggle) htmlContent.replace("id=\"SPS30_toggle\"", "id=\"SPS30_toggle\" checked");
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
    BMP280_toggle = jsonDoc["BMP280_toggle"];
    SHT31_toggle = jsonDoc["SHT31_toggle"];
    SGP30_toggle = jsonDoc["SGP30_toggle"];
    mdns_hostname = jsonDoc["mdns_hostname"].as<String>();
    hotspot_ssid = jsonDoc["hotspot_ssid"].as<String>();
    if(jsonDoc["hotspot_pass"].as<String>()!=(String)NO_PASS_CHANGE) hotspot_pass = jsonDoc["hotspot_pass"].as<String>();	
    else hotspot_pass=oldHotspotPass;
    AHT2x_toggle = jsonDoc["AHT2x_toggle"];
    ENS160_toggle = jsonDoc["ENS160_toggle"]; 
    PMSx003_toggle = jsonDoc["PMSx003_toggle"];
    PM1006K_toggle = jsonDoc["PM1006K_toggle"];
    SCD4x_toggle = jsonDoc["SCD4x_toggle"];
    SPS30_toggle = jsonDoc["SPS30_toggle"];
    refreshTime = jsonDoc["refreshTime"];


    serializeJson(jsonDoc, Serial);

    Serial.println("-- done pasrsing JSON -- http data");

    saveConfig();
    jsonDoc.clear();
    loadConfig();
    readConfig();
    server.send(200, "application/json", "{\"message\": \"ok\"}");

    delay(200);
    sensorsBegin();// reload sensors
    if(oldSSID!=wifi_ssid || oldWiFiPass!=wifi_pass)wifiStart();
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
  server.on("/currentConfig", httpConfig);

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
    WiFi.softAP("esp-captive");

    WiFi.begin(hotspot_ssid == NULL ? "weatherstation_!" : hotspot_ssid.c_str(), hotspot_pass == NULL ? "" : hotspot_pass.c_str());
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
  server.on("/currentConfig", httpConfig);
  server.onNotFound(httpDefault);
  server.begin();

  dnsServer.start(53, "*", WiFi.softAPIP());

  Serial << "ready" << endl;
}
