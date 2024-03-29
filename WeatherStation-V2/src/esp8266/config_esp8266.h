#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include <SoftwareSerial.h>
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
#include "SparkFun_ENS160.h"
#include <AHTxx.h>
#include "SparkFun_SCD4x_Arduino_Library.h"
#include <PMserial.h> 

//main
void rst();
void sleepAndReset();
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
void AHT2xRead();
void ENS160Read();
void SCD4XRead();
void PMSx003Read();
//mqtt.h
void callback(char *topic, byte *payload, unsigned int length);
void mqttSetup();
void mqttSend();
void sht31Send();
void sgp30Send();
void bmp280Send();
void AHT2xSend();
void ENS160Send();
void SCD4XSend();
void PMSx003Send();
//wifi.h
void httpDefault();
void httpHome();
void httpData();
void wifiStart();


void writeToJson();

// Set the size of the JSON object
const int JSON_OBJECT_SIZE = 2048;

// Set the path to the JSON file
const char *JSON_FILE_PATH = "/config1.json";



struct {
  int 
    mqtt_port, 
    refreshTime;
  bool 
    BMP280_toggle, 
    SHT31_toggle,
    SGP30_toggle,
    AHT2x_toggle,
    ENS160_toggle,
    lowPowerMode_toggle;
  String 
    wifi_ssid,
    wifi_pass,
    mqtt_server,
    mqtt_user,
    mqtt_password,
    hotspot_ssid,
    hotspot_pass,
    mdns_hostname;
} values;



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
bool AHT2x_toggle = false;
bool ENS160_toggle = false;
bool SCD4x_toggle = true;
bool PMSx003_toggle = false;
bool lowPowerMode_toggle = false;
int refreshTime = 30;

String mdns_hostname = "tmp";
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


constexpr auto PMS_RX = 14;
constexpr auto PMS_TX = 13;
SerialPM pms(PMS5003, PMS_RX, PMS_TX); // PMSx003, RX, TX


AHTxx aht20(AHTXX_ADDRESS_X38, AHT2x_SENSOR); 
SparkFun_ENS160 myENS; 

Adafruit_SGP30 sgp30;
Adafruit_BMP280 bmp280;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
SCD4x scd4x;

float sgp30_tvoc=0, sgp30_co2=0, sgp30_eth=0, sgp30_h2=0;
float bmp280_temp=0, bmp280_press=0, bmp280_alt=0;
float sht31_temp=0, sht31_hum=0;
float scd4x_temp=0, scd4x_hum=0, scd4x_co2=0;

float ens160_eco2=0, ens160_tvoc=0, ens160_aqi=0;
float aht2x_temp=0, aht2x_hum=0;
uint16_t pmsx003Data[9]={0};
float pmsx003Extra[3]={0};
/*
pm01 	uint16_t 	PM 	<= 1.0 µm 	µg/m³ 	PM1.0, ultra fine particles
pm25 	uint16_t 	PM 	<= 2.5 µm 	µg/m³ 	PM2.5, fine particles
pm10 	uint16_t 	PM 	<= 10 µm 	µg/m³ 	PM10
n0p3 	uint16_t 	NC 	>= 0.3 µm 	#/100 cm³ 	
n0p5 	uint16_t 	NC 	>= 0.5 µm 	#/100 cm³ 	
n1p0 	uint16_t 	NC 	>= 1.0 µm 	#/100 cm³ 	
n2p5 	uint16_t 	NC 	>= 2.5 µm 	#/100 cm³ 	
n5p0 	uint16_t 	NC 	>= 5.0 µm 	#/100 cm³ 	
n10p0 	uint16_t 	NC 	>= 10 µm 	#/100 cm³ 	
pm 	uint16_t[3] 	PM 	<= 1,2.5,10 µm 	µg/m³ 	array containing pm01,pm25,pm10
nc 	uint16_t[6] 	NC 	>= 0.3,0.5,1,5,10 µm 	#/100cm³ 	array containing n0p3,..,n10p0
data 	uint16_t[9] 	PM/NC 			all PM/NC data pm01,..,n10p0
temp 	float 	T 		°C 	temperature
rhum 	float 	RH 		% 	relative humidity
hcho 	float 	HCHO 		mg/m³ 	formaldehyde concentration
extra 	float[3] 	T/RH/HCHO 			array containing temp,rhum,hcho
*/

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