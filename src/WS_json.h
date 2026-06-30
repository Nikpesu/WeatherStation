// ───────────────────────── config encryption ─────────────────────────
// Sensitive fields are stored encrypted (AES-256-CBC) in /config.json instead
// of plain text. The key is derived from the config password via SHA-256. An
// empty password still encrypts, just with an empty-derived key. The password
// itself is never written to the file; it lives in NVS (ESP32) / a key file
// (ESP8266) so the device can decrypt automatically on boot.

bool isSensitiveField(const String &id)
{
  return id == "wifi_pass" || id == "mqtt_user" ||
         id == "mqtt_password" || id == "hotspot_pass";
}

void deriveKey(const String &pass, uint8_t key[32])
{
  SHA256 sha;
  sha.reset();
  sha.update((const uint8_t *)pass.c_str(), pass.length());
  sha.finalize(key, 32);
}

void fillRandom(uint8_t *buf, size_t len)
{
  for (size_t i = 0; i < len; i += 4)
  {
#if defined(ESP32)
    uint32_t r = esp_random();
#elif defined(ESP8266)
    uint32_t r = RANDOM_REG32;
#else
    uint32_t r = ((uint32_t)random() << 16) ^ (uint32_t)random();
#endif
    size_t n = (len - i < 4) ? (len - i) : 4;
    memcpy(buf + i, &r, n);
  }
}

char hexNibble(uint8_t v) { return v < 10 ? ('0' + v) : ('a' + v - 10); }
uint8_t hexVal(char c)
{
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return 0;
}
String toHex(const uint8_t *data, size_t len)
{
  String s;
  s.reserve(len * 2);
  for (size_t i = 0; i < len; i++)
  {
    s += hexNibble(data[i] >> 4);
    s += hexNibble(data[i] & 0x0F);
  }
  return s;
}
size_t fromHex(const String &hex, uint8_t *out, size_t maxLen)
{
  size_t n = hex.length() / 2;
  if (n > maxLen) n = maxLen;
  for (size_t i = 0; i < n; i++)
    out[i] = (hexVal(hex[2 * i]) << 4) | hexVal(hex[2 * i + 1]);
  return n;
}

// Encrypt plaintext -> "enc:" + hex(IV || ciphertext). AES-256-CTR is a stream
// mode, so the ciphertext is the same length as the plaintext (no padding).
String encryptString(const String &plain, const String &pass)
{
  uint8_t key[32];
  deriveKey(pass, key);
  uint8_t iv[16];
  fillRandom(iv, 16);

  size_t len = plain.length();
  uint8_t *ct = (uint8_t *)malloc(len ? len : 1);
  if (!ct) return plain;

  CTR<AES256> ctr;
  ctr.setKey(key, 32);
  ctr.setIV(iv, 16);
  if (len) ctr.encrypt(ct, (const uint8_t *)plain.c_str(), len);

  String out = "enc:";
  out += toHex(iv, 16);
  out += toHex(ct, len);

  free(ct);
  return out;
}

// Decrypt "enc:..." back to plaintext. A value that is not in the "enc:" format
// is returned untouched, so a plain-text config from before this feature loads.
String decryptString(const String &stored, const String &pass)
{
  if (!stored.startsWith("enc:")) return stored;

  String hex = stored.substring(4);
  size_t total = hex.length() / 2;
  if (total < 16) return ""; // need at least the IV

  uint8_t *raw = (uint8_t *)malloc(total);
  if (!raw) return "";
  fromHex(hex, raw, total);

  uint8_t iv[16];
  memcpy(iv, raw, 16);
  size_t ctLen = total - 16;

  uint8_t key[32];
  deriveKey(pass, key);
  CTR<AES256> ctr;
  ctr.setKey(key, 32);
  ctr.setIV(iv, 16);

  String out;
  out.reserve(ctLen);
  if (ctLen)
  {
    uint8_t *pt = (uint8_t *)malloc(ctLen);
    if (!pt) { free(raw); return ""; }
    ctr.decrypt(pt, raw + 16, ctLen);
    for (size_t i = 0; i < ctLen; i++) out += (char)pt[i];
    free(pt);
  }

  free(raw);
  return out;
}

