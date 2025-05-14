void startProgram()
{
  
  pinMode(HOTSPOT_PIN, INPUT_PULLUP);
  pinMode(RESET_CONFIG_PIN, INPUT_PULLUP);
  if(!LittleFS.begin()){
    Serial.println("["+runningTime()+"] An Error has occurred while mounting LittleFS");
    return;
  }
  loadConfig(); 
  //Wire.begin(21,22,100000); //21,22 for esp32 devkitcv4
  Wire.begin(sda,scl,100000);
  // 1 wifi 0 hotspot
  if(digitalRead(HOTSPOT_PIN)==false || wifi_ssid=="") 
  {
    Serial.println("["+runningTime()+"] Connection using Hotspot");
    wifiConnectionType=false;
    apStart();
  }
  else
  { 
    Serial.println("["+runningTime()+"] Connection using Wi-Fi");
    wifiConnectionType=true;  
    wifiStart();
    mqttConnect();
    sensorsBegin();
  }

  //list dirs Serial.println("--XD--"); Dir dir = LittleFS.openDir(""); while (dir.next()) { Serial.println(dir.fileName()); File f = dir.openFile("r"); Serial.println(f.size()); } Serial.println("--XD--"); 
  ArduinoOTA.onEnd(rst);
}

void rst(){
  ESP.restart();
}

void sleepAndReset(){
  Serial.print("["+runningTime()+"] Sleep for "+(refreshTime-5)+"s And Reset!"); 
  ESP.deepSleep((refreshTime-5)*1e6);
  rst();
} //1s = 1e6

unsigned int avgTime=0;
unsigned int counter=0;
unsigned int counter2=0;
void loopedProgram()
{
  int timeStart;
  int timeTook;
  if(wifiConnectionType)
  {
    timeStart=millis();
    if (WiFi.status() != WL_CONNECTED) wifiStart();
    client.loop();
    
    ///taskManager.runLoop();
    sensorLoop();
    //dnsServer.processNextRequest();
    ArduinoOTA.handle();
    server.handleClient();
    timeTook=millis()-timeStart;
    timeStart= 100-timeTook;
    delay(timeStart>0 ? timeStart : 0);

    if(counter++==10)
    {
      if(PM1006K_toggle)pm1006kRead();
      //if(SPS30_toggle) sps30Read();
      counter=0;
    } 
  }
  else
  {
    timeStart=millis();

    dnsServer.processNextRequest();
    server.handleClient();

    timeTook=millis()-timeStart;
    timeStart= 10-timeTook;
    delay(timeStart>0 ? timeStart : 0);
  }
  avgTime+=(100-timeStart);
  if(counter2++==99)
  {
    delay(timeStart>0 ? timeStart : 0);

    infoString="["+runningTime()+"] " + "INFO:";
    infoString+="\n\t\tAVG last 100 loop time: "+(String)(avgTime/100)+ "ms";
    infoString+="\n\t\tmdns hostname: "+(String)(mdns_hostname)+ ".local";
    #if defined(ESP32)
      infoString+="\n\t\tFS use: "+(String)(LittleFS.usedBytes()/1024)+"KB/"+(String)(LittleFS.totalBytes()/1024)+"KB"; 
    #elif defined(ESP8266)
      FSInfo fs_info;
      LittleFS.info(fs_info);
      infoString+="\n\t\tFS use: "+(String)(fs_info.usedBytes/1024)+"KB/"+(String)(fs_info.totalBytes/1024)+"KB"; 
    #endif
    infoString+="\n\t\tRAM free: "+(String)(ESP.getFreeHeap()/1024)+"KB"; 
    infoString+="\n\t\tIP: "+ (wifiConnectionType ? WiFi.localIP() : WiFi.softAPIP()).toString();
    infoString+="\n\t\tConnection type: " + (String)(wifiConnectionType?"WiFi":"Hotspot");
    if(!wifiConnectionType)infoString+="\n\t\tHotspot password: " + hotspot_pass;
    infoString+="\n\t\tDefault config pin: " + (String)RESET_CONFIG_PIN;
    infoString+="\n\t\tHotspot pin: " + (String)HOTSPOT_PIN +"\n";
    Serial.print(infoString);
    counter2=0;
    avgTime=0;
  }
}

String runningTime()
{
  char buffer[10];
  unsigned int  seconds = millis() / 1000;
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



