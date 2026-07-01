#ifndef GLOBAL_H
#define GLOBAL_H


#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#include <HardwareSerial.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#elif defined(ESP8266)
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PString.h> 
#include <SoftwareSerial.h> 
#endif
#include "bt_hci_bridge.h"
#include <LittleFS.h>
#include <DNSServer.h>
#include <ArduinoOTA.h>
#include <Streaming.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Crypto.h>
#include <AES.h>
#include <CTR.h>
#include <SHA256.h>
#if defined(ESP32)
#include <esp_system.h>
#endif
#include <Adafruit_BMP280.h>
#include <SparkFun_BMP581_Arduino_Library.h>
#include <Adafruit_SGP30.h>
#include <Adafruit_SHT31.h>
#include <PM1006K.h>
#include <PubSubClient.h>
#include "SparkFun_ENS160.h"
#include <AHTxx.h> 
#include <SensirionI2cSps30.h>
#include "SparkFun_SCD4x_Arduino_Library.h"
#include <PMserial.h>
#include <Adafruit_NeoPixel.h>
#include <deque>

//#include <PString.h>
//#include <TaskManagerIO.h>


//default config
#if __has_include("../secrets.h")
  #include "../secrets.h"
#else
  #define NO_PASS_CHANGE "__*passwordDidNotChange*__"

  #define WIFI_PASS ""
  #define HOTSPOT_PASS "12345678"
  #define MQTT_USER ""
  #define MQTT_PASSWORD ""

  #define MDNS_HOSTNAME "ws1"
  #define WIFI_SSID ""
  #define HOTSPOT_SSID "ws1"
  #define MQTT_SERVER ""
  #define MQTT_PORT 1883
  #define MQTT_MESSAGEROOT "WeatherStations/ws1"
  #define SUGGESTED_AREA  ""

  #define LOOP_TIME 100 //in ms (for looped program)
  #define REFRESH_TIME 30 //in s (for sensors)

  #define LOWPOWERMODE_TOGGLE false
  #define AHT2X_TOGGLE false
  #define BMP280_TOGGLE false
  #define BMP580_TOGGLE false
  #define ENS160_TOGGLE false
  #define PM1006K_TOGGLE false
  #define PMSX003_TOGGLE false
  #define SCD4X_TOGGLE false
  #define SGP30_TOGGLE false
  #define SHT31_TOGGLE false
  #define SPS30_TOGGLE false
#endif

// Default for sensors added after a user's secrets.h was written, so an existing
// secrets.h without this key still compiles.
#ifndef BMP580_TOGGLE
  #define BMP580_TOGGLE false
#endif

//device
#define device ARDUINO_BOARD

#if defined(SEEED_XIAO_ESP32S3)
  #define SDA_PIN D4
  #define SCL_PIN D5
  #define PM1006K_TX_PIN D9
  #define PM1006K_RX_PIN D8
  #define PMSX003_RX_PIN D7
  #define PMSX003_TX_PIN D6
  #define RESET_CONFIG_PIN D0
  #define HOTSPOT_PIN D1
  #define FAN_PIN -1
  #define BOARDIMGLINK "https://wdcdn.qpic.cn/MTY4ODg1Nzc0ODUwMjM3NA_318648_dMoXitoaQiq2N3-a_1711678067?w=1486&h=1228"
#elif defined(SEEED_XIAO_ESP32C3)
  #define SDA_PIN D4
  #define SCL_PIN D5
  #define PM1006K_TX_PIN D9
  #define PM1006K_RX_PIN D8
  #define PMSX003_RX_PIN D7
  #define PMSX003_TX_PIN D6
  #define RESET_CONFIG_PIN D0
  #define HOTSPOT_PIN D1
  #define FAN_PIN D2
  #define BOARDIMGLINK "https://files.seeedstudio.com/wiki/XIAO_WiFi/pin_map-2.png"
