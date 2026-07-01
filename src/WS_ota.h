#ifndef WS_OTA_H
#define WS_OTA_H

// ───────────────────────── firmware update / OTA ─────────────────────────
// Two ways to update the device without a USB cable, both from the web UI:
//
//   1. Manual upload  — POST a firmware.bin (and/or a littlefs.bin filesystem
//      image) straight to the device (/updateFirmware, /updateFilesystem).
//   2. GitHub OTA     — the device pulls the latest release from
//      https://github.com/Nikpesu/WeatherStation/releases, compares the tag to
//      its running SWversion and, if newer, downloads + flashes the asset that
//      matches this board (FW_BOARD).
//
// Settings are preserved across an update: a firmware-only flash never touches
// LittleFS, so /config.json (and the NVS-stored password/WiFi creds) survive
// untouched. A *filesystem* flash would wipe /config.json, so we mirror it into
// NVS first (backupConfigToNVS) and loadConfig() restores it on the next boot.

#if defined(ESP32)
  #include <Update.h>
  #include <HTTPClient.h>
  #include <WiFiClientSecure.h>
  #include <HTTPUpdate.h>
#elif defined(ESP8266)
  #include <ESP8266HTTPClient.h>
  #include <WiFiClientSecureBearSSL.h>
  #include <ESP8266httpUpdate.h>
#endif

// Where OTA looks for releases. Assets must be named "<FW_BOARD>-firmware.bin"
// and "<FW_BOARD>-littlefs.bin" (produced by the firmware maker scripts / CI).
#define GITHUB_RELEASES_API "https://api.github.com/repos/Nikpesu/WeatherStation/releases/latest"

// Compare dotted numeric versions ("3.4.0", optional leading 'v'). Returns >0 if
// a is newer than b, 0 if equal, <0 if older. Missing trailing parts read as 0.
int versionCompare(String a, String b)
{
  a.trim(); b.trim();
  if (a.startsWith("v") || a.startsWith("V")) a = a.substring(1);
  if (b.startsWith("v") || b.startsWith("V")) b = b.substring(1);
  for (int i = 0; i < 4; i++)
  {
    int ai = a.toInt(), bi = b.toInt();
    if (ai != bi) return ai - bi;
    int da = a.indexOf('.'), db = b.indexOf('.');
    if (da < 0 && db < 0) break;
    a = (da < 0) ? String("0") : a.substring(da + 1);
    b = (db < 0) ? String("0") : b.substring(db + 1);
  }
  return 0;
}

// Fetch the latest GitHub release. On success fills `tag` and the download URLs
// for this board's firmware/filesystem assets (either may stay empty if the
// release doesn't carry that asset). Returns false + a reason in `err` on failure.
bool otaFetchLatest(String &tag, String &fwUrl, String &fsUrl, String &err)
{
  tag = ""; fwUrl = ""; fsUrl = ""; err = "";
  if (WiFi.status() != WL_CONNECTED) { err = "no wifi"; return false; }

#if defined(ESP32)
  WiFiClientSecure client;
  client.setInsecure();                       // GitHub cert isn't pinned on-device
#elif defined(ESP8266)
  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  client.setBufferSizes(4096, 512);
#endif

  HTTPClient https;
  if (!https.begin(client, GITHUB_RELEASES_API)) { err = "connect failed"; return false; }
  https.addHeader("User-Agent", "WeatherStation-ESP");   // GitHub API requires a UA
  https.addHeader("Accept", "application/vnd.github+json");
  https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  int code = https.GET();
  if (code != 200)
  {
    err = "http " + String(code);
    https.end();
    return false;
  }

  // Filter the (large) release JSON down to just the fields we need, so the whole
  // response never has to fit in RAM at once.
  JsonDocument filter;
  filter["tag_name"] = true;
  filter["assets"][0]["name"] = true;
  filter["assets"][0]["browser_download_url"] = true;

  JsonDocument rel;
  DeserializationError jerr = deserializeJson(rel, https.getStream(),
                                              DeserializationOption::Filter(filter));
  https.end();
  if (jerr) { err = String("json ") + jerr.c_str(); return false; }

  tag = rel["tag_name"].as<String>();
  String fwName = String(FW_BOARD) + "-firmware.bin";
  String fsName = String(FW_BOARD) + "-littlefs.bin";
  for (JsonObject a : rel["assets"].as<JsonArray>())
  {
    String name = a["name"].as<String>();
    String url  = a["browser_download_url"].as<String>();
    if (name == fwName) fwUrl = url;
    else if (name == fsName) fsUrl = url;
  }
  return true;
}

