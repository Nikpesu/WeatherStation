#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include <ESPAsyncTCP.h>
#include "config.h"


DNSServer dnsServer;
AsyncWebServer server(80);

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}
  void handleRequest(AsyncWebServerRequest *request)
  {
      File file = LittleFS.open("/index.html", "r");
      String data = file.readString();
      file.close();
      request->send_P(200, "text/html", data.c_str());
      Serial.println(data.c_str());
      Serial.println("Client Connected");   
      file.close();
  }
};
void setupServer()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    { 
      File file = LittleFS.open("/index.html", "r");
      String data = file.readString();
      file.close();
      //request->send_P(200, "text/html", data.c_str());
      request->send(200, "text/html", data.c_str());
      //Serial.println(data.c_str());
      Serial.print("Served / for: ");
      Serial.println(request->client()->remoteIP());   
      file.close();
    }
  );
  server.on("/POST", HTTP_GET, [](AsyncWebServerRequest *request)
    { 
      Serial.println(request->params());
      Serial.print("Served / for: ");
    }
  );

  Serial.println("-- asyncwebserver setup done!");
}

void hotspotStart()
{
  Serial.println("-----HOTSPOT----");
  WiFi.softAP("esp-captive");
  Serial.println(WiFi.softAPIP());
  //captive portal & dns start
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
}