#elif defined(LOLIN_S2_MINI)
  #define SDA_PIN 4
  #define SCL_PIN 5
  #define PM1006K_TX_PIN 13
  #define PM1006K_RX_PIN 14
  #define PMSX003_RX_PIN 14
  #define PMSX003_TX_PIN 13
  #define RESET_CONFIG_PIN 0
  #define HOTSPOT_PIN 12
  #define FAN_PIN 2
  #define BOARDIMGLINK "https://www.wemos.cc/en/latest/_images/s2_mini_v1.0.0_4_16x9.jpg"
#elif defined(SEEED_XIAO_ESP32C6)
  #define SDA_PIN D4
  #define SCL_PIN D5
  #define PM1006K_TX_PIN D9
  #define PM1006K_RX_PIN D8
  #define PMSX003_RX_PIN D7
  #define PMSX003_TX_PIN D6
  #define RESET_CONFIG_PIN D0
  #define HOTSPOT_PIN D1
  #define FAN_PIN D2
  #define BOARDIMGLINK "https://wdcdn.qpic.cn/MTY4ODg1Nzc0ODUwMjM3NA_318648_dMoXitoaQiq2N3-a_1711678067?w=1486&h=1228"
#elif defined(ESP32DOIT_DEVKIT_V1)
  #define SDA_PIN 21
  #define SCL_PIN 22
  #define PM1006K_TX_PIN D9
  #define PM1006K_RX_PIN D8
  #define PMSX003_RX_PIN D7
  #define PMSX003_TX_PIN D6
  #define RESET_CONFIG_PIN D0
  #define HOTSPOT_PIN D1
  #define FAN_PIN D2
  #define BOARDIMGLINK "https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2018/08/ESP32-DOIT-DEVKIT-V1-Board-Pinout-36-GPIOs-updated.jpg?resize=750%2C538&quality=100&strip=all&ssl=1"
#elif defined(AZ_DELIVERY_DEVKIT_V4)
  #define SDA_PIN 21
  #define SCL_PIN 22
  #define PM1006K_TX_PIN D9
  #define PM1006K_RX_PIN D8
  #define PMSX003_RX_PIN D7
  #define PMSX003_TX_PIN D6
  #define RESET_CONFIG_PIN D0
  #define HOTSPOT_PIN D1
  #define FAN_PIN D2
  #define BOARDIMGLINK "https://media.s-bol.com/x75zGxQXKlZl/Q0XOo6q/550x558.jpg"
#elif defined(ESP32)
  #define SDA_PIN 21
  #define SCL_PIN 22
  #define PM1006K_TX_PIN D9
  #define PM1006K_RX_PIN D8
  #define PMSX003_RX_PIN D7
  #define PMSX003_TX_PIN D6
  #define RESET_CONFIG_PIN D0
  #define HOTSPOT_PIN D1
  #define FAN_PIN D2
  #define BOARDIMGLINK "https://www.upesy.com/cdn/shop/files/doc-esp32-pinout-reference-wroom-devkit.png"
#elif defined(D1_MINI)  //esp8266
  #define SDA_PIN D2
  #define SCL_PIN D1
  #define PM1006K_RX_PIN D5
  #define PM1006K_TX_PIN D7 //RX / GPIO3 TX / GPIO0
  #define PMSX003_RX_PIN D5
  #define PMSX003_TX_PIN D7
  #define RESET_CONFIG_PIN D3
  #define HOTSPOT_PIN D6
  #define FAN_PIN D4
  #define BOARDIMGLINK "https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2019/05/ESP8266-WeMos-D1-Mini-pinout-gpio-pin.png?resize=715%2C362&quality=100&strip=all&ssl=1"
#elif defined(ESP8266)
  #define SDA_PIN 4
  #define SCL_PIN 5
  #define PM1006K_RX_PIN SERIAL_TX_ONLY
  #define PM1006K_TX_PIN D8
  #define PMSX003_RX_PIN D5
  #define PMSX003_TX_PIN D7
  #define RESET_CONFIG_PIN D3
  #define HOTSPOT_PIN D6
  #define FAN_PIN D2
  #define BOARDIMGLINK "https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2019/05/ESP8266-NodeMCU-kit-12-E-pinout-gpio-pin.png?resize=817%2C542&quality=100&strip=all&ssl=1"
