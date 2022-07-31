
## Installation

1) Upload Sketch

2) Connect to AP (Weatherstation_xxx)

3) Set parameters (or you could have set them in sketch, But you cant set wifi password)

```C++
//Set parameters here if you want
char mqtt_server[40]="";
char mqtt_port[10] = "";
char mqtt_user[40]="";
char mqtt_password[40] = "";
char sleep_time[10] = "";
```
## Board scheme



## Adding Board

1.1) Preferences 
1.2) Settings 
1.3) Additional Boards Manager URLs 
1.4) https://arduino.esp8266.com/stable/package_esp8266com_index.json

2.1) Tools 
2.2) Board 
2.3) Boards Manager...
2.4) esp8266 (ESP8266 Community) 
2.5) Install

3.1) Tools 
3.2) Board
3.3) esp8266 boards (x.x.x) 
3.4) LOLIN(WEMOS) D1 mini (clone)

4.1) Tools 
4.2) Board
4.3) Port