// Persist / restore the config password to NVS (ESP32) or a key file (ESP8266)
// so it survives reboots and (on ESP32) a filesystem-erasing firmware flash.
void saveConfigPassword(const String &pass)
{
#if defined(ESP32)
  Preferences prefs;
  prefs.begin("ws_creds", false);
  prefs.putString("cfgpass", pass);
  prefs.end();
#elif defined(ESP8266)
  File f = LittleFS.open("/cfg.key", "w");
  if (f) { f.print(pass); f.close(); }
#endif
}
String loadConfigPassword()
{
#if defined(ESP32)
  Preferences prefs;
  prefs.begin("ws_creds", true);
  String v = prefs.getString("cfgpass", "");
  prefs.end();
  return v;
#elif defined(ESP8266)
  File f = LittleFS.open("/cfg.key", "r");
  if (!f) return "";
  String v = f.readString();
  f.close();
  return v;
#else
  return "";
#endif
}

// Apply encryption only to sensitive fields; everything else passes through.
String storeFieldValue(const String &id, const String &plain)
{
  return isSensitiveField(id) ? encryptString(plain, config_password) : plain;
}
String readFieldValue(const String &id, const String &stored)
{
  return isSensitiveField(id) ? decryptString(stored, config_password) : stored;
}

// ───────────────────────── web-UI authentication ─────────────────────────

// Pull the ws_session token out of the request's Cookie header.
String sessionCookie()
{
  String c = server.header("Cookie");
  int i = c.indexOf("ws_session=");
  if (i < 0) return "";
  i += 11; // strlen("ws_session=")
  int j = c.indexOf(';', i);
  return (j < 0) ? c.substring(i) : c.substring(i, j);
}

bool validToken(const String &t)
{
  if (t == "") return false;
  for (int i = 0; i < MAX_SESSIONS; i++)
    if (sessions[i] != "" && sessions[i] == t) return true;
  return false;
}

// A request is authorized when no password is set (open site) or it carries a
// valid session cookie.
bool isAuthed()
{
  if (config_password == "") return true;
  return validToken(sessionCookie());
}

// Hand out a new random session token (oldest is evicted when full).
String createSession()
{
  uint8_t r[16];
  fillRandom(r, 16);
  String t = toHex(r, 16);
  sessions[sessionWriteIdx] = t;
  sessionWriteIdx = (sessionWriteIdx + 1) % MAX_SESSIONS;
  return t;
}

// Invalidate the session token carried by the current request.
void clearCurrentSession()
{
  String t = sessionCookie();
  if (t == "") return;
  for (int i = 0; i < MAX_SESSIONS; i++)
    if (sessions[i] == t) sessions[i] = "";
}

// Persist / read the "first-run setup completed" flag.
void saveSetupDone(bool done)
{
#if defined(ESP32)
  Preferences prefs;
  prefs.begin("ws_creds", false);
  prefs.putBool("setupdone", done);
  prefs.end();
#elif defined(ESP8266)
  if (done) { File f = LittleFS.open("/setup.ok", "w"); if (f) { f.print("1"); f.close(); } }
  else      { LittleFS.remove("/setup.ok"); }
#endif
}
bool loadSetupDone()
{
#if defined(ESP32)
  Preferences prefs;
  prefs.begin("ws_creds", true);
  bool v = prefs.getBool("setupdone", false);
  prefs.end();
  return v;
#elif defined(ESP8266)
  return LittleFS.exists("/setup.ok");
#else
  return true;
#endif
}