#endif

// ─── Sensor status-indicator LEDs (WS2812 / NeoPixel) ───
// A short addressable strip whose LEDs mirror the health of the enabled sensors:
// one LED per enabled sensor (in sensor order), green = sensor is producing valid
// data, red = enabled but no data yet / not responding, off = unused slot. The
// whole feature is toggled on the website, and the data pin is user-configurable
// (defaults to D8 on the XIAO boards). Note: D8 is also the default PM1006K/PMSx003
// RX pin, so move the LED pin if you run a particulate sensor.
#ifndef LED_PIN
  #if defined(SEEED_XIAO_ESP32C6) || defined(SEEED_XIAO_ESP32C3) || defined(SEEED_XIAO_ESP32S3)
    #define LED_PIN D8
  #else
    #define LED_PIN 8
  #endif
#endif
#ifndef LED_INDICATOR_TOGGLE
  #define LED_INDICATOR_TOGGLE false
#endif
#define LED_COUNT 5            // default number of active LEDs on the strip
#define LED_MAX_COUNT 16       // capacity: most LEDs the user can configure
#define LED_BRIGHTNESS 40      // 0-255, fixed brightness when auto-brightness is off
#define LED_BRIGHTNESS_MIN 2   // 0-255, dimmest the LEDs go in the dark (auto mode)
#define LED_BRIGHTNESS_MAX 150 // 0-255, brightest the LEDs go in daylight (auto mode)

// Ambient-light (LDR) auto-brightness. The LDR forms a divider with the on-board
// 10k pull-up to 3.3V (LDR to GND), read on an ADC-capable pin. Brighter ambient
// -> lower ADC reading -> brighter LEDs; darker -> dimmer, so they don't glare at
// night. Default pin D0. NOTE: D0 is also RESET_CONFIG_PIN on the XIAO boards —
// move one of them if you use both.
#ifndef LDR_PIN
  #if defined(SEEED_XIAO_ESP32C6) || defined(SEEED_XIAO_ESP32C3) || defined(SEEED_XIAO_ESP32S3)
    #define LDR_PIN D0
  #else
    #define LDR_PIN 0
  #endif
#endif
#ifndef LDR_BRIGHTNESS_TOGGLE
  #define LDR_BRIGHTNESS_TOGGLE false
#endif
#define LDR_ADC_MAX 4095 // 12-bit ADC full-scale (ESP32 analogRead default)

// Onboard ESP status LED, used for the boot blink (separate from the WS2812 strip).
// Defaults to the board's built-in LED. XIAO and ESP8266 built-in LEDs are active-low
// (lit when the pin is driven LOW), most ESP32 devkit LEDs are active-high.
#ifndef ESP_STATUS_LED_PIN
  #ifdef LED_BUILTIN
    #define ESP_STATUS_LED_PIN LED_BUILTIN
  #else
    #define ESP_STATUS_LED_PIN 2
  #endif
#endif
#ifndef ESP_STATUS_LED_ACTIVE_LOW
  #if defined(SEEED_XIAO_ESP32C3) || defined(SEEED_XIAO_ESP32C6) || defined(SEEED_XIAO_ESP32S3) || defined(ESP8266)
    #define ESP_STATUS_LED_ACTIVE_LOW 1
  #else
    #define ESP_STATUS_LED_ACTIVE_LOW 0
  #endif
#endif

// Number of entries in ledMeasures[] (WS_leds.h). Used to size the per-measurement
// calibration-offset array. A static_assert in WS_leds.h keeps the two in sync.
#define SENSOR_MEASURE_COUNT 32

