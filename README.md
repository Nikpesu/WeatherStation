
## Installation

1) Upload Sketch

2) Connect to AP (Weatherstation_xxx)

3) Set parameters (or you could have set them in sketch, But you cant set wifi password)

```C++
{
  //Set parameters here if you want
  char mqtt_server[40]="";
  char mqtt_port[10] = "";
  char mqtt_user[40]="";
  char mqtt_password[40] = "";
  char sleep_time[10] = "";
}
```
## Board scheme



## Adding Board

* Preferences  
	* Settings 
		* Additional Boards Manager URLs
			* https://arduino.esp8266.com/stable/package_esp8266com_index.json

* Tools 
	* Board
		* Boards Manager...
			* esp8266 (ESP8266 Community)
				* Install

* Tools 
	* Board
		* esp8266 boards (x.x.x) 
			* LOLIN(WEMOS) D1 mini (clone)

* Tools 
	* Board
		* Port