void loadConfig()
{
  // The config password lives in NVS / key file, never in /config.json. Load it
  // first so sensitive fields can be decrypted (and a freshly written default
  // config gets encrypted with the right key).
  config_password = loadConfigPassword();
  setupDone = loadSetupDone();

  // Check if JSON exists, if not make a new one
  if (!LittleFS.exists(JSON_FILE_PATH) || !digitalRead(RESET_CONFIG_PIN))
  {
    Serial.println("["+runningTime()+"] new config making; loadConfig");

    // A hardware config reset also clears the password and setup flag, so a
    // forgotten password can never lock the user out of the device.
    if (!digitalRead(RESET_CONFIG_PIN))
    {
      config_password = "";
      saveConfigPassword("");
      setupDone = false;
      saveSetupDone(false);
      Serial.println("["+runningTime()+"] auth reset by config pin; loadConfig");
    }
    #if defined(ESP32)
    // Restore WiFi credentials from NVS before writing defaults so the
    // device reconnects after a firmware flash that erased LittleFS.
    Preferences prefs;
    prefs.begin("ws_creds", true);
    if (prefs.isKey("ssid")) {
      wifi_ssid = prefs.getString("ssid", wifi_ssid);
      wifi_pass = prefs.getString("pass", wifi_pass);
      Serial.println("["+runningTime()+"] WiFi credentials restored from NVS; loadConfig");
    }
    prefs.end();
    #endif
    saveConfig();
    Serial.println("["+runningTime()+"] new config made; loadConfig");
  }

  // Open the JSON file for reading
  File configFile = LittleFS.open(JSON_FILE_PATH, "r");
  Serial.println("["+runningTime()+"] config opened; loadConfig");

  if (configFile)
  {
    // Read the whole file (encrypted sensitive fields make it variable length)
    String content = configFile.readString();
    configFile.close();

    // Parse the JSON object
    JsonDocument json;
    DeserializationError error = deserializeJson(json, content);


    if (!error)
    {
      //Sensor toggles
      for(int i=0; i<SENSOR_COUNT; i++)
      {//toggleIDName: i0-Sensor_Toggle i1-SensorName
        *toggles[i]=json[toggleIDName[i][0]];
      }

      for(int i=0; i<FIELD_COUNT;i++)
      {
        String id = fieldsIDNameTypePlaceholder[i][0];
        if (id == "config_password") continue; // loaded from NVS, never in the file

        String value = readFieldValue(id, json[id].as<String>());
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

      for(int i=0; i<PINS_COUNT; i++)
      {
        // Keep the compile-time default for keys missing from an older config
        // file (e.g. LED_pin added after the file was first written).
        if(json[pinIDName[i][0]].isNull()) continue;
        // Accept either a GPIO number or a "Dx" board label.
        *pins[i]=pinFromString(json[pinIDName[i][0]].as<String>());
      }

      // LED feature settings (managed by the LED popup, not part of fields[]).
      // Load the toggles and the count first so the per-LED arrays below are
      // bounded correctly. All are absent in older files -> struct defaults kept.
      if(!json["led_toggle"].isNull()) config.led_toggle = json["led_toggle"].as<bool>();
      if(!json["ldr_brightness_toggle"].isNull()) config.ldr_brightness_toggle = json["ldr_brightness_toggle"].as<bool>();
      if(!json["led_count"].isNull())
      {
        int c = json["led_count"].as<int>();
        if(c < 1) c = 1;
        if(c > LED_MAX_COUNT) c = LED_MAX_COUNT;
        config.led_count = c;
      }
      // Per-LED sensor mapping (array of sensor indices).
      if(json["led_map"].is<JsonArray>())
      {
        JsonArray arr = json["led_map"].as<JsonArray>();
        for(int i=0; i<LED_MAX_COUNT && i<(int)arr.size(); i++)
          config.led_map[i]=arr[i].as<int>();
      }
      if(json["led_red"].is<JsonArray>())
        for(int i=0; i<LED_MAX_COUNT && i<(int)json["led_red"].as<JsonArray>().size(); i++)
          config.led_red[i]=json["led_red"][i].as<float>();
      if(json["led_yellow"].is<JsonArray>())
        for(int i=0; i<LED_MAX_COUNT && i<(int)json["led_yellow"].as<JsonArray>().size(); i++)
          config.led_yellow[i]=json["led_yellow"][i].as<float>();
      if(json["led_green"].is<JsonArray>())
        for(int i=0; i<LED_MAX_COUNT && i<(int)json["led_green"].as<JsonArray>().size(); i++)
          config.led_green[i]=json["led_green"][i].as<float>();
      if(json["led_col1"].is<JsonArray>())
        for(int i=0; i<LED_MAX_COUNT && i<(int)json["led_col1"].as<JsonArray>().size(); i++)
          config.led_col1[i]=json["led_col1"][i].as<uint32_t>();
      if(json["led_col2"].is<JsonArray>())
        for(int i=0; i<LED_MAX_COUNT && i<(int)json["led_col2"].as<JsonArray>().size(); i++)
          config.led_col2[i]=json["led_col2"][i].as<uint32_t>();
      if(json["led_col3"].is<JsonArray>())
        for(int i=0; i<LED_MAX_COUNT && i<(int)json["led_col3"].as<JsonArray>().size(); i++)
          config.led_col3[i]=json["led_col3"][i].as<uint32_t>();
      updateFieldsToNative();
      //serializeJson(json, Serial); Serial.println();
    }
  else
  {
    Serial.println("["+runningTime()+"] Failed to parse config file1; loadConfig");
  }

    Serial.println("["+runningTime()+"] Config loaded; loadConfig");
  }
  else
  {
    Serial.println("["+runningTime()+"] Failed to open config file2; loadConfig");
  }
  Serial.println("["+runningTime()+"] done; loadConfig");
}
// Serialize the current `config` (via the fields/toggles/pins arrays, which all
// reference members of the struct) into a JSON document.
void buildConfigJson(JsonDocument &json)
{
  //Sensor toggles
  for(int i=0; i<SENSOR_COUNT; i++)
  {//toggleIDName: [i][0]-Sensor_Toggle(sht31_toggle) [i][1]-SensorName(SHT31)
    json[toggleIDName[i][0]]= *toggles[i];
  }
  for(int i=0; i<FIELD_COUNT;i++)
  {
    String id = fieldsIDNameTypePlaceholder[i][0];
    if (id == "config_password") continue; // kept only in NVS, never in the file
    json[id]= storeFieldValue(id, *(fields[i]));
  }
  for(int i=0; i<PINS_COUNT;i++)
  {
    json[pinIDName[i][0]]= *(pins[i]);
  }
  // LED feature settings (managed entirely by the LED popup, not fields[]).
  json["led_toggle"] = config.led_toggle;
  json["ldr_brightness_toggle"] = config.ldr_brightness_toggle;
  json["led_count"] = config.led_count;
  // Per-LED sensor mapping
  JsonArray ledMap = json["led_map"].to<JsonArray>();
  for(int i=0; i<config.led_count; i++)
    ledMap.add(config.led_map[i]);
  // Per-LED colour scale: step value points + per-step packed-RGB colours.
  JsonArray ledRed = json["led_red"].to<JsonArray>();
  JsonArray ledYel = json["led_yellow"].to<JsonArray>();
  JsonArray ledGrn = json["led_green"].to<JsonArray>();
  JsonArray ledC1  = json["led_col1"].to<JsonArray>();
  JsonArray ledC2  = json["led_col2"].to<JsonArray>();
  JsonArray ledC3  = json["led_col3"].to<JsonArray>();
  for(int i=0; i<config.led_count; i++)
  {
    ledRed.add(config.led_red[i]);
    ledYel.add(config.led_yellow[i]);
    ledGrn.add(config.led_green[i]);
    ledC1.add(config.led_col1[i]);
    ledC2.add(config.led_col2[i]);
    ledC3.add(config.led_col3[i]);
  }
}

// Function to save the configuration to the JSON file
bool saveConfig()
{
  // Create a JSON object
  JsonDocument json;
  Serial.println("["+runningTime()+"] started saving to JSON; saveConfig");
  // Set the values of the variables in the JSON object
  // Serial.println(ESP.getFreeHeap());

  buildConfigJson(json);

  serializeJson(json, Serial); Serial.println();
  
  Serial.println("["+runningTime()+"] copied to JSON; saveConfig ");
  // Open the JSON file for writing
  File configFile = LittleFS.open(JSON_FILE_PATH, "w");

  Serial.println("["+runningTime()+"] opened config file; saveConfig");
  if (configFile)
  {
    // Serialize the JSON object into a buffer
    serializeJson(json, configFile);
    configFile.close();
    Serial.println("["+runningTime()+"] copied to JSON; saveConfig");
    // Persist the encryption password (NVS on ESP32, key file on ESP8266) so the
    // device can decrypt the sensitive fields again on the next boot.
    saveConfigPassword(config_password);
    #if defined(ESP32)
    // Mirror WiFi credentials to NVS so they survive future firmware flashes.
    Preferences prefs;
    prefs.begin("ws_creds", false);
    prefs.putString("ssid", wifi_ssid);
    prefs.putString("pass", wifi_pass);
    prefs.end();
    #endif
    return true;
  }
  else
  {
    Serial.println("["+runningTime()+"] Failed to open config file for writing; saveConfig");
    return false;
  }
}

void updateFieldsToString()
{
  mqtt_port_str = String(mqtt_port);
  lowPowerMode_toggle_str = String(lowPowerMode_toggle);
  refreshTime_str = String(refreshTime);
}

void updateFieldsToNative()
{
  mqtt_port=mqtt_port_str.toInt();
  lowPowerMode_toggle=(lowPowerMode_toggle_str=="true"? true : false);
  refreshTime = refreshTime_str.toInt();
}

// Export the current configuration as a JSON string (used for backup/download).
// Returns exactly what is persisted in /config.json so it round-trips cleanly
// through importConfig. Falls back to the in-memory config if the file is missing.
String exportConfig()
{
  File configFile = LittleFS.open(JSON_FILE_PATH, "r");
  if (configFile)
  {
    String out = configFile.readString();
    configFile.close();
    return out;
  }

  JsonDocument json;
  buildConfigJson(json);
  String out;
  serializeJson(json, out);
  return out;
}

// Import a configuration from a JSON string (e.g. an uploaded backup), apply it
// into the `config` struct, persist it and reload. The supplied `password` is
// the one the backup's sensitive fields were encrypted with; the device adopts
// it as its own config password so the imported secrets stay decryptable.
// Returns false on parse error.
bool importConfig(String jsonStr, String password)
{
  JsonDocument json;
  DeserializationError error = deserializeJson(json, jsonStr);
  if (error)
  {
    Serial.println("["+runningTime()+"] Failed to parse imported config; importConfig");
    return false;
  }

  // Decrypt the incoming sensitive fields with the password the file was
  // exported with, and keep using it from now on.
  config_password = password;

  //Sensor toggles
  for(int i=0; i<SENSOR_COUNT; i++)
  {//toggleIDName: i0-Sensor_Toggle i1-SensorName
    if(!json[toggleIDName[i][0]].isNull())
      *toggles[i]=json[toggleIDName[i][0]];
  }

  for(int i=0; i<PINS_COUNT; i++)
  {
    if(!json[pinIDName[i][0]].isNull())
      *pins[i]=pinFromString(json[pinIDName[i][0]].as<String>());
  }

  if(!json["led_toggle"].isNull()) config.led_toggle = json["led_toggle"].as<bool>();
  if(!json["ldr_brightness_toggle"].isNull()) config.ldr_brightness_toggle = json["ldr_brightness_toggle"].as<bool>();
  if(!json["led_count"].isNull())
  {
    int c = json["led_count"].as<int>();
    if(c < 1) c = 1;
    if(c > LED_MAX_COUNT) c = LED_MAX_COUNT;
    config.led_count = c;
  }
  if(json["led_map"].is<JsonArray>())
  {
    JsonArray arr = json["led_map"].as<JsonArray>();
    for(int i=0; i<LED_MAX_COUNT && i<(int)arr.size(); i++)
      config.led_map[i]=arr[i].as<int>();
  }
  if(json["led_red"].is<JsonArray>())
    for(int i=0; i<LED_MAX_COUNT && i<(int)json["led_red"].as<JsonArray>().size(); i++)
      config.led_red[i]=json["led_red"][i].as<float>();
  if(json["led_yellow"].is<JsonArray>())
    for(int i=0; i<LED_MAX_COUNT && i<(int)json["led_yellow"].as<JsonArray>().size(); i++)
      config.led_yellow[i]=json["led_yellow"][i].as<float>();
  if(json["led_green"].is<JsonArray>())
    for(int i=0; i<LED_MAX_COUNT && i<(int)json["led_green"].as<JsonArray>().size(); i++)
      config.led_green[i]=json["led_green"][i].as<float>();
  if(json["led_col1"].is<JsonArray>())
    for(int i=0; i<LED_MAX_COUNT && i<(int)json["led_col1"].as<JsonArray>().size(); i++)
      config.led_col1[i]=json["led_col1"][i].as<uint32_t>();
  if(json["led_col2"].is<JsonArray>())
    for(int i=0; i<LED_MAX_COUNT && i<(int)json["led_col2"].as<JsonArray>().size(); i++)
      config.led_col2[i]=json["led_col2"][i].as<uint32_t>();
  if(json["led_col3"].is<JsonArray>())
    for(int i=0; i<LED_MAX_COUNT && i<(int)json["led_col3"].as<JsonArray>().size(); i++)
      config.led_col3[i]=json["led_col3"][i].as<uint32_t>();

  for(int i=0; i<FIELD_COUNT; i++)
  {
    String id = fieldsIDNameTypePlaceholder[i][0];
    if (id == "config_password") continue; // master password is never imported
    if(json[id].isNull()) continue;

    // Imported sensitive fields are encrypted with this device's password.
    String value = readFieldValue(id, json[id].as<String>());
    String nochange = (String)NO_PASS_CHANGE;

    // Skip redacted passwords so an export with hidden passwords keeps the old ones.
    if (fieldsIDNameTypePlaceholder[i][2] == "password" && value == nochange)
      continue;

    *(fields[i]) = value;
  }
  updateFieldsToNative();

  Serial.println("["+runningTime()+"] imported config applied; importConfig");
  saveConfig();
  setupDone = true;
  saveSetupDone(true);
  json.clear();
  loadConfig();
  return true;
}