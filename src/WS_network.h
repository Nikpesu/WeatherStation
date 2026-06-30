

// Reject the request with 401 when the site is password-protected and the
// caller has no valid session. Used at the top of every data/config endpoint.
#define REQUIRE_AUTH() do { if(!isAuthed()){ server.send(401, "application/json", "{\"message\": \"unauthorized\"}"); return; } } while(0)

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
  REQUIRE_AUTH();
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
void httpIDs()
{
  REQUIRE_AUTH();
  String sendingValue="[";
  for(int i=0; i<SENSOR_COUNT; i++)
  {
    sendingValue+="\""+toggleIDName[i][0]+"\""+",";
  }
  for(int i=0; i<FIELD_COUNT; i++)
  {
    sendingValue+="\""+fieldsIDNameTypePlaceholder[i][0]+"\""+",";
  }
  for(int i=0; i<PINS_COUNT; i++)
  {
    sendingValue+="\""+pinIDName[i][0]+"\""+(i<PINS_COUNT-1? ",":"");
  }
  sendingValue+="]";
  Serial.println(sendingValue);
  server.send(200, "application/json",sendingValue);

}
void httpWiFi()
{
  REQUIRE_AUTH();
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
  REQUIRE_AUTH();
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
// "#rrggbb" / "rrggbb" -> packed 0xRRGGBB.
uint32_t parseHexColor(const String &s)
{
  String t = s;
  t.trim();
  if (t.startsWith("#")) t = t.substring(1);
  return (uint32_t)strtoul(t.c_str(), nullptr, 16) & 0xFFFFFF;
}
// packed 0xRRGGBB -> "#rrggbb" (for <input type="color"> in the popup).
String colorToHex(uint32_t c)
{
  char buf[8];
  snprintf(buf, sizeof(buf), "#%06lX", (unsigned long)(c & 0xFFFFFF));
  return String(buf);
}

// Everything the LED popup needs in one payload: the two feature toggles, the
// active count + capacity, the sensor list (for the per-LED mapping dropdowns),
// and per-LED mapping + colour scale (values + per-step colours) + live reading.
void httpLedConfig()
{
  REQUIRE_AUTH();
  String s = "{";
  s += "\"led_toggle\":" + String(led_toggle ? "true" : "false") + ",";
  s += "\"ldr_toggle\":" + String(ldr_brightness_toggle ? "true" : "false") + ",";
  s += "\"count\":" + String(config.led_count) + ",";
  s += "\"maxCount\":" + String(LED_MAX_COUNT) + ",";
  s += "\"sensors\":[";
  for(int i=0; i<SENSOR_COUNT; i++)
  {
    s += "{\"idx\":" + String(i) + ",\"id\":\"" + toggleIDName[i][0] + "\",\"name\":\"" + toggleIDName[i][1] + "\",\"enabled\":" + String(*toggles[i] ? "true" : "false") + "}";
    s += (i<SENSOR_COUNT-1 ? "," : "");
  }
  s += "],\"measures\":[";
  for(int i=0; i<MEASURE_COUNT; i++)
  {
    s += "{\"idx\":" + String(i) + ",\"sensor\":" + String(ledMeasures[i].sensor) +
         ",\"label\":\"" + ledMeasures[i].label + "\",\"key\":\"" + ledMeasures[i].key + "\"}";
    s += (i<MEASURE_COUNT-1 ? "," : "");
  }
  s += "],\"leds\":[";
  for(int i=0; i<config.led_count; i++)
  {
    int m = config.led_map[i];
    bool mapped = (m >= 0 && m < MEASURE_COUNT);
    float v = mapped ? *ledMeasures[m].value : nan("");
    s += "{\"map\":" + String(config.led_map[i]) + ",";
    s += "\"red\":" + String(config.led_red[i], 2) + ",";
    s += "\"yellow\":" + String(config.led_yellow[i], 2) + ",";
    s += "\"green\":" + String(config.led_green[i], 2) + ",";
    s += "\"col1\":\"" + colorToHex(config.led_col1[i]) + "\",";
    s += "\"col2\":\"" + colorToHex(config.led_col2[i]) + "\",";
    s += "\"col3\":\"" + colorToHex(config.led_col3[i]) + "\",";
    s += "\"value\":" + (isnan(v) ? String("null") : String(v, 2)) + "}";
    s += (i<config.led_count-1 ? "," : "");
  }
  s += "]}";
  server.send(200, "application/json", s);
}
// Save all LED settings from the popup. Body:
//   { led_toggle, ldr_toggle, count, map:[], red:[], yellow:[], green:[],
//     col1:["#rrggbb"], col2:[...], col3:[...] }
// Persists and re-inits the strip live (ledsBegin) without restarting the device.
void httpLedConfigSet()
{
  REQUIRE_AUTH();
  String body = server.hasArg("plain") ? server.arg("plain") : server.arg(0);
  JsonDocument d;
  if(deserializeJson(d, body))
  {
    server.send(400, "application/json", "{\"message\": \"bad request\"}");
    return;
  }
  if(!d["led_toggle"].isNull()) led_toggle = d["led_toggle"].as<bool>();
  if(!d["ldr_toggle"].isNull()) ldr_brightness_toggle = d["ldr_toggle"].as<bool>();
  if(!d["count"].isNull())
  {
    int c = d["count"].as<int>();
    if(c < 1) c = 1;
    if(c > LED_MAX_COUNT) c = LED_MAX_COUNT;
    config.led_count = c;
  }
  if(d["map"].is<JsonArray>())
    for(int i=0; i<config.led_count && i<(int)d["map"].as<JsonArray>().size(); i++)
    {
      int mv = d["map"][i].as<int>();
      config.led_map[i] = (mv >= 0 && mv < MEASURE_COUNT) ? mv : -1;
    }
  if(d["red"].is<JsonArray>())
    for(int i=0; i<config.led_count && i<(int)d["red"].as<JsonArray>().size(); i++)
      config.led_red[i]=d["red"][i].as<float>();
  if(d["yellow"].is<JsonArray>())
    for(int i=0; i<config.led_count && i<(int)d["yellow"].as<JsonArray>().size(); i++)
      config.led_yellow[i]=d["yellow"][i].as<float>();
  if(d["green"].is<JsonArray>())
    for(int i=0; i<config.led_count && i<(int)d["green"].as<JsonArray>().size(); i++)
      config.led_green[i]=d["green"][i].as<float>();
  if(d["col1"].is<JsonArray>())
    for(int i=0; i<config.led_count && i<(int)d["col1"].as<JsonArray>().size(); i++)
      config.led_col1[i]=parseHexColor(d["col1"][i].as<String>());
  if(d["col2"].is<JsonArray>())
    for(int i=0; i<config.led_count && i<(int)d["col2"].as<JsonArray>().size(); i++)
      config.led_col2[i]=parseHexColor(d["col2"][i].as<String>());
  if(d["col3"].is<JsonArray>())
    for(int i=0; i<config.led_count && i<(int)d["col3"].as<JsonArray>().size(); i++)
      config.led_col3[i]=parseHexColor(d["col3"][i].as<String>());
  saveConfig();
  ledsBegin();  // apply toggle / pin / count changes live
  server.send(200, "application/json", "{\"message\": \"ok\"}");
}
void httpPins()
{
  REQUIRE_AUTH();
  String sendingValue="[";
  for(int i=0; i<PINS_COUNT; i++)
  {

    sendingValue+="{\"pinID\":\""+pinIDName[i][0]+"\",";
    sendingValue+="\"pinName\":\""+pinIDName[i][1]+"\",";
    sendingValue+="\"pinValue\":" + (String)*(pins[i]) +"}"+(i<PINS_COUNT-1? ",":"");
    

  }
  sendingValue+="]";
  server.send(200, "application/json",sendingValue);

}
void httpTitles()
{
  // Intentionally not auth-gated: the header titles and footer device-info are
  // shown on the login/setup screens too. Only low-sensitivity status (hostname,
  // IP, RSSI, uptime, version) is exposed here.
  String title3;

  if(wifiConnectionType) 
    title3=runningTime() + " | RSSI: " + WiFi.RSSI() + rssiToChart(WiFi.RSSI());
  else 
    title3=runningTime();

  server.send(200, "application/json", 
    "{"
    "\"footerText\":\"<p>Weatherstation_"+UniqueDeviceID+" at "+mdns_hostname+".local</p><p>Running on: "+device+", Version "+SWversion+"</p><br>\","
    "\"title1\":\""+mdns_hostname+"\","
    "\"title2\":\"SSID: "+ (wifiConnectionType ? WiFi.SSID() : WiFi.softAPSSID())+" | IP: "+(wifiConnectionType ? WiFi.localIP() : WiFi.softAPIP()).toString()+"\","
    "\"title3\":\"Uptime:"+title3+"\""
    "}");
}
void httpRestart()
{
  REQUIRE_AUTH();
  server.send(200, "application/json", "{\"restart\":\"success\"}");
  delay(200);
  rst();
}
// Themed read-only view of the live + default config. It is embedded as an
// <iframe> inside index.html; it reads the saved theme from localStorage (shared
// same-origin) so it always matches the parent page's theme.
void httpConfig()
{
  REQUIRE_AUTH();
  updateFieldsToString();

  String s = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">";
  s += "<meta name=\"viewport\" content=\"initial-scale=1.0, width=device-width\">";
  s += "<style>";
  s += ":root,[data-theme=\"dark\"]{--bg:#0d1117;--bg2:#161b22;--bg3:#1c2330;--border:#30363d;--text:#e6edf3;--text-dim:#7d8590;--accent:#58a6ff;--accent2:#3fb950;}";
  s += "[data-theme=\"light\"]{--bg:#f5f6fa;--bg2:#fff;--bg3:#eef0f4;--border:#d0d5dd;--text:#1a202c;--text-dim:#6b7280;--accent:#2563eb;--accent2:#16a34a;}";
  s += "[data-theme=\"terminal\"]{--bg:#0a0f00;--bg2:#0f1a00;--bg3:#142000;--border:#2a4a00;--text:#b4e64a;--text-dim:#5a8a20;--accent:#39ff14;--accent2:#7fff00;}";
  s += "[data-theme=\"amber\"]{--bg:#120900;--bg2:#1c1000;--bg3:#251600;--border:#4a2e00;--text:#ffd080;--text-dim:#8a6020;--accent:#ffaa00;--accent2:#ffdd44;}";
  s += "*{box-sizing:border-box;}";
  s += "body{margin:0;padding:1rem;background:var(--bg);color:var(--text);font-family:'Share Tech Mono',monospace,monospace;font-size:0.85rem;line-height:1.7;}";
  s += "h2{color:var(--accent);font-size:1rem;letter-spacing:0.08em;border-bottom:1px solid var(--border);padding-bottom:0.4rem;margin:1.4rem 0 0.6rem;}";
  s += ".k{color:var(--text-dim);} .v{color:var(--accent2);}";
  s += ".box{background:var(--bg2);border:1px solid var(--border);border-radius:10px;padding:0.9rem 1.1rem;margin-bottom:0.6rem;}";
  s += "hr{border:none;border-top:1px dashed var(--border);margin:0.6rem 0;}";
  s += "textarea{width:100%;background:var(--bg3);color:var(--text);border:1px solid var(--border);border-radius:8px;padding:0.6rem;font-family:inherit;font-size:0.8rem;}";
  s += "img{max-width:100%;border:1px solid var(--border);border-radius:8px;margin-top:0.6rem;}";
  s += "</style></head><body>";
  s += "<script>try{document.documentElement.setAttribute('data-theme',localStorage.getItem('ws-theme')||'dark');}catch(e){}</scr"
       "ipt>";

  s += "<h2>Current configuration</h2><div class=\"box\">";
  for(int i=0; i<FIELD_COUNT;i++)
  {
    s += "<span class=\"k\">" + fieldsIDNameTypePlaceholder[i][0] + ":</span> <span class=\"v\">";
    if(fieldsIDNameTypePlaceholder[i][2]=="password")
      s += "redacted";
    else
      s += ((String)*(fields[i]));
    s += "</span><br>";
  }
  s += "<hr>";
  for(int i=0; i<SENSOR_COUNT;i++)
    s += "<span class=\"k\">" + toggleIDName[i][0] + ":</span> <span class=\"v\">" + (String)( (bool)*(toggles[i]) ? "true" : "false" ) + "</span><br>";
  s += "<hr>";
  for(int i=0; i<PINS_COUNT;i++)
    s += "<span class=\"k\">" + pinIDName[i][0] + ":</span> <span class=\"v\">" + (String)( *(pins[i]) ) + "</span><br>";
  s += "</div>";

  s += "<h2>Log</h2><textarea rows=\"10\" readonly>"+infoString+"</textarea>";

  s += "<h2>Default config</h2><div class=\"box\">";
  s += "#define MDNS_HOSTNAME " + String(MDNS_HOSTNAME) + "<br>";
  s += "#define WIFI_SSID " + String(WIFI_SSID) + "<br>";
  s += "#define HOTSPOT_SSID " + String(HOTSPOT_SSID) + "<br>";
  s += "#define MQTT_SERVER " + String(MQTT_SERVER) + "<br>";
  s += "#define MQTT_PORT " + String(MQTT_PORT) + "<br>";
  s += "#define MQTT_MESSAGEROOT " + String(MQTT_MESSAGEROOT) + "<br>";
  s += "#define SUGGESTED_AREA " + String(SUGGESTED_AREA) + "<br>";
  s += "<hr>";
  s += "#define LOOP_TIME " + String(LOOP_TIME) + "<br>";
  s += "#define REFRESH_TIME " + String(REFRESH_TIME) + "<br>";
  s += "<hr>";
  s += "#define LOWPOWERMODE_TOGGLE " + String(LOWPOWERMODE_TOGGLE ? "true" : "false") + "<br>";
  s += "#define AHT2X_TOGGLE " + String(AHT2X_TOGGLE ? "true" : "false") + "<br>";
  s += "#define BMP280_TOGGLE " + String(BMP280_TOGGLE ? "true" : "false") + "<br>";
  s += "#define BMP580_TOGGLE " + String(BMP580_TOGGLE ? "true" : "false") + "<br>";
  s += "#define ENS160_TOGGLE " + String(ENS160_TOGGLE ? "true" : "false") + "<br>";
  s += "#define PM1006K_TOGGLE " + String(PM1006K_TOGGLE ? "true" : "false") + "<br>";
  s += "#define PMSX003_TOGGLE " + String(PMSX003_TOGGLE ? "true" : "false") + "<br>";
  s += "#define SCD4X_TOGGLE " + String(SCD4X_TOGGLE ? "true" : "false") + "<br>";
  s += "#define SGP30_TOGGLE " + String(SGP30_TOGGLE ? "true" : "false") + "<br>";
  s += "#define SHT31_TOGGLE " + String(SHT31_TOGGLE ? "true" : "false") + "<br>";
  s += "#define SPS30_TOGGLE " + String(SPS30_TOGGLE ? "true" : "false") + "<br>";
  s += "<hr>";
  s += "int sda=" + String(sda_pin) + ";<br>";
  s += "int scl=" + String(scl_pin) + ";<br>";
  s += "#define PM1006K_TX_PIN " + String(PM1006K_TX_PIN) + "<br>";
  s += "#define PM1006K_RX_PIN " + String(PM1006K_RX_PIN) + "<br>";
  s += "#define PMSX003_RX_PIN " + String(PMSX003_RX_PIN) + "<br>";
  s += "#define PMSX003_TX_PIN " + String(PMSX003_TX_PIN) + "<br>";
  s += "#define RESET_CONFIG_PIN " + String(RESET_CONFIG_PIN) + "<br>";
  s += "#define HOTSPOT_PIN " + String(HOTSPOT_PIN) + "<br>";
  s += "</div>";
  s += "<img src=\""+String(BOARDIMGLINK)+"\">";
  s += "</body></html>";

  server.send(200, "text/html", s);
}
void httpExportConfig()
{
  REQUIRE_AUTH();
  String json = exportConfig();
  server.sendHeader("Content-Disposition", "attachment; filename=\"" + mdns_hostname + "_config.json\"");
  server.send(200, "application/json", json);
}
void httpImportConfig()
{
  REQUIRE_AUTH();
  // Body: { "config": "<exported json string>", "password": "<may be blank>" }
  String body = server.hasArg("plain") ? server.arg("plain") : server.arg(0);
  JsonDocument req;
  if (deserializeJson(req, body))
  {
    server.send(400, "application/json", "{\"message\": \"bad request\"}");
    return;
  }
  String cfg = req["config"].as<String>();
  String pw = req["password"].isNull() ? "" : req["password"].as<String>();

  if (importConfig(cfg, pw))
  {
    server.send(200, "application/json", "{\"message\": \"config imported, restarting device\"}");
    Serial.println("["+runningTime()+"] config imported, restarting; httpImportConfig");
    delay(200);
    rst();
  }
  else
  {
    server.send(418, "application/json", "{\"message\": \"invalid config file\"}");
  }
}
// ───────────────────────── auth endpoints ─────────────────────────
void httpStatus()
{
  // Always reachable: tells the UI whether to show setup, login, or the app.
  String json = "{";
  json += "\"authRequired\":" + String(config_password != "" ? "true" : "false") + ",";
  json += "\"authenticated\":" + String(isAuthed() ? "true" : "false") + ",";
  json += "\"needsSetup\":" + String(!setupDone ? "true" : "false") + ",";
  json += "\"connectionType\":" + String(wifiConnectionType ? "\"wifi\"" : "\"hotspot\"");
  json += "}";
  server.send(200, "application/json", json);
}
void httpLogin()
{
  String body = server.hasArg("plain") ? server.arg("plain") : server.arg(0);
  JsonDocument d;
  if (deserializeJson(d, body))
  {
    server.send(400, "application/json", "{\"message\": \"bad request\"}");
    return;
  }
  String pass = d["password"].isNull() ? "" : d["password"].as<String>();
  if (pass == config_password)
  {
    String t = createSession();
    server.sendHeader("Set-Cookie", "ws_session=" + t + "; Path=/; Max-Age=86400; HttpOnly; SameSite=Lax");
    server.send(200, "application/json", "{\"message\": \"ok\"}");
  }
  else
  {
    server.send(401, "application/json", "{\"message\": \"wrong password\"}");
  }
}
void httpLogout()
{
  clearCurrentSession();
  server.sendHeader("Set-Cookie", "ws_session=; Path=/; Max-Age=0");
  server.send(200, "application/json", "{\"message\": \"ok\"}");
}
void httpSetup()
{
  // First-run only: choose the master/login password (may be blank) and mark the
  // device as set up. Available without auth because no password exists yet.
  if (setupDone)
  {
    server.send(403, "application/json", "{\"message\": \"already set up\"}");
    return;
  }
  String body = server.hasArg("plain") ? server.arg("plain") : server.arg(0);
  JsonDocument d;
  if (deserializeJson(d, body))
  {
    server.send(400, "application/json", "{\"message\": \"bad request\"}");
    return;
  }
  config_password = d["password"].isNull() ? "" : d["password"].as<String>();
  saveConfig(); // re-encrypt sensitive fields with the new key + persist it to NVS
  setupDone = true;
  saveSetupDone(true);
  String t = createSession();
  server.sendHeader("Set-Cookie", "ws_session=" + t + "; Path=/; Max-Age=86400; HttpOnly; SameSite=Lax");
  server.send(200, "application/json", "{\"message\": \"ok\"}");
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
  REQUIRE_AUTH();
  JsonDocument jsonDoc;
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
    for(int i=0; i<PINS_COUNT;i++)
    {
      // Accept either a GPIO number or a "Dx" board label.
      *pins[i]=pinFromString(jsonDoc[pinIDName[i][0]].as<String>());
    }
    // LED settings are handled entirely by the LED popup (/ledConfigSet), not here.
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
  // Make the Cookie request header available so session tokens can be read.
  // ESP8266's collectHeaders is variadic (header names directly); ESP32's takes
  // an array + count.
#if defined(ESP8266)
  server.collectHeaders("Cookie");
#else
  const char *headerKeys[] = {"Cookie"};
  server.collectHeaders(headerKeys, 1);
#endif

  server.on("/", httpHome);
  server.on("/data", httpData);
  server.on("/restart", httpRestart);
  server.on("/titles", httpTitles);
  server.on("/sensors", httpSensors);
  server.on("/pins", httpPins);
  server.on("/ledConfig", httpLedConfig);
  server.on("/ledConfigSet", httpLedConfigSet);
  server.on("/currentConfig", httpConfig);
  server.on("/exportConfig", httpExportConfig);
  server.on("/importConfig", httpImportConfig);
  server.on("/wifi", httpWiFi);
  server.on("/fields", httpfields);
  server.on("/IDs", httpIDs);
  server.on("/status", httpStatus);
  server.on("/login", httpLogin);
  server.on("/logout", httpLogout);
  server.on("/setup", httpSetup);
  server.onNotFound(httpDefault);
  server.begin();
}
// Scan for access points advertising the configured SSID and start connecting
// to the one with the strongest signal (mesh setup). Falls back to a plain
// connect if the SSID is not seen in the scan.
void connectToBestAP()
{
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);

  int n = WiFi.scanNetworks();
  int bestIdx = -1, bestRssi = -1000;
  for (int i = 0; i < n; i++)
  {
    if (WiFi.SSID(i) == wifi_ssid && WiFi.RSSI(i) > bestRssi)
    {
      bestRssi = WiFi.RSSI(i);
      bestIdx = i;
    }
  }

  if (bestIdx >= 0)
  {
    uint8_t bssid[6];
    memcpy(bssid, WiFi.BSSID(bestIdx), 6);
    int ch = WiFi.channel(bestIdx);
    WiFi.scanDelete();
    Serial.println("["+runningTime()+"] connecting to best AP, RSSI=" + String(bestRssi) + " dBm, ch=" + String(ch));
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str(), ch, bssid, true);
  }
  else
  {
    WiFi.scanDelete();
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
  }
}

