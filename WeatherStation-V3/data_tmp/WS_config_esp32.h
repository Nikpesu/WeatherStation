#ifndef GLOBAL_H
#define GLOBAL_H


#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <DNSServer.h>
#include <ArduinoOTA.h>
#include <Streaming.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_SGP30.h>
#include <Adafruit_SHT31.h>
#include <PM1006K.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "SparkFun_ENS160.h"
#include <AHTxx.h> 
#include <sps30.h>
#include "SparkFun_SCD4x_Arduino_Library.h" 
#include <PMserial.h> 
//#include <PString.h>
//#include <TaskManagerIO.h>

#if __has_include("../secrets.h")
  #include "../secrets.h"
#else
  #define NO_PASS_CHANGE "__*passwordDidNotChange*__"

  #define HOTSPOT_PIN 3
  #define RESET_CONFIG_PIN 2

  #define PMS5003_TYPE PMS5003

  #define WIFI_SSID ""
  #define WIFI_PASS ""
  #define MQTT_SERVER ""
  #define MQTT_PORT 1883
  #define MQTT_USER ""
  #define MQTT_PASSWORD ""
  #define MQTT_MESSAGEROOT "WeatherStations/ws1"
  #define MDNS_HOSTNAME "ws1"
  #define HOTSPOT_SSID "ws1"
  #define HOTSPOT_PASS ""
  #define SUGGESTED_AREA  ""

  #define REFRESH_TIME 30
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
#if defined(SEEED_XIAO_ESP32C3) //esp32
  int sda=6;
  int scl=7;
  #define PM1006K_RX_PIN 8
  #define PM1006K_TX_PIN 9
  #define PMSX003_RX_PIN 13
  #define PMSX003_TX_PIN 14
  #define device "SEEED_XIAO_ESP32C3"
#elif defined(LOLIN_S2_MINI)
  int sda=33;
  int scl=35;
  #define PM1006K_RX_PIN 8
  #define PM1006K_TX_PIN 9
  #define PMSX003_RX_PIN 13
  #define PMSX003_TX_PIN 14
  #define device "LOLIN_S2_MINI"
#elif defined(SEEED_XIAO_ESP32C6)
  int sda=22;
  int scl=23;
  #define PM1006K_RX_PIN 8
  #define PM1006K_TX_PIN 9
  #define PMSX003_RX_PIN 13
  #define PMSX003_TX_PIN 14
  #define device "SEEED_XIAO_ESP32C6"
#elif defined(ESP32DOIT_DEVKIT_V1)
  int sda=21;
  int scl=22;
  #define PM1006K_RX_PIN 8
  #define PM1006K_TX_PIN 9
  #define PMSX003_RX_PIN 13
  #define PMSX003_TX_PIN 14
  #define device "ESP32DOIT-DEVKIT-V1"
#elif defined(AZ_DELIVERY_DEVKIT_V4)
  int sda=21;
  int scl=22;
  #define PM1006K_RX_PIN 8
  #define PM1006K_TX_PIN 9
  #define PMSX003_RX_PIN 13
  #define PMSX003_TX_PIN 14
  #define device "AZ-DELIVERY-DEVKIT-V4"
#elif defined(ESP32)
  int sda=21;
  int scl=22;
  #define PM1006K_RX_PIN 8
  #define PM1006K_TX_PIN 9
  #define PMSX003_RX_PIN 13
  #define PMSX003_TX_PIN 14
  #define device "GenericESP32"
#elif defined(D1_MINI)  //esp8266
  int sda=4;
  int scl=5;
  #define PM1006K_RX_PIN SERIAL_TX_ONLY
  #define PM1006K_TX_PIN 9
  #define PMSX003_RX_PIN 13
  #define PMSX003_TX_PIN 14
  #define device "D1_MINI"
#elif defined(ESP8266)
  int sda=4;
  int scl=5;
  #define PM1006K_RX_PIN 8
  #define PM1006K_TX_PIN 9
  #define PMSX003_RX_PIN 13
  #define PMSX003_TX_PIN 14
  #define device "GenericESP32"
#endif

//main
void startProgram();
void loopedProgram();
void sleepAndReset(); 
void rst();
String runningTime();
String runningTime();
//json.h
void loadConfig();
bool saveConfig();
void updateFieldsToNative();
void updateFieldsToString();
//sensors.h
void sensorLoop();
void pm1006kRead();
void sensorsBegin();
void allSetupSend();
void allReconfigure();
int getAbsoluteHumidity(float temperature, float humidity);
//mqtt.h
void callback(char *topic, byte *payload, unsigned int length);
void mqttConnect();
void sendMqtt(String topic, String msg, bool retain);
void mqttSetup();
String makeMqttSensorTopic(int i, String sensor, String status_topic, String deviceClass[], String SensorSuffix[],  String unitOfMeasurement[]);
//wifi.h
String IpAddress2String(const IPAddress& ipAddress);
String rssiToChart(int8_t);
void apStart();
void httpConfig();
void httpData();
void httpDefault();
void httpfields();
void httpHome();
void httpRestart();
void httpsensorAndFieldsIDs();
void httpSensors();
void httpServicesStart();
void httpTitles();
void httpWiFi();
void wifiStart();


