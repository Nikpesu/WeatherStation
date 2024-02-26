// 1 wifi 0 hotspot
bool wifiCon;
void startProgram()
{

  Serial.begin(115200);
  Serial.println("||||||||");
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  loadConfig(); 
  Wire.begin(4,5);
  pms.init();

  // 1 wifi 0 hotspot
  if(digitalRead(12)==true)
  {
    wifiCon=true;  
    wifiStart();
    mqttSetup();
    sensorsBegin();
  }
  else
  {
    wifiCon=false;
    apStart();
  }

  //list dirs Serial.println("--XD--"); Dir dir = LittleFS.openDir(""); while (dir.next()) { Serial.println(dir.fileName()); File f = dir.openFile("r"); Serial.println(f.size()); } Serial.println("--XD--"); 
  ArduinoOTA.onEnd(rst);
}

void rst(){ESP.reset();}
void sleepAndReset(){Serial.print("\nsleepAndReset"); ESP.deepSleep((refreshTime-5)*1e6); rst();} //1s = 1e6

int i=10;
void loopedProgram()
{
  if(wifiCon)
  {
    if(!WiFi.isConnected())
      wifiStart();
    client.loop();
    taskManager.runLoop();
    MDNS.update();
    dnsServer.processNextRequest();
    ArduinoOTA.handle();
    server.handleClient();
  }
  else
  {
    dnsServer.processNextRequest();
    server.handleClient();
  }
  delay(250);
}