// Periodic mesh check: if another AP with the same SSID is clearly stronger than
// the one we're on, jump to it. Called every ROAM_CHECK_INTERVAL from the loop.
void wifiRoamCheck()
{
  if (!WiFi.isConnected() || !WiFi.BSSID()) return;

  int currentRssi = WiFi.RSSI();
  uint8_t currentBssid[6];
  memcpy(currentBssid, WiFi.BSSID(), 6);

  int n = WiFi.scanNetworks();
  if (n <= 0) { WiFi.scanDelete(); return; }

  int bestIdx = -1, bestRssi = -1000;
  for (int i = 0; i < n; i++)
  {
    if (WiFi.SSID(i) == wifi_ssid && WiFi.RSSI(i) > bestRssi)
    {
      bestRssi = WiFi.RSSI(i);
      bestIdx = i;
    }
  }

  if (bestIdx >= 0)
  {
    uint8_t bestBssid[6];
    memcpy(bestBssid, WiFi.BSSID(bestIdx), 6);
    int ch = WiFi.channel(bestIdx);
    bool sameAP = (memcmp(bestBssid, currentBssid, 6) == 0);
    WiFi.scanDelete();

    if (!sameAP && bestRssi > currentRssi + ROAM_RSSI_THRESHOLD)
    {
      Serial.println("["+runningTime()+"] roaming to stronger AP: " + String(currentRssi) + " -> " + String(bestRssi) + " dBm");
      WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str(), ch, bestBssid, true);
      unsigned long until = millis() + 8000;
      while (!WiFi.isConnected() && millis() < until) delay(200);
      if (WiFi.isConnected())
        Serial.println("["+runningTime()+"] roamed, RSSI=" + String(WiFi.RSSI()) + " dBm");
    }
  }
  else
  {
    WiFi.scanDelete();
  }
}