// Board "Dx" pin-name -> GPIO number lookup, so pins can be entered either as a
// raw GPIO number or as the silkscreen "Dx" label (resolved by pinFromString()).
#if defined(SEEED_XIAO_ESP32C3) || defined(SEEED_XIAO_ESP32C6) || defined(SEEED_XIAO_ESP32S3)
  #define DPIN_COUNT 11
  static const int kDPinMap[DPIN_COUNT] = { D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10 };
#elif defined(D1_MINI) || defined(ESP8266)
  #define DPIN_COUNT 9
  static const int kDPinMap[DPIN_COUNT] = { D0,D1,D2,D3,D4,D5,D6,D7,D8 };
#else
  // Generic ESP32 boards don't reliably define Dx names; Dx input falls back to
  // being parsed as a plain GPIO number.
  #define DPIN_COUNT 0
#endif

// Resolve a pin entered as a number ("19") or a board label ("D8") to its GPIO
// number. Unknown/blank input yields -1 (disabled).
inline int pinFromString(const String &s)
{
  String t = s;
  t.trim();
  if (t.length() == 0) return -1;
  if ((t[0] == 'D' || t[0] == 'd') && t.length() > 1)
  {
    bool digits = true;
    for (unsigned int i = 1; i < t.length(); i++)
      if (!isDigit(t[i])) { digits = false; break; }
    if (digits)
    {
      int n = t.substring(1).toInt();
#if DPIN_COUNT > 0
      if (n >= 0 && n < DPIN_COUNT) return kDPinMap[n];
#endif
    }
  }
  return t.toInt();
}

//main
void startProgram();
void loopedProgram();
void sleepAndReset(); 
void rst();
String runningTime();
//json.h
void loadConfig();
bool saveConfig();
void backupConfigToNVS(); // mirror /config.json into NVS so an FS-erasing flash keeps settings
void updateFieldsToNative();
void updateFieldsToString();
String exportConfig();
int importConfig(String jsonStr, String password); // 0=ok, 1=bad file, 2=wrong password
//auth (defined in json.h)
String createSession();
bool isAuthed();
void clearCurrentSession();
void saveSessions();
bool loadSetupDone();
void saveSetupDone(bool done);
//leds.h
void ledsBegin();
void ledsUpdate();
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
void connectToBestAP();
void wifiRoamCheck();
void apStart();
void httpConfig();
void httpData();
void httpDefault();
void httpExportConfig();
void httpImportConfig();
void httpStatus();
void httpLedConfig();
void httpLedConfigSet();
void httpLogin();
void httpLogout();
void httpSetup();
void httpfields();
void httpHome();
void httpRestart();
void httpIDs();
void httpSensors();
void httpServicesStart();
void httpTitles();
void httpWiFi();
void wifiStart();


// Minimum loop cycle time (ms) in hotspot/AP config mode. The captive portal
// loop used to spin every ~10ms which kept the CPU busy and made the board run
// hot; pacing it slower idles the CPU without hurting config responsiveness.
const int HOTSPOT_LOOP_TIME = 50;
// Guaranteed idle delay (ms) every loop, even when a cycle already overran its
// target, so the CPU is never pegged at 0ms sleep.
const int MIN_LOOP_DELAY = 5;

// WiFi mesh roaming: among access points sharing the configured SSID, connect to
// the one with the strongest signal, and re-check periodically. The device only
// jumps to another AP when it is at least ROAM_RSSI_THRESHOLD dB stronger than
// the current one (hysteresis to avoid flapping between similar APs).
const int ROAM_RSSI_THRESHOLD = 10;                  // dB
const unsigned long ROAM_CHECK_INTERVAL = 600000UL;  // 10 minutes (ms)

// Set the size of the JSON object
const int JSON_OBJECT_SIZE = 1536;

// Set the path to the JSON file
const char *JSON_FILE_PATH = "/config.json";


// All user-configurable settings live in one structure so the whole config can
// be loaded, saved, exported and imported as a single object. The config is
// loaded from JSON into this struct (see loadConfig/importConfig in WS_json.h).
struct WSConfig {
  // fields
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
  int refreshTime = REFRESH_TIME;
  String suggested_area = SUGGESTED_AREA;
  // Bluetooth HCI-over-TCP bridge (esp_bt controller exposed on a TCP port).
  // Off by default; uses significant RAM and needs a restart to (de)activate.
  bool bt_bridge_toggle = false;

  // sensor status-indicator LEDs (WS2812)
  bool led_toggle = LED_INDICATOR_TOGGLE;
  // ambient-light auto-brightness via LDR
  bool ldr_brightness_toggle = LDR_BRIGHTNESS_TOGGLE;
  // number of active LEDs (1..LED_MAX_COUNT), editable in the LED popup.
  int led_count = LED_COUNT;
  // which sensor each LED shows (value = sensor index 0..SENSOR_COUNT-1, -1 = off).
  // Default: LED i -> sensor i (then "none"). Arrays are sized to LED_MAX_COUNT;
  // entries past led_count are unused. Keep initialiser length == LED_MAX_COUNT.
  int led_map[LED_MAX_COUNT]    = {0,1,2,3,4,5,6,7,8,-1,-1,-1,-1,-1,-1,-1};
  // Per-LED colour scale: led_red/led_yellow/led_green are the three step VALUES
  // (step 1/2/3 thresholds). The live reading is interpolated between the step
  // colours below as it moves across these value points. Set in the LED popup.
  float led_red[LED_MAX_COUNT]    = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  float led_yellow[LED_MAX_COUNT] = {50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
  float led_green[LED_MAX_COUNT]  = {100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100};
  // Per-LED, per-step colours (packed 0xRRGGBB). Fully-saturated LED primaries so
  // they render vividly on WS2812s. Defaults: step1 red, step2 yellow, step3 green
  // (each is a colour picker in the popup).
  uint32_t led_col1[LED_MAX_COUNT] = {0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000,0xFF0000};
  uint32_t led_col2[LED_MAX_COUNT] = {0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00,0xFFFF00};
  uint32_t led_col3[LED_MAX_COUNT] = {0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00,0x00FF00};

  // LDR auto-brightness calibration (editable in the LED popup). Ambient = raw ADC
  // input range (min..max); Bright = LED output brightness range (0-255). A bright
  // ambient (low ADC) maps to ldr_bright_max; a dark ambient (high ADC) to ldr_bright_min.
  int ldr_ambient_min = 0;             // ADC reading at brightest ambient
  int ldr_ambient_max = LDR_ADC_MAX;   // ADC reading at darkest ambient
  int ldr_bright_min  = LED_BRIGHTNESS_MIN; // dimmest LED output (in the dark)
  int ldr_bright_max  = LED_BRIGHTNESS_MAX; // brightest LED output (in daylight)

  // Per-measurement calibration offset, added to each live reading. Indexed
  // identically to ledMeasures[] / led_map (0..SENSOR_MEASURE_COUNT-1). Editable in
  // the sensor-calibration popup. Default 0 = no correction.
  float sensor_offset[SENSOR_MEASURE_COUNT] = {0};

  // master password used to encrypt the sensitive fields at rest. Kept only in
  // NVS (ESP32) / a key file (ESP8266), never written into /config.json. Empty
  // is allowed: the config is still encrypted, just with an empty-derived key.
  String config_password = "";

  // sensor toggles
  bool AHT2x_toggle = AHT2X_TOGGLE;
  bool BMP280_toggle = BMP280_TOGGLE;
  bool BMP580_toggle = BMP580_TOGGLE;
  bool ENS160_toggle = ENS160_TOGGLE;
  bool PM1006K_toggle = PM1006K_TOGGLE;
  bool PMSx003_toggle = PMSX003_TOGGLE;
  bool SCD4x_toggle = SCD4X_TOGGLE;
  bool SGP30_toggle = SGP30_TOGGLE;
  bool SHT31_toggle = SHT31_TOGGLE;
  bool SPS30_toggle = SPS30_TOGGLE;

  // pins
  int sda_pin = SDA_PIN;
  int scl_pin = SCL_PIN;
  int PM1006K_RX_pin = PM1006K_RX_PIN;
  int PM1006K_TX_pin = PM1006K_TX_PIN;
  int PMSX003_RX_pin = PMSX003_RX_PIN;
  int PMSX003_TX_pin = PMSX003_TX_PIN;
  int RESET_CONFIG_pin = RESET_CONFIG_PIN;
  int HOTSPOT_pin = HOTSPOT_PIN;
  int FAN_pin = FAN_PIN;
  int LED_pin = LED_PIN;
  int LDR_pin = LDR_PIN;
};

// The single source of truth for the device configuration.
WSConfig config;

// Backwards-compatible aliases: existing code keeps using these names, but they
// now refer directly to the members of `config`. Because they are references,
// taking their address (used by the fields/toggles/pins pointer arrays below)
// yields the address of the matching struct member, so loads write into `config`.
String &wifi_ssid = config.wifi_ssid;
String &wifi_pass = config.wifi_pass;
String &mqtt_server = config.mqtt_server;
int &mqtt_port = config.mqtt_port;
String &mqtt_user = config.mqtt_user;
String &mqtt_password = config.mqtt_password;
String &mqtt_messageRoot = config.mqtt_messageRoot;
String &mdns_hostname = config.mdns_hostname;
String &hotspot_ssid = config.hotspot_ssid;
String &hotspot_pass = config.hotspot_pass;
bool &lowPowerMode_toggle = config.lowPowerMode_toggle;
int &refreshTime = config.refreshTime;
String &suggested_area = config.suggested_area;
bool &bt_bridge_toggle = config.bt_bridge_toggle;
bool &led_toggle = config.led_toggle;
bool &ldr_brightness_toggle = config.ldr_brightness_toggle;
String &config_password = config.config_password;

bool &AHT2x_toggle = config.AHT2x_toggle;
bool &BMP280_toggle = config.BMP280_toggle;
bool &BMP580_toggle = config.BMP580_toggle;
bool &ENS160_toggle = config.ENS160_toggle;
bool &PM1006K_toggle = config.PM1006K_toggle;
bool &PMSx003_toggle = config.PMSx003_toggle;
bool &SCD4x_toggle = config.SCD4x_toggle;
bool &SGP30_toggle = config.SGP30_toggle;
bool &SHT31_toggle = config.SHT31_toggle;
bool &SPS30_toggle = config.SPS30_toggle;

// UI mirror strings: the field pointer array holds String*, so the non-string
// config values get a textual mirror here (kept in sync by updateFieldsTo*).
String mqtt_port_str = String(mqtt_port);
String lowPowerMode_toggle_str = String(lowPowerMode_toggle);
String refreshTime_str = String(refreshTime);
String bt_bridge_toggle_str = String(bt_bridge_toggle);
String test = "1";

// NOTE: the LED toggles (led_toggle, ldr_brightness_toggle) and all other LED
// settings are NOT in this list — they live entirely in the LED popup and are
// persisted directly by buildConfigJson()/loadConfig().
#define FIELD_COUNT 15
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
    {"suggested_area", "Suggested area:", "text", "for example: Outside, Inside, Bedroom..."},
    {"bt_bridge_toggle", "Bluetooth HCI bridge (TCP, needs restart)", "checkbox", ""},
    {"config_password", "Config Password (encrypts stored secrets, leave blank for none):", "password", "password"}
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
    &suggested_area,
    &bt_bridge_toggle_str,
    &config_password
};

#define SENSOR_COUNT 10
String toggleIDName[SENSOR_COUNT][2] = {
    {"AHT2x_toggle", "AHT2x"},
    {"BMP280_toggle", "BMP280"},
    {"ENS160_toggle", "ENS160"},
    {"PM1006K_toggle", "PM1006K"},
    {"PMSx003_toggle", "PMSx003"},
    {"SCD4x_toggle", "SCD4x"},
    {"SGP30_toggle", "SGP30"},
    {"SHT31_toggle", "SHT31"},
    {"SPS30_toggle", "SPS30"},
    {"BMP580_toggle", "BMP580"}
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
    &SPS30_toggle,
    &BMP580_toggle
};

int &sda_pin = config.sda_pin;
int &scl_pin = config.scl_pin;
int &PM1006K_RX_pin = config.PM1006K_RX_pin;
int &PM1006K_TX_pin = config.PM1006K_TX_pin;//RX / GPIO3 TX / GPIO0
int &PMSX003_RX_pin = config.PMSX003_RX_pin;
int &PMSX003_TX_pin = config.PMSX003_TX_pin;
int &RESET_CONFIG_pin = config.RESET_CONFIG_pin;
int &HOTSPOT_pin = config.HOTSPOT_pin;
int &FAN_pin = config.FAN_pin;
int &LED_pin = config.LED_pin;
int &LDR_pin = config.LDR_pin;

#define PINS_COUNT 11
String pinIDName[PINS_COUNT][2] = {
    {"sda_pin", "SDA"},
    {"scl_pin", "SCL"},
    {"PM1006K_RX_pin", "PM1006K RX"},
    {"PM1006K_TX_pin", "PM1006K TX"},
    {"PMSX003_RX_pin", "PMSX003 RX"},
    {"PMSX003_TX_pin", "PMSX003 TX"},
    {"RESET_CONFIG_pin", "RESET CONFIG"},
    {"HOTSPOT_pin", "HOTSPOT"},
    {"FAN_pin", "FAN"},
    {"LED_pin", "Status LEDs (WS2812)"},
    {"LDR_pin", "LDR (auto brightness)"}
};
int* pins[PINS_COUNT] = {
    &sda_pin,
    &scl_pin,
    &PM1006K_RX_pin,
    &PM1006K_TX_pin,
    &PMSX003_RX_pin,
    &PMSX003_TX_pin,
    &RESET_CONFIG_pin,
    &HOTSPOT_pin,
    &FAN_pin,
    &LED_pin,
    &LDR_pin
};






bool wifiConnectionType; //0 hotspot, 1 wifi
bool reconfigure=0;
bool runningTasks=1;
bool sensorStart=1;
unsigned long lastRoamCheck=0; //millis of the last mesh roaming check

// Web-UI authentication state. The login password is the same as the config
// encryption password (config_password). When it is empty the site is open.
// Sessions are random tokens handed out on login and checked via a cookie;
// they live only in RAM, so a reboot logs everyone out.
#define MAX_SESSIONS 4
String sessions[MAX_SESSIONS];
int sessionWriteIdx = 0;
bool setupDone = false; //becomes true once first-run setup (password/import) is done

//taskid_t AHT2x_task, BMP280_task, ENS160_task, PM1006K_task, PMSx003_task, SCD4x_task, SGP30_task, SHT31_task, SPS30_task;

float aht2x_temp=nan(""), aht2x_hum=nan("");
float bmp280_temp=nan(""), bmp280_press=nan(""), bmp280_alt=nan("");
float bmp580_temp=nan(""), bmp580_press=nan(""), bmp580_alt=nan("");
float ens160_eco2=nan(""), ens160_tvoc=nan(""), ens160_aqi=nan("");
float pm1006k_pm1_0 = nan(""), pm1006k_pm2_5 = nan(""), pm1006k_pm10_0 = nan("");
float pmsx003_pm1_0 = nan(""), pmsx003_pm2_5 = nan(""), pmsx003_pm10_0 = nan(""), pmsx003_temp = nan(""), pmsx003_hum = nan(""), pmsx003_hcho = nan("");
float scd4x_co2=nan(""), scd4x_temp=nan(""), scd4x_hum=nan("");
float sgp30_tvoc=nan(""), sgp30_co2=nan(""), sgp30_eth=nan(""), sgp30_h2=nan("");
float sht31_temp=nan(""), sht31_hum=nan("");
float sps30_pm1_0 = nan(""), sps30_pm2_5 = nan(""), sps30_pm10_0 = nan("");

// Home Assistant discovery unique_id version tag. Bump this (e.g. "v2" -> "v3") to
// force HA to register the sensors as brand-new entities, dropping any stale
// entity_id it kept in its registry keyed by the old unique_id.
#define DISCOVERY_UID_VER "v2"

// Known plaintext stored (encrypted with the config password) in every export as
// "pw_check". On import it is decrypted with the supplied password; a mismatch means
// the wrong password was given, so the import is rejected before anything is applied.
#define CONFIG_PW_CHECK "ws-config-ok"

// Board key used for OTA asset matching and the firmware-maker output folders.
// Must equal the PlatformIO env name so "<FW_BOARD>-firmware.bin" / "-littlefs.bin"
// line up with the GitHub-release assets (see WS_ota.h and firmware maker/).
#if defined(SEEED_XIAO_ESP32C3)
  #define FW_BOARD "seeed_xiao_esp32c3"
#elif defined(SEEED_XIAO_ESP32C6)
  #define FW_BOARD "seeed_xiao_esp32c6"
#elif defined(SEEED_XIAO_ESP32S3)
  #define FW_BOARD "seeed_xiao_esp32s3"
#elif defined(LOLIN_S2_MINI)
  #define FW_BOARD "lolin_s2_mini"
#elif defined(ESP32DOIT_DEVKIT_V1)
  #define FW_BOARD "esp32doit_devkit_v1"
#elif defined(AZ_DELIVERY_DEVKIT_V4)
  #define FW_BOARD "az_delivery_devkit_v4"
#elif defined(D1_MINI)
  #define FW_BOARD "d1_mini"
#else
  #define FW_BOARD "unknown"
#endif

//TODO update sw/hw version
String SWversion="3.4.2"; //software version bump this when you change the firmware, so the user can see if they are up to date
String HWversion="0.1"; //hardware version
String Model="Weatherstation";
String ModelID=device; //ESP32, SEEED_XIAO_ESP32C3... 
String Manufacturer="@Nikpesu";
#if defined(ESP8266)
  String UniqueDeviceID=String(WiFi.macAddress());
#elif defined(ESP32)
  String UniqueDeviceID=String(ESP.getEfuseMac());
#endif
String infoString="log available after about 10s";


// Define the JSON document size
// StaticJsonDocument<1024> doc;
JsonDocument doc;

const IPAddress apIp(192, 168, 0, 1);
DNSServer dnsServer;

#if defined(ESP8266)
    ESP8266WebServer server(80);
#elif defined(ESP32)
    WebServer server(80);
#endif
WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_SGP30 sgp30;
Adafruit_BMP280 bmp280;
BMP581 bmp580;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
SparkFun_ENS160 myENS; 
AHTxx aht2x(AHTXX_ADDRESS_X38, AHT2x_SENSOR);
SCD4x scd4x;
SensirionI2cSps30 sps30;

#if defined(ESP8266)
  SoftwareSerial pm1006Serial(PM1006K_TX_pin, PM1006K_RX_pin); 
#endif 
PM1006K * pm1006k;


PMS pmsx003Type=PMS5003;
SerialPM pmsx003(pmsx003Type, PMSX003_RX_pin, PMSX003_TX_pin); // PMSx003, RX, TX

// Status-indicator LED strip. The data pin is applied at runtime in ledsBegin()
// from the (user-configurable) config.LED_pin, so the compile-time LED_PIN here
// is only a placeholder default. Allocated for the maximum count; the active
// length is set to config.led_count in ledsBegin().
Adafruit_NeoPixel leds(LED_MAX_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


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