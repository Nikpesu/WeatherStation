#if defined(ESP8266) || defined(ESP32)
  #include "./WS_config.h"
  #include "./WS_mqtt.h"
  #include "./WS_sensors.h"
  #include "./WS_network.h"
  #include "./WS_json.h"
  #include "./WS_main.h"
#else
#error "This ain't a ESP8266 or ESP32!"
#endif


void setup()
{
  Serial.begin(115200);
  //while(!Serial.available()) delay(10);
  Serial.println("\n-------WEATHERSTATION-------");
  startProgram();
}

void loop()
{
  loopedProgram();
}