void httpDefault()
{
  String ipda = mdns_hostname;
  ipda += ".local";
  Serial.println(ipda);
  server.sendHeader("Location", ipda, true);
  server.send(302, "text/plain", "");
  server.client().stop();
}

void httpHome()
{
  File file = LittleFS.open("/index.html", "r");
  server.streamFile(file, "text/html");
  file.close();
}
String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}
void httpConfig()
{
    String ispis=htmlStart;
    ispis+="<body> <h1>CONFIG:</h1>";
    ispis+="<br><br>Wifi_SSID: "+ wifi_ssid;
    ispis+="<br>mqtt_server: "+ mqtt_server+":"+mqtt_port;
    ispis+="<br>mqtt_user: "+ mqtt_user;
    ispis+="<br>mqtt_messageRoot: "+ mqtt_messageRoot;
    ispis+="<br>BMP280_toggle: "+ BMP280_toggle;
    ispis+="<br>SHT31_toggle: "+ SHT31_toggle;
    ispis+="<br>SGP30_toggle: "+ SGP30_toggle;
    ispis+="<br>AHT2x_toggle: "+ AHT2x_toggle;
    ispis+="<br>ENS160_toggle: "+ ENS160_toggle;
    ispis+="<br>mdns_hostname: "+ mdns_hostname;
    ispis+="<br>hotspot_ssid: "+ hotspot_ssid;
    ispis+="<br>hotspot_pass: "+ hotspot_pass;
    ispis=ispis+"<br><input id=\"subm\" type=\"button\" value=\"Back\" onclick=\"window.open(" + IpAddress2String(WiFi.localIP()) + ")\">";
    ispis+="</body></html>";
  server.send(200, "text/plain", ispis);
}
void httpData()
{
  DynamicJsonDocument jsonDoc(JSON_OBJECT_SIZE);
  DeserializationError error = deserializeJson(jsonDoc, server.arg(0));
  if (error)
  {
    Serial.println("!! Failed to parse JSON -- http data");
  }
  else
  {
    wifi_ssid = jsonDoc["wifi_ssid"].as<String>();
    wifi_pass = jsonDoc["wifi_pass"].as<String>();
    mqtt_server = jsonDoc["mqtt_server"].as<String>();
    mqtt_port = jsonDoc["mqtt_port"];
    mqtt_user = jsonDoc["mqtt_user"].as<String>();
    mqtt_password = jsonDoc["mqtt_password"].as<String>();
    mqtt_messageRoot = jsonDoc["mqtt_messageRoot"].as<String>();
    BMP280_toggle = jsonDoc["BMP280_toggle"];
    SHT31_toggle = jsonDoc["SHT31_toggle"];
    SGP30_toggle = jsonDoc["SGP30_toggle"];
    mdns_hostname = jsonDoc["mdns_hostname"].as<String>();
    hotspot_ssid = jsonDoc["hotspot_ssid"].as<String>();
    hotspot_pass = jsonDoc["hotspot_pass"].as<String>();	
    AHT2x_toggle = jsonDoc["AHT2x_toggle"];
    ENS160_toggle = jsonDoc["ENS160_toggle"]; 
    AHT2x_toggle = jsonDoc["AHT2x_toggle"];
    ENS160_toggle = jsonDoc["ENS160_toggle"];


    serializeJson(jsonDoc, Serial);

    Serial.println("-- done pasrsing JSON -- http data");
  }
  saveConfig();
  jsonDoc.clear();
  loadConfig();
  readConfig();
  server.send(200);
}
long  int daaah=0;
void wifiStart()
{
  A:
  daaah=millis()+10000;
  Serial.println();
  Serial.println(wifi_ssid.c_str());
  Serial.println(wifi_pass.c_str());

  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(mdns_hostname.c_str());
  WiFi.hostname(mdns_hostname);
  //WiFi.hostname("weatherstation");
  WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
  Serial.println("-----WI-FI----");
  while (!WiFi.isConnected())
  {
    delay(500);
    Serial.print(".");
    if(millis()>daaah) goto A;
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

  server.onNotFound(httpDefault);
  server.begin();

  MDNS.addService("http", "tcp", 80);
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
  server.on("/currentConfig", httpConfig);
  server.onNotFound(httpDefault);
  server.begin();

  dnsServer.start(53, "*", WiFi.softAPIP());

  Serial << "ready" << endl;
}