// Download `url` and flash it. `filesystem` selects the FS partition instead of
// the app partition. Does NOT reboot (the caller reboots once, after all parts).
bool otaApplyUrl(const String &url, bool filesystem, String &err)
{
  err = "";
#if defined(ESP32)
  WiFiClientSecure client;
  client.setInsecure();
  httpUpdate.rebootOnUpdate(false);
  httpUpdate.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);  // GitHub asset → S3 redirect
  t_httpUpdate_return r = filesystem ? httpUpdate.updateSpiffs(client, url)
                                     : httpUpdate.update(client, url);
  if (r == HTTP_UPDATE_OK) return true;
  err = httpUpdate.getLastErrorString();
  return false;
#elif defined(ESP8266)
  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  client.setBufferSizes(4096, 512);
  ESPhttpUpdate.rebootOnUpdate(false);
  ESPhttpUpdate.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);  // GitHub asset → S3 redirect
  t_httpUpdate_return r = filesystem ? ESPhttpUpdate.updateFS(client, url)
                                     : ESPhttpUpdate.update(client, url);
  if (r == HTTP_UPDATE_OK) return true;
  err = ESPhttpUpdate.getLastErrorString();
  return false;
#else
  err = "unsupported";
  return false;
#endif
}

// GET /otaCheck — report the running version and the latest GitHub release.
void httpOtaCheck()
{
  if (!isAuthed()) { server.send(401, "application/json", "{\"message\":\"unauthorized\"}"); return; }
  String tag, fwUrl, fsUrl, err;
  if (!otaFetchLatest(tag, fwUrl, fsUrl, err))
  {
    server.send(502, "application/json", "{\"error\":\"" + err + "\"}");
    return;
  }
  bool avail = versionCompare(tag, SWversion) > 0;
  String s = "{";
  s += "\"current\":\"" + SWversion + "\",";
  s += "\"latest\":\"" + tag + "\",";
  s += "\"board\":\"" + String(FW_BOARD) + "\",";
  s += "\"hasFirmware\":" + String(fwUrl != "" ? "true" : "false") + ",";
  s += "\"hasFilesystem\":" + String(fsUrl != "" ? "true" : "false") + ",";
  s += "\"updateAvailable\":" + String(avail ? "true" : "false");
  s += "}";
  server.send(200, "application/json", s);
}

// POST /otaUpdate  body: { "filesystem": <bool> } — pull the latest release and
// flash the firmware (always) and, if requested and available, the filesystem
// image. Reboots on success.
void httpOtaUpdate()
{
  if (!isAuthed()) { server.send(401, "application/json", "{\"message\":\"unauthorized\"}"); return; }
  String body = server.hasArg("plain") ? server.arg("plain") : (server.args() ? server.arg(0) : String("{}"));
  JsonDocument d;
  deserializeJson(d, body);
  bool doFs = d["filesystem"].as<bool>();

  String tag, fwUrl, fsUrl, err;
  if (!otaFetchLatest(tag, fwUrl, fsUrl, err))
  {
    server.send(502, "application/json", "{\"error\":\"" + err + "\"}");
    return;
  }
  if (fwUrl == "" && !(doFs && fsUrl != ""))
  {
    server.send(404, "application/json", "{\"error\":\"no matching asset for " + String(FW_BOARD) + "\"}");
    return;
  }

  Serial.println("[" + runningTime() + "] OTA: updating to " + tag + "; httpOtaUpdate");
  // Filesystem flash wipes LittleFS, so stash the config in NVS first (ESP32).
  if (doFs && fsUrl != "") backupConfigToNVS();

  if (fwUrl != "" && !otaApplyUrl(fwUrl, false, err))
  {
    server.send(500, "application/json", "{\"error\":\"firmware: " + err + "\"}");
    return;
  }
  if (doFs && fsUrl != "" && !otaApplyUrl(fsUrl, true, err))
  {
    server.send(500, "application/json", "{\"error\":\"filesystem: " + err + "\"}");
    return;
  }

  server.send(200, "application/json", "{\"message\":\"updated to " + tag + ", restarting\"}");
  Serial.println("[" + runningTime() + "] OTA done, restarting; httpOtaUpdate");
  delay(400);
  rst();
}

