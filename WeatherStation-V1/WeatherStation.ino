#include <FS.h>                   //Must be first!!!! Otherwise esp burns
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>         //https://github.com/knolleary/pubsubclient
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>      //https://github.com/adafruit/Adafruit_BMP280_Library
#include "Adafruit_SGP30.h"       //https://github.com/adafruit/Adafruit_SGP30
#include "Adafruit_SHT31.h"       //https://github.com/adafruit/Adafruit_SHT31

  Adafruit_BMP280 BMP280;
  Adafruit_SHT31 SHT31 = Adafruit_SHT31();
  Adafruit_SGP30 SGP30;

  WiFiClient espClient;
  PubSubClient client(espClient);
  IPAddress IP;//MQTT server IP

  #define wsID "Weatherstation_1"
  char mqtt_ID[40];

  //Set parameters here if you want
  char mqtt_server[40]="";
  char mqtt_port[10] = "";
  char mqtt_user[40]="";
  char mqtt_password[40] = "";
  char sleep_time[10] = "";
  
  const bool BMP280_toggle=true;
  const bool SHT31_toggle=true;
  const bool SGP30_toggle=true;
  int datalength=35;
  
  #define MSG_BUFFER_SIZE  (50)
  char msg[MSG_BUFFER_SIZE];

/*
--------------------------------------
              Setup
--------------------------------------
*/
void setup() 
  {
    Serial.begin(115200);
    pinMode(12, INPUT_PULLUP);
    if (!SPIFFS.begin()) 
    {
      Serial.println("failed to mount FS");
    } 
      else 
    {
      Serial.println("mounted FS");
      if (SPIFFS.exists("/config.json")) 
      {
        Serial.println("reading config file");
        File configFile = SPIFFS.open("/config.json", "r");
        if (configFile) 
        {
          Serial.println("opened config file");
          size_t size = configFile.size();
          std::unique_ptr<char[]> buf(new char[size]); //buffer for JSON
          configFile.readBytes(buf.get(), size);
          DynamicJsonDocument json(1024);
          auto deserializeError = deserializeJson(json, buf.get());
          serializeJson(json, Serial);
          if ( ! deserializeError ) 
          {
            Serial.println("\nparsed json");
            strcpy(mqtt_server, json["mqtt_server"]);
            strcpy(mqtt_port, json["mqtt_port"]);
            strcpy(mqtt_user, json["mqtt_user"]);
            strcpy(mqtt_password, json["mqtt_password"]);
            strcpy(mqtt_ID, json["mqtt_ID"]);
            strcpy(sleep_time, json["sleep_time"]);
            configFile.close();
          } 
            else 
          { 
            Serial.println("failed to load json config");
          }
        }
      }
      else 
      {
        ((String)wsID).toCharArray(mqtt_ID, 40);
        
        Serial.println("No /config.json -- making it");
        
        DynamicJsonDocument json(1024);
        
        json["mqtt_server"] = mqtt_server;
        json["mqtt_port"] = mqtt_port;
        json["mqtt_user"] = mqtt_user;
        json["mqtt_password"] = mqtt_password;
        json["mqtt_ID"] = mqtt_ID;
        json["sleep_time"] = sleep_time;
        
        File configFile = SPIFFS.open("/config.json", "w");
        
        if (!configFile) {
          Serial.println("failed to open config file for writing");
        }
        
        serializeJson(json, Serial);
        serializeJson(json, configFile);
        configFile.close();
        
        Serial.println("reboot...");
        ESP.reset();
      }
    }
    
    if(digitalRead(12)==LOW)
    {
      Serial.println("-------------------\n\nButton pushed, turning on AP!!\n\n-------------------");
      apStart();
      writeToJson();
      ESP.reset();
    }
    
    setup_wifi();
    
    client.setServer(mqtt_server, (int)mqtt_port);
    client.setCallback(callback);
    
    reconnect();
  
    Wire.begin(4,5);
    if(SHT31_toggle) SHT31setup();
    if(BMP280_toggle) BMP280setup();
    if(SGP30_toggle) SGP30setup();
  }

/*
--------------------------------------
              Things...
--------------------------------------
*/
ICACHE_RAM_ATTR void handlerMethod() {}
void callback(char* topic, byte* payload, unsigned int length) {}

