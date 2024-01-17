struct PrintMac
{
  const uint8_t *mac;
};

Print &
operator<<(Print &p, const PrintMac &pmac)
{
  const uint8_t *mac = pmac.mac;
  for (int i = 0; i < 6; ++i)
  {
    if (i > 0)
    {
      p << ':';
    }
    p << _WIDTHZ(_HEX(mac[i]), 2);
  }
  return p;
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

void httpDefault()
{
  String ipda = mdns_hostname;
  ipda += ".local";
  Serial.println(ipda);
  server.sendHeader("Location", IpAddress2String(WiFi.localIP()), true);
  server.send(302, "text/plain", "");
  server.client().stop();
}

void httpHome()
{
  File file = LittleFS.open("/index.html", "r");
  server.streamFile(file, "text/html");
  file.close();
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
    Serial.println("-- started to parse JSON -- http data");
    wifi_ssid = (String)jsonDoc["wifi_ssid"];
    wifi_pass = (String)jsonDoc["wifi_pass"];
    mqtt_server = (String)jsonDoc["mqtt_server"];
    mqtt_port = jsonDoc["mqtt_port"];
    mqtt_user = (String)jsonDoc["mqtt_user"];
    mqtt_password = (String)jsonDoc["mqtt_password"];
    mqtt_messageRoot = (String)jsonDoc["mqtt_messageRoot"];
    BMP280_toggle = jsonDoc["BMP280_toggle"];
    SHT31_toggle = jsonDoc["SHT31_toggle"];
    SGP30_toggle = jsonDoc["SGP30_toggle"];
    mdns_hostname = (String)jsonDoc["mdns_hostname"];
    hotspot_ssid = (String)jsonDoc["hotspot_ssid"];
    hotspot_pass = (String)jsonDoc["hotspot_pass"];
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

void wifiStart()
{
  Serial.println();
  Serial.println();

  WiFi.persistent(false);
  WiFi.disconnect(true);

  WiFi.mode(WIFI_STA);
  WiFi.setHostname(mdns_hostname.c_str());
  // WiFi.hostname("weatherstation");
  WiFi.begin(wifi_ssid, wifi_pass);
  Serial.println("-----WI-FI----");
  while (!WiFi.isConnected())
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  if (MDNS.begin(mdns_hostname))
  {
    // Start mDNS

    Serial.println("-- MDNS started");
  }
  Serial.print("Hostname: ");
  Serial.println(WiFi.hostname().c_str());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

  server.on("/", httpHome);
  server.on("/data", httpData);

  server.onNotFound(httpDefault);
  server.begin();

  MDNS.addService("http", "tcp", 80);
  ArduinoOTA.begin(false);

  Serial << "ready" << endl;
}

void apStart()
{
  Serial.println();
  Serial.println();

  WiFi.persistent(false);
  WiFi.disconnect(true);

  WiFi.softAP("esp-captive");

  WiFi.begin(hotspot_ssid == NULL ? "weatherstation_!" : hotspot_ssid, hotspot_pass == NULL ? "" : hotspot_pass);
  Serial.println("-----HOTSPOT-----");
  Serial.println("");
  Serial.print("SSID:");
  Serial.println(WiFi.softAPSSID());
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  if (MDNS.begin(mdns_hostname))
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
