
// 1 wifi 0 hotspot
bool wifiCon;
void startProgram()
{
  Serial.println("||||||||");
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  loadConfig(); 
  Wire.begin(33,35); //21,22 for esp32 devkitcv4
 
  // 1 wifi 0 hotspot
  if(digitalRead(12)==false) 
  {
    wifiCon=true;  
    wifiStart();
    sensorsBegin();
    mqttSetup();
  }
  else
  { 
    wifiCon=false;
    apStart();
  }

  //list dirs Serial.println("--XD--"); Dir dir = LittleFS.openDir(""); while (dir.next()) { Serial.println(dir.fileName()); File f = dir.openFile("r"); Serial.println(f.size()); } Serial.println("--XD--"); 
  ArduinoOTA.onEnd(rst);
}

void rst(){ESP.restart();}

int i=10;
void loopedProgram()
{
  if(wifiCon)
  {
    if(!WiFi.status()==WL_CONNECTED)
      wifiStart();
    client.loop();
    taskManager.runLoop();
    //MDNS.run();
    dnsServer.processNextRequest();
    ArduinoOTA.handle();
    server.handleClient();
  }
  else
  {
    dnsServer.processNextRequest();
    server.handleClient();
  }
  delay(500);
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.getHostname());
}