/*
--------------------------------------
         Connecting to wifi
--------------------------------------
*/
void setup_wifi() 
  {
    WiFiManager wifiManager;
    delay(10);
  
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wifiManager.getSSID());
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiManager.getSSID(), wifiManager.getPassword());
  
    int tries=0;
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      tries++;
      if(tries == 40) 
      {
        //apStart();
        //writeToJson();
        Serial.println("reboot...");
        ESP.reset();
      }
    }
  
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
/*
--------------------------------------
     Making AP for configurating
--------------------------------------
*/
void apStart()
  {
    WiFiManager wifiManager;
    
    WiFiManagerParameter custom_mqtt_ID("ID", "mqtt ID", mqtt_ID, 40);
    WiFiManagerParameter custom_sleep_time("sleep", "sleep time", sleep_time, 40);
    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 10);
    WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 40);
    WiFiManagerParameter custom_mqtt_password("password", "mqtt password", mqtt_password, 40);
    
    wifiManager.addParameter(&custom_mqtt_ID);
    wifiManager.addParameter(&custom_sleep_time);
    wifiManager.addParameter(&custom_mqtt_user);
    wifiManager.addParameter(&custom_mqtt_password);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_mqtt_server);
  
    String APname=(String)mqtt_ID;
    char APnamechar[100];
    APname.toCharArray(APnamechar, 100);
    wifiManager.startConfigPortal(APnamechar, mqtt_password);
  
    ((String)custom_mqtt_server.getValue()).toCharArray(mqtt_server,40);
    ((String)custom_mqtt_port.getValue()).toCharArray(mqtt_port,10);
    ((String)custom_mqtt_ID.getValue()).toCharArray(mqtt_ID,40);
    ((String)custom_sleep_time.getValue()).toCharArray(sleep_time,10);
    ((String)custom_mqtt_user.getValue()).toCharArray(mqtt_user,40);
    ((String)custom_mqtt_password.getValue()).toCharArray(mqtt_password,40);
  }

/*
--------------------------------------
         Writing conf to json
--------------------------------------
*/
void writeToJson()
  {
    DynamicJsonDocument json(1024);
    
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["mqtt_user"] = mqtt_user;
    json["mqtt_password"] = mqtt_password;
    json["mqtt_ID"] = mqtt_ID;
    json["sleep_time"] = sleep_time;
    
    File configFile = SPIFFS.open("/config.json", "w");
  
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }
  
    serializeJson(json, Serial);
    serializeJson(json, configFile);
    configFile.close();
  }

/*
--------------------------------------
      Connecting to MQTT server
--------------------------------------
*/
void reconnect() 
  {
    // Loop until we're reconnected
   
    IP.fromString(mqtt_server);
    String mqttPortString = mqtt_port;
    int mqttPortInt = mqttPortString.toInt();
    client.setServer(IP, mqttPortInt);
    Serial.println(mqttPortInt); 
    Serial.println(IP);
    Serial.println(IP);
    Serial.println(IP);
    while (!client.connected()) {
      
      Serial.print("Attempting MQTT connection...");
      String clientID=(String)mqtt_ID, clientUser=(String)mqtt_user, clientPass=(String)mqtt_password;
      if (client.connect(clientID.c_str(), clientUser.c_str(), clientPass.c_str())) {
        char wsChar[datalength];
        String wsString=(String)wsID;
        wsString.toCharArray(wsChar,datalength);
        client.publish(wsChar,"1",  true);
      } 
      else 
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" opening AP");
        apStart();
        writeToJson();
        Serial.println("reboot...");
        ESP.reset();
      }
    }
  }

/*
--------------------------------------
           Main procedure
--------------------------------------
*/
void loop() 
  {
    
      refresh();
      String sleepString = sleep_time;
      int sleepInt = sleepString.toInt();
      client.loop();
      ESP.deepSleep(sleepInt*1000000);
      ESP.reset();
  }

/*
--------------------------------------
           Sensors REFRESH
--------------------------------------
*/

  bool BMP280send=false,SGP30send=false,SHT31send=false;
  int BMP280Status=false, SGP30Status=false, SHT31Status=false;
  
  String BMP280_TEMP, BMP280_PRESS, BMP280_ALT;
  String SGP30_TVOC, SGP30_eCO2;
  
  bool SHT31_Heater = false;
  String  SHT31_TEMP, SHT31_HUM;

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

