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
#include <PM1006K.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "SparkFun_ENS160.h"
#include <AHTxx.h> 
#include <sps30.h>
#include <SensirionI2cScd4x.h> 

#if __has_include("../secrets.h")
  #include "../secrets.h"
#else
  #define NO_PASS_CHANGE "__*passwordDidNotChange*__"

  #define WIFI_SSID ""
  #define WIFI_PASS ""
  #define MQTT_SERVER ""
  #define MQTT_PORT 1883
  #define MQTT_USER ""
  #define MQTT_PASSWORD ""
  #define MQTT_MESSAGEROOT "Weatherstation"
  #define MDNS_HOSTNAME "Weatherstation"
  #define HOTSPOT_SSID "Weatherstation"
  #define HOTSPOT_PASS ""

  #define LOWPOWERMODE_TOGGLE false
  #define AHT2X_TOGGLE false
  #define BMP280_TOGGLE false
  #define ENS160_TOGGLE false
  #define PM1006K_TOGGLE false
  #define PMSX003_TOGGLE false
  #define SCD4X_TOGGLE false
  #define SGP30_TOGGLE false
  #define SHT31_TOGGLE false
  #define SPS30_TOGGLE false
#endif

//device
#if defined(SEEED_XIAO_ESP32C3)
  int sda=6;
  int scl=7;
  #define device "SEEED_XIAO_ESP32C3"
#elif defined(LOLIN_S2_MINI)
  int sda=33;
  int scl=35;
  #define device "LOLIN_S2_MINI"
#elif defined(SEEED_XIAO_ESP32C6)
  int sda=22;
  int scl=23;
  #define device "SEEED_XIAO_ESP32C6"
#elif defined(ESP32DOIT_DEVKIT_V1)
  int sda=21;
  int scl=22;
  #define device "ESP32DOIT-DEVKIT-V1"
#elif defined(AZ_DELIVERY_DEVKIT_V4)
  int sda=21;
  int scl=22;
  #define device "AZ-DELIVERY-DEVKIT-V4"
#elif defined(ESP32)
  int sda=21;
  int scl=22;
  #define device "GenericESP32"
#endif

//main
void rst();
void sleepAndReset(); 
String runningTime();
//json.h
void saveNewConfig();
void loadConfig();
bool saveConfig();
void readConfig();
//sensors.h
//void sendReadingsMqtt(String mqtt_messageRoot, String sensor, String *SensorSuffix,float *sensorVariables);
void sensorsBegin();
int getAbsoluteHumidity(float temperature, float humidity);
//mqtt.h
void callback(char *topic, byte *payload, unsigned int length);
void mqttConnect();
void sendMqtt(String topic, String msg, bool retain);
//wifi.h
void httpDefault();
void httpHome();
void httpData();
void wifiStart();

// Set the size of the JSON object
const int JSON_OBJECT_SIZE = 1536;

// Set the path to the JSON file
const char *JSON_FILE_PATH = "/config1.json";


// Define the variables


String wifi_ssid = WIFI_SSID;
String wifi_pass = WIFI_PASS;
String mqtt_server = MQTT_SERVER;
int mqtt_port = MQTT_PORT;
String mqtt_user = MQTT_USER;
String mqtt_password = MQTT_PASSWORD;
String mqtt_messageRoot = MQTT_MESSAGEROOT;
String mdns_hostname = MDNS_HOSTNAME;
String hotspot_ssid = HOTSPOT_SSID;
String hotspot_pass = HOTSPOT_PASS;

int refreshTime=REFRESH_TIME; 

bool lowPowerMode_toggle = LOWPOWERMODE_TOGGLE;
bool AHT2x_toggle = AHT2X_TOGGLE;
bool BMP280_toggle = BMP280_TOGGLE;
bool ENS160_toggle = ENS160_TOGGLE;
bool PM1006K_toggle = PM1006K_TOGGLE;
bool PMSx003_toggle = PMSX003_TOGGLE;
bool SCD4x_toggle = SCD4X_TOGGLE;
bool SGP30_toggle = SGP30_TOGGLE;
bool SHT31_toggle = SHT31_TOGGLE;
bool SPS30_toggle = SPS30_TOGGLE;

#define SENSOR_COUNT 9
bool* toggles[SENSOR_COUNT] = {
    &AHT2x_toggle,
    &BMP280_toggle,
    &ENS160_toggle,
    &PM1006K_toggle,
    &PMSx003_toggle,
    &SCD4x_toggle,
    &SGP30_toggle,
    &SHT31_toggle,
    &SPS30_toggle
};
String toggleIDName[SENSOR_COUNT][2] = {
    {"AHT2x_toggle", "AHT2x"},
    {"BMP280_toggle", "BMP280"},
    {"ENS160_toggle", "ENS160"},
    {"PM1006K_toggle", "PM1006K"},
    {"PMSx003_toggle", "PMSx003"},
    {"SCD4x_toggle", "SCD4x"},
    {"SGP30_toggle", "SGP30"},
    {"SHT31_toggle", "SHT31"},
    {"SPS30_toggle", "SPS30"}
};