long  int timeout=0;
void wifiStart()
{
  //wifi start ...
  do{
    timeout=millis()+10000;
    WiFi.persistent(false);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    if(!WiFi.status()==WL_CONNECTED) WiFi.disconnect();
    connectToBestAP();
    Serial.print("["+runningTime()+"] WI-FI starting");
    while (!WiFi.isConnected() and millis()<=timeout)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println(".");
  }while(WiFi.status() != WL_CONNECTED);
  Serial.println("\n["+runningTime()+"] WI-FI started!");


  //dns
  if (!MDNS.begin(mdns_hostname.c_str()))
    Serial.println("["+runningTime()+"] MDNS started!");
  ArduinoOTA.setHostname(mdns_hostname.c_str());
  WiFi.setHostname(mdns_hostname.c_str());
  MDNS.addService("_http", "_tcp", 80);

  /*
  #if defined(ESP32)
      //mdns_service_add(mdns_hostname.c_str(),"_http","_tcp", 80, NULL,0);
      Serial.println("["+runningTime()+"] MDNS added port 80!");
      
  #elif defined(ESP8266) 
    else 
      Serial.println("["+runningTime()+"] MDNS FAILED!");
    if(!MDNS.addService("_http", "_tcp", 80))
      Serial.println("["+runningTime()+"] MDNS http service on port 80 started!");
    else
      Serial.println("["+runningTime()+"] MDNS http service on port 80 failed!");
  #endif*/

  //start server services (HTTP)
  httpServicesStart();

  //ota service start
  ArduinoOTA.begin();

  //info
  Serial.println("["+runningTime()+"] WI-FI info:");
  Serial.println("\t\tHostname: " + (String)WiFi.getHostname()+".local");
  Serial.println("\t\tIP: "+ WiFi.localIP().toString());
  Serial.println("\t\tRRSI: "+ (String)WiFi.RSSI());

}

void apStart()
{
  //hotspot start ...
  Serial.println("["+runningTime()+"] AP starting");
  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.softAP(hotspot_ssid.c_str(), hotspot_pass.c_str());
  WiFi.begin();


  //dns
  if (MDNS.begin(mdns_hostname.c_str()))
  {
    if(MDNS.addService("http", "tcp", 80))
      Serial.println("["+runningTime()+"] MDNS http service on port 80 started!");
    else
      Serial.println("["+runningTime()+"] MDNS http service on port 80 failed!");
  }
  else 
    Serial.println("["+runningTime()+"] MDNS start FAILED!");

    
  //info
  Serial.println("["+runningTime()+"] HOTSPOT info:");
  Serial.println("\t\t SSID: "+ (String)WiFi.softAPSSID());
  Serial.println("\t\t IP: "+ (String)WiFi.softAPIP().toString());


  //start server services (HTTP)
  httpServicesStart();


  //DNS server start
  dnsServer.start(53, "*", WiFi.softAPIP());

}