// ───────────────────────── manual .bin web upload ─────────────────────────
// State shared between the streaming upload callback and the completion handler.
bool otaUploadOk = false;

// Core of the multipart upload: `filesystem` picks the FS partition. Runs during
// the request body stream (START → WRITE… → END).
void otaHandleUpload(bool filesystem)
{
  HTTPUpload &up = server.upload();
  if (up.status == UPLOAD_FILE_START)
  {
    otaUploadOk = false;
    if (!isAuthed()) return;   // completion handler will 401
    Serial.println("[" + runningTime() + "] OTA upload start (" +
                   String(filesystem ? "filesystem" : "firmware") + "): " + up.filename);
    // Browser sends the raw file size as ?size= so ESP8266 can size the image.
    size_t declared = server.hasArg("size") ? (size_t)server.arg("size").toInt() : 0;
    if (filesystem) backupConfigToNVS();   // preserve settings across an FS flash
#if defined(ESP32)
    int cmd = filesystem ? U_SPIFFS : U_FLASH;
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) { Update.printError(Serial); return; }
#elif defined(ESP8266)
    Update.runAsync(true);
    if (filesystem)
    {
      FSInfo fs_info; LittleFS.info(fs_info);
      size_t fsSize = declared ? declared : fs_info.totalBytes;
      LittleFS.end();
      if (!Update.begin(fsSize, U_FS)) { Update.printError(Serial); return; }
    }
    else
    {
      uint32_t maxSketch = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      size_t sz = declared ? declared : maxSketch;
      if (!Update.begin(sz, U_FLASH)) { Update.printError(Serial); return; }
    }
#endif
    otaUploadOk = true;
  }
  else if (up.status == UPLOAD_FILE_WRITE)
  {
    if (otaUploadOk && Update.write(up.buf, up.currentSize) != up.currentSize)
    { Update.printError(Serial); otaUploadOk = false; }
  }
  else if (up.status == UPLOAD_FILE_END)
  {
    if (otaUploadOk)
    {
      if (Update.end(true)) Serial.println("[" + runningTime() + "] OTA upload success: " + String(up.totalSize) + " bytes");
      else { Update.printError(Serial); otaUploadOk = false; }
    }
  }
  else if (up.status == UPLOAD_FILE_ABORTED)
  {
    Update.end();
    otaUploadOk = false;
    Serial.println("[" + runningTime() + "] OTA upload aborted");
  }
}

// Completion handler for both /updateFirmware and /updateFilesystem: report the
// result, then reboot into the freshly-flashed image.
void httpUpdateDone()
{
  server.sendHeader("Connection", "close");
  if (!isAuthed()) { server.send(401, "application/json", "{\"ok\":false,\"message\":\"unauthorized\"}"); return; }
  if (!otaUploadOk || Update.hasError())
  {
    server.send(500, "application/json", "{\"ok\":false,\"message\":\"update failed\"}");
    return;
  }
  server.send(200, "application/json", "{\"ok\":true,\"message\":\"updated, restarting\"}");
  delay(400);
  rst();
}

#endif