bool wifiConnectionType; //0 hotspot, 1 wifi
bool reconfigure=0;
bool runningTasks=1;
bool sensorStart=1;

taskid_t AHT2x_task, BMP280_task, ENS160_task, PM1006K_task, PMSx003_task, SCD4x_task, SGP30_task, SHT31_task, SPS30_task;

float aht2x_temp=nan(""), aht2x_hum=nan("");
float bmp280_temp=nan(""), bmp280_press=nan(""), bmp280_alt=nan("");
float ens160_eco2=nan(""), ens160_tvoc=nan(""), ens160_aqi=nan("");
float pm1006k_pm1_0 = nan(""), pm1006k_pm2_5 = nan(""), pm1006k_pm10_0 = nan("");
float pmsx003_pm1_0 = nan(""), pmsx003_pm2_5 = nan(""), pmsx003_pm10_0 = nan("");
float scd4x_co2=nan(""), scd4x_temp=nan(""), scd4x_hum=nan("");
float sgp30_tvoc=nan(""), sgp30_co2=nan(""), sgp30_eth=nan(""), sgp30_h2=nan("");
float sht31_temp=nan(""), sht31_hum=nan("");
float sps30_pm1_0 = nan(""), sps30_pm2_5 = nan(""), sps30_pm10_0 = nan("");

//TODO update sw/hw version
String SWversion="3.1.0"; //software version
String HWversion="0.1"; //hardware version
String Model="Weatherstation";
String ModelID=device; //ESP32, SEEED_XIAO_ESP32C3... 
String Manufacturer="@Nikpesu";
String UniqueDeviceID=String(ESP.getEfuseMac());

// Define the JSON document size
// StaticJsonDocument<1024> doc;
DynamicJsonDocument doc(JSON_OBJECT_SIZE);

const IPAddress apIp(192, 168, 0, 1);
DNSServer dnsServer;
WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);


Adafruit_SGP30 sgp30;
Adafruit_BMP280 bmp280;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
SparkFun_ENS160 myENS; 
AHTxx aht2x(AHTXX_ADDRESS_X38, AHT2x_SENSOR);
SensirionI2cScd4x scd4x;

#define PM1006K_RX_PIN 8
#define PM1006K_TX_PIN 9
PM1006K * pm1006k;


String htmlStart="<!DOCTYPE html> \
<html> \
 \
<head> \
  <title>Weatherstation</title> \
  <meta name=\"viewport\" content=\"initial-scale=1.0\"> \
  <style> \
    form { \
      width: 75%; \
      font-family: Arial, Helvetica, sans-serif; \
      font-size: large; \
      font-weight: bold; \
    } \
 \
    #myForm>div { \
 \
      display: flex; \
      justify-content: center; \
      align-content: center; \
      flex-direction: column; \
      width: auto; \
      align-content: center; \
    } \
 \
    body { \
      background-color: rgba(196, 196, 196, 0.411); \
      font-family: Arial, Helvetica, sans-serif; \
      display: flex; \
      justify-content: center; \
      align-content: center; \
      flex-direction: column; \
      width: auto; \
      align-content: center; \
      flex-wrap: wrap; \
    } \
 \
    .senzori { \
      display: flex; \
      justify-content: space-around; \
      align-items: center; \
      width: 100%; \
      flex-direction: row; \
 \
    } \
 \
    .senzor { \
      display: flex; \
      flex-direction: column; \
    } \
 \
    label { \
      align-self: center; \
      margin-top: 1em; \
    } \
 \
    input { \
      height: 1.7em; \
      font-size: 1.6em; \
      background-color: gainsboro; \
      border-radius: 0.3em; \
      border: gray 1px solid; \
      transition: 0.2s; \
      color: rgb(0, 31, 78); \
    } \
 \
    input[type=\"button\"] { \
      height: 3em; \
      margin-top: 1em; \
 \
    } \
    input:hover { \
      border-color: rgb(82, 82, 82); \
      background-color: rgb(175, 175, 175); \
      transition: 0.2s; \
    } \
     \
    input[type=\"button\"]:active { \
      border-color: rgb(80, 78, 78); \
      background-color: rgb(100, 112, 117); \
      color: white; \
 \
    } \
 \
    input[type=number]::-webkit-inner-spin-button, \
    input[type=number]::-webkit-outer-spin-button { \
      -webkit-appearance: none; \
      margin: 1; \
    } \
 \
    input[type=number] { \
      -moz-appearance: textfield; \
      appearance: textfield; \
    } \
  </style>";

#endif