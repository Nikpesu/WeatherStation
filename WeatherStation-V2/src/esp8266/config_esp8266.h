#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <LittleFS.h>
#include <DNSServer.h>
#include <ArduinoOTA.h>
#include <PString.h>
#include <Streaming.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_SGP30.h>
#include <Adafruit_SHT31.h>
#include <TaskManagerIO.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

//main
void rst();
//json.h
void saveNewConfig();
void loadConfig();
bool saveConfig();
void readConfig();
//sensors.h
void sensorsBegin();
void sht31Read();
uint32_t getAbsoluteHumidity(float temperature, float humidity);
void sgp30Read();
void bmp280Read();
//mqtt.h
void callback(char *topic, byte *payload, unsigned int length);
void mqttSetup();
void mqttSend();
void sht31Send();
void sgp30Send();
void bmp280Send();
//wifi.h
void httpDefault();
void httpHome();
void httpData();
void wifiStart();

// Set the size of the JSON object
const int JSON_OBJECT_SIZE = 2048;

// Set the path to the JSON file
const char *JSON_FILE_PATH = "/config1.json";

int refreshTime=20;

// Define the variables
String wifi_ssid = "";
String wifi_pass = "";
String mqtt_server = "";
int mqtt_port = 0;
String mqtt_user = "";
String mqtt_password = "";
String mqtt_messageRoot = "";
bool BMP280_toggle = false;
bool SHT31_toggle = false;
bool SGP30_toggle = false;
String mdns_hostname = "";
String hotspot_ssid = "Weatherstation";
String hotspot_pass = "";

// Define the JSON document size
// StaticJsonDocument<1024> doc;
DynamicJsonDocument doc(JSON_OBJECT_SIZE);

const IPAddress apIp(192, 168, 0, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);


Adafruit_SGP30 sgp30;
Adafruit_BMP280 bmp280;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

float sgp30_tvoc=0, sgp30_co2=0, sgp30_eth=0, sgp30_h2=0;
float bmp280_temp=0, bmp280_press=0, bmp280_alt=0;
float sht31_temp=0, sht31_hum=0;


#endif