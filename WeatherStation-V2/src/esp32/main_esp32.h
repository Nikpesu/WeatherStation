
// 1 wifi 0 hotspot
bool wifiCon;
void startProgram()
{
  
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  loadConfig(); 
  //Wire.begin(21,22,100000); //21,22 for esp32 devkitcv4
  Wire.begin(sda,scl,100000);
  // 1 wifi 0 hotspot
  if(digitalRead(12)==false && wifi_ssid!="") 
  {
    Serial.println("Connection using Wi-Fi");
    wifiCon=true;  
    wifiStart();
    mqttConnect();
    sensorsBegin();
  }
  else
  { 
    Serial.println("Connection using Hotspot");
    wifiCon=false;
    apStart();
  }

  //list dirs Serial.println("--XD--"); Dir dir = LittleFS.openDir(""); while (dir.next()) { Serial.println(dir.fileName()); File f = dir.openFile("r"); Serial.println(f.size()); } Serial.println("--XD--"); 
  ArduinoOTA.onEnd(rst);
}

void rst(){ESP.restart();}
void sleepAndReset(){Serial.print("\nsleep And Reset"); ESP.deepSleep((refreshTime-5)*1e6); rst();} //1s = 1e6

int counter=0;
void loopedProgram()
{
  if(wifiCon)
  {
    int timestmp=millis();
    if(!WiFi.status()==WL_CONNECTED) wifiStart();
    client.loop();
    taskManager.runLoop();
    //dnsServer.processNextRequest();
    ArduinoOTA.handle();
    server.handleClient();
    timestmp= 100-millis()+timestmp;
    delay(timestmp>0 ? timestmp : 0);

    if(PM1006K_toggle and counter++==10)
    {
      pm1006kRead();
      sps30Read();
      counter=0;
    } 
  }
  else
  {
    dnsServer.processNextRequest();
    server.handleClient();
    delay(10);
  }
   // Serial.println(wifiCon);
}

String runningTime()
{
  char buffer[10];
  unsigned long seconds = millis() / 1000;
  unsigned int days = seconds / 86400;
  unsigned int hours = (seconds / 3600) % 24;
  unsigned int minutes = (seconds / 60) % 60;
  unsigned int secs = seconds % 60;

  String time = String(days) + "d ";

  sprintf(buffer, "%02d", hours);
  time += String(buffer) + ":";

  sprintf(buffer, "%02d", minutes);
  time += String(buffer) + ":";

  sprintf(buffer, "%02d", secs);
  time += String(buffer);

  return time;
}