void SHT31_refresh()
  {
    SHT31_TEMP = (String)SHT31.readTemperature();
    SHT31_HUM = (String)SHT31.readHumidity();
    if(SHT31_TEMP!="nan") SHT31send=true;
  }

void BMP280_refresh()
  {
    if(BMP280Status==1)
    { 
      BMP280_TEMP = (String)BMP280.readTemperature();
      BMP280_PRESS = (String)(BMP280.readPressure() * 0.01);
      BMP280_ALT = (String)BMP280.readAltitude(1035.7);
      BMP280send=true;
    }
  
  }
  
void SGP30_refresh()
  {
    SGP30.setHumidity(getAbsoluteHumidity(SHT31.readTemperature(), SHT31.readHumidity()));
    if (SGP30.IAQmeasure()) SGP30send=true;
    SGP30_TVOC=(String)SGP30.TVOC;
    SGP30_eCO2=(String)SGP30.eCO2;
  }

void refresh()
  {
    int tries=0;
    do
    {
      SGP30_refresh();
      delay(10);
      tries++; 
    } 
    while(tries<10 and SGP30send==false and SGP30Status==true);
    
    tries=0;
    do 
    {
      SHT31_refresh();
      delay(10);
      tries++; 
    } 
    while(tries<10 and SHT31send==false and  SHT31Status==true);
    
    tries=0;
    do 
    {
      BMP280_refresh();
      delay(10);
      tries++; 
    }
    while(tries<10 and BMP280send==false and BMP280Status==true);
      
    String wsString=(String)mqtt_ID;
  
    if(SGP30_toggle and SGP30send)
    {
      char aSGP30[datalength], bSGP30[datalength];
      (wsString+"/SGP30/TVOC").toCharArray(aSGP30,datalength);
      (wsString+"/SGP30/eCO2").toCharArray(bSGP30,datalength);
      char tSGP30[datalength], hSGP30[datalength];
      SGP30_TVOC.toCharArray(tSGP30,datalength);
      SGP30_eCO2.toCharArray(hSGP30,datalength);
      client.publish(aSGP30, tSGP30,  true);
      delay(10);
      client.publish(bSGP30, hSGP30,  true);
      delay(10);
    }
  
    if(BMP280_toggle and BMP280send)
    {
      char aBMP280[datalength], bBMP280[datalength], cBMP280[datalength];
      (wsString+"/BMP280/temp").toCharArray(aBMP280,datalength);
      (wsString+"/BMP280/press").toCharArray(bBMP280,datalength);
      (wsString+"/BMP280/alt").toCharArray(cBMP280,datalength);
      char tBMP280[datalength], pBMP280[datalength], altBMP280[datalength];
      BMP280_TEMP.toCharArray(tBMP280,datalength);
      BMP280_PRESS.toCharArray(pBMP280,datalength);
      BMP280_ALT.toCharArray(altBMP280,datalength);
      client.publish(aBMP280, tBMP280, true);
      delay(10);
      client.publish(bBMP280, pBMP280, true);
      delay(10);
      client.publish(cBMP280, altBMP280, true);
      delay(10);
    }
  
    if(SHT31_toggle and SHT31send)
    {      
      char aSHT31[datalength], bSHT31[datalength];
      (wsString+"/SHT31/temp").toCharArray(aSHT31,datalength);
      (wsString+"/SHT31/hum").toCharArray(bSHT31,datalength);
      char tSHT31[datalength], hSHT31[datalength];
      SHT31_TEMP.toCharArray(tSHT31,datalength);
      SHT31_HUM.toCharArray(hSHT31,datalength);
      client.publish(aSHT31, tSHT31,  true);
      delay(10);
      client.publish(bSHT31, hSHT31,  true);
      delay(10); 
    }
  }
  

/*
--------------------------------------
            Sensors SETUP
--------------------------------------
*/

void SGP30setup()
  {
    Serial.print("SGP30: ");
    SGP30Status=SGP30.begin();
    Serial.println((String)SGP30Status);
  }

void SHT31setup()
  {
    Serial.print("SHT31: ");
    SHT31Status=SHT31.begin(0x44);
    Serial.println((String)SHT31Status);
    SHT31.heater(0);
  }

void BMP280setup()
  {
    Serial.print("BMP280: ");
    BMP280Status=BMP280.begin(0x76);
    Serial.println((String)BMP280Status);
    BMP280.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  }
