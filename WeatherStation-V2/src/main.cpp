#if defined(ESP8266)
  #include "esp8266/config_esp8266.h"
  #include "esp8266/mqtt_esp8266.h"
  #include "esp8266/sensors_esp8266.h"
  #include "esp8266/wifi_esp8266.h"
  #include "esp8266/json_esp8266.h"
  #include "esp8266/main_esp8266.h"
  #include "esp8266/new_json.h"
#elif defined(ESP32)
#pragma message "ESP32 stuff happening!"
  #include "esp32/config_esp32.h"
  #include "esp32/mqtt_esp32.h"
  #include "esp32/sensors_esp32.h"
  #include "esp32/wifi_esp32.h"
  #include "esp32/json_esp32.h"
  #include "esp32/main_esp32.h"
#else
#error "This ain't a ESP8266 or ESP32, dumbo!"
#endif
void setup()
{
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);
  //while(!Serial.available()) delay(10);
  Serial.println("\n-------WEATHERSTATION-------");
  startProgram();
}

void loop()
{
  loopedProgram();
}