
## Setup

1) Install <a href="https://code.visualstudio.com/">Visual Studio Code</a> 

2) Install <a href="https://platformio.org/install/ide?install=vscode">PlatformIO<a/> extension from <a href="https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide">Extensions</a> (CTRL + SHIFT + X)

3) Open project with Platform IO.
![image](https://user-images.githubusercontent.com/92652074/227805914-9f61558e-7341-4283-bba1-01baa1d0d283.png)

4) Then select board
![image](https://user-images.githubusercontent.com/92652074/227806081-7891bc30-c31b-41e3-9e3c-0b7a8aa0ceae.png)

5) Then set parameters (in ```esp32/main_esp32.h or esp8266/main_esp8266.h```): ```SDA``` and ```SCL``` port (```default => ESP8266  4, 5 or ESP32 33, 35```) and ```pin``` for button (```default => 12```)
![image](https://user-images.githubusercontent.com/92652074/227806317-3180fef1-5d0f-4acd-a1d8-52aff0d38488.png)

6) Compile and upload.
![image](https://user-images.githubusercontent.com/92652074/227806434-7f347533-40c4-4e5e-92a7-02da082f8ce5.png)

7) Upload data.
![image](https://user-images.githubusercontent.com/92652074/227806562-6ec9c297-f2da-4a4b-8441-a30f86b7e0bb.png)

8) Hold the button (```pin 9``` default) and press restart for hotspot to open (default ip should be ```192.168.4.1``` or just ```see serial for ip/hostname```)

9) Input data to ESP and restart ESP if it doesn't by itself
![image](https://user-images.githubusercontent.com/92652074/227806872-59262a0b-603e-4a0c-8dac-6966c2ac84b8.png)

10) Enjoy 🥰

  
## OTA update

1) set in platformio.ini file for selected board ```upload_port``` (example```upload_port=ws1.local```)

2) upload... (if it doesn't do it first time try again)

## Board scheme v1

![Schematic_weather V2_2022-08-01](https://user-images.githubusercontent.com/92652074/182050715-7694a899-4b08-4b32-82c2-49ca656223d8.png)