// Set the size of the JSON object
const int JSON_OBJECT_SIZE = 1536;

// Set the path to the JSON file
const char *JSON_FILE_PATH = "/config.json";

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
bool lowPowerMode_toggle = LOWPOWERMODE_TOGGLE;
int refreshTime=REFRESH_TIME; 
String suggested_area=SUGGESTED_AREA; 

String mqtt_port_str = String(mqtt_port);
String lowPowerMode_toggle_str = String(lowPowerMode_toggle);
String refreshTime_str = String(refreshTime);
String test = "1"; 

bool AHT2x_toggle = AHT2X_TOGGLE;
bool BMP280_toggle = BMP280_TOGGLE;
bool ENS160_toggle = ENS160_TOGGLE;
bool PM1006K_toggle = PM1006K_TOGGLE;
bool PMSx003_toggle = PMSX003_TOGGLE;
bool SCD4x_toggle = SCD4X_TOGGLE;
bool SGP30_toggle = SGP30_TOGGLE;
bool SHT31_toggle = SHT31_TOGGLE;
bool SPS30_toggle = SPS30_TOGGLE;


#define FIELD_COUNT 13
String fieldsIDNameTypePlaceholder[FIELD_COUNT][4] = {
    {"wifi_ssid", "Wifi SSID:", "text", "SSID"},
    {"wifi_pass", "Wifi Password:", "password", "password"},
    {"mqtt_server", "MQTT Server:", "text", "server address"},
    {"mqtt_port", "MQTT Port:", "number", "default: 1883"},
    {"mqtt_user", "MQTT Username:", "text", "username"},
    {"mqtt_password", "MQTT Password:", "password", "password"},
    {"mqtt_messageRoot", "MQTT Message Root (\\\"msgroot\\\" + \\\"/sensor/data\\\"):", "text", "root/topic"},
    {"mdns_hostname", "mDNS Hostname (without .local):", "text", "device.local"},
    {"hotspot_ssid", "Hotspot SSID:", "text", "SSID"},
    {"hotspot_pass", "Hotspot Password:", "password", "password"},
    {"lowPowerMode_toggle", "Low Power Mode (can only be configured in hotspot mode!!)", "checkbox", ""},
    {"refreshTime", "Refresh Time:", "number", "default: 30"},
    {"suggested_area", "Suggested area:", "text", "for example: Outside, Inside, Bedroom..."}
};
String* fields[FIELD_COUNT] = {
    &wifi_ssid,
    &wifi_pass,
    &mqtt_server,
    &mqtt_port_str,
    &mqtt_user,
    &mqtt_password,
    &mqtt_messageRoot,
    &mdns_hostname,
    &hotspot_ssid,
    &hotspot_pass,
    &lowPowerMode_toggle_str,
    &refreshTime_str,
    &suggested_area
};

#define SENSOR_COUNT 9
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

bool wifiConnectionType; //0 hotspot, 1 wifi
bool reconfigure=0;
bool runningTasks=1;
bool sensorStart=1;

//taskid_t AHT2x_task, BMP280_task, ENS160_task, PM1006K_task, PMSx003_task, SCD4x_task, SGP30_task, SHT31_task, SPS30_task;

float aht2x_temp=nan(""), aht2x_hum=nan("");
float bmp280_temp=nan(""), bmp280_press=nan(""), bmp280_alt=nan("");
float ens160_eco2=nan(""), ens160_tvoc=nan(""), ens160_aqi=nan("");
float pm1006k_pm1_0 = nan(""), pm1006k_pm2_5 = nan(""), pm1006k_pm10_0 = nan("");
float pmsx003_pm1_0 = nan(""), pmsx003_pm2_5 = nan(""), pmsx003_pm10_0 = nan(""), pmsx003_temp = nan(""), pmsx003_hum = nan(""), pmsx003_hcho = nan("");
float scd4x_co2=nan(""), scd4x_temp=nan(""), scd4x_hum=nan("");
float sgp30_tvoc=nan(""), sgp30_co2=nan(""), sgp30_eth=nan(""), sgp30_h2=nan("");
float sht31_temp=nan(""), sht31_hum=nan("");
float sps30_pm1_0 = nan(""), sps30_pm2_5 = nan(""), sps30_pm10_0 = nan("");

//TODO update sw/hw version
String SWversion="3.3.0"; //software version
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
SCD4x scd4x;
//SPS30 sps30;

PM1006K * pm1006k;


PMS pmsx003Type=PMS5003_TYPE;
SerialPM pmsx003(PMS5003, PMSX003_RX_PIN, PMSX003_TX_PIN); // PMSx003, RX, TX


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
#