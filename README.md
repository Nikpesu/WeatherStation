
## Installation

0) Set parameters (not needed as wifi ap will open)

```C++
//Set parameters here if you want
char mqtt_server[40]="";
char mqtt_port[10] = "";
char mqtt_user[40]="";
char mqtt_password[40] = "";
char sleep_time[10] = "";
```

1) Upload Sketch

2) Connect to AP (Weatherstation_xxx)

3) Set parameters


## Board scheme



## Adding Board to arduino IDE

1) Adding board repository <br/>
&emsp;➥ Preferences <br/>
&emsp;&emsp;➥	Settings <br/> 
&emsp;&emsp;&emsp;➥	Additional Boards Manager URLs <br/>
&emsp;&emsp;&emsp;&emsp;➥	https://arduino.esp8266.com/stable/package_esp8266com_index.json <br/>


2) Installing board <br/>
&emsp;➥ Tools <br/>
&emsp;&emsp;➥ Board <br/>
&emsp;&emsp;&emsp;➥ Boards Manager...<br/>
&emsp;&emsp;&emsp;&emsp;➥ ESP8266 (ESP8266 Community) <br/>
&emsp;&emsp;&emsp;&emsp;&emsp;➥ Install<br/>

3) Setting board <br/>
&emsp;➥ Tools<br/>
&emsp;&emsp;➥ Board <br/>
&emsp;&emsp;&emsp;➥ ESP8266 boards (x.x.x)  <br/>
&emsp;&emsp;&emsp;&emsp;➥ LOLIN(WEMOS) D1 mini (clone) <br/>

4) Setting port <br/>
&emsp;➥ Tools<br/>
&emsp;&emsp;➥  Board<br/>
&emsp;&emsp;&emsp;➥  Port


