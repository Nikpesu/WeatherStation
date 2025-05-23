void startProgram()
{
  //setting pins for hotspot i reset config after reboot
  pinMode(HOTSPOT_PIN, INPUT_PULLUP);
  pinMode(RESET_CONFIG_PIN, INPUT_PULLUP);
  #if defined(ESP8266) 
    pinMode(FAN_PIN, OUTPUT);
    digitalWrite(FAN_PIN, LOW);
  #endif
  
  //if UniqueDeviceID is MAC it must not contain ':' for mqtt to work
  UniqueDeviceID.replace(":", "");

  //delay for pulup pins
  delay(200);

  //opening littlefs and loading config 
  if(!LittleFS.begin()){
    Serial.println("["+runningTime()+"] An Error has occurred while mounting LittleFS");
    return;
  }
  loadConfig(); 

  //starting I2C interface
  Wire.begin(sda,scl,100000);


  // false=wifi, true=hotspot
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

  //setting what to do when OTA update finishes
  ArduinoOTA.onEnd(rst);
}

void rst(){
  //restart :D
  ESP.restart();
}

void sleepAndReset(){
  Serial.print("["+runningTime()+"] Sleep for "+(refreshTime-5)+"s and restart!"); 
  ESP.deepSleep((refreshTime-5)*1e6);
  rst();
} //1s = 1e6

unsigned int avgTime=0;
unsigned int loopCount=0;
unsigned int counter=10000; //some sensors read/refresh data
unsigned int counter2=10000; //INFO refresh time
void loopedProgram()
{
  int timeStart;
  int timeTook;
  if(wifiConnectionType)
  {
    timeStart=millis();


    //refreshing sensors, connecting to wifi if disconnected, looping http services and dns loop
    if (WiFi.status() != WL_CONNECTED) wifiStart();
    client.loop();
    sensorLoop();

    #if defined(ESP8266)
      MDNS.update();
    #endif
    
    //ota and server handling
    ArduinoOTA.handle();
    server.handleClient();

    //delay and delay calculation
    timeTook=millis()-timeStart;
    timeStart=LOOP_TIME-timeTook;
    delay(timeStart>0 ? timeStart : 0);

    //some sensors read/refresh data
    if(counter>=1000)
    {
      if(PM1006K_toggle) pm1006kRead();
      //if(SPS30_toggle) sps30Read();
      counter=0;
    } 
  }
  else
  {
    timeStart=millis();

    //handling requests and dns sending...
    dnsServer.processNextRequest();
    server.handleClient();

    //delay and delay calculation
    timeTook=millis()-timeStart;
    timeStart= 10-timeTook;
    delay(timeStart>0 ? timeStart : 0);
  }

  //update counters
  loopCount++;
  counter+=LOOP_TIME;
  counter2+=LOOP_TIME;
  avgTime+=(LOOP_TIME-timeStart);

  //every 10s print out INFO and reset counters...
  if(counter2>=10000)
  {
    //make string
    infoString="["+runningTime()+"] " + "INFO:";
    infoString+="\n\t\tAVG loop time: "+(String)(avgTime/loopCount)+ "ms";
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

    //send string
    Serial.print(infoString);

    //update counters
    counter2=0;
    avgTime=0;
    loopCount=0;
  }
}


//format [Dd HH:MM:SS]  D -day HH-Hour MM-minute SS-Secound
String runningTime()
{
  char buffer[10];
  unsigned int seconds = millis() / 1000;
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



