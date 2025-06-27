# WeatherStation, ESP32/ESP8266 MQTT Integration for Home Assistant

This project is designed for **ESP32** and **ESP8266** microcontrollers. It uses **MQTT** to send data to **Home Assistant** via the MQTT integration.

### Requirements

* An MQTT broker/server (e.g. Mosquitto)
* A running Home Assistant instance with MQTT integration configured
* Both your device and Home Assistant must connect to the **same MQTT server**
* The MQTT discovery prefix in Home Assistant must be set to `homeassistant` (this is the default)

---

## 🚀 Setup Instructions

### 1. Install Required Tools

* Download and install [Visual Studio Code](https://code.visualstudio.com/)
* Install the [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode) from the [Visual Studio Marketplace](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)

### 2. Open the Project in PlatformIO

![Open Project](https://user-images.githubusercontent.com/92652074/227805914-9f61558e-7341-4283-bba1-01baa1d0d283.png)

### 3. Select Your Board

Choose the appropriate board for your microcontroller:

![Select Board](https://user-images.githubusercontent.com/92652074/227806081-7891bc30-c31b-41e3-9e3c-0b7a8aa0ceae.png)

### 4. Configure Settings (Optional)

Customize parameters using either `secrets.h` or `config.h`.

### 5. Upload the Firmware

Click the upload button:

![Upload Sketch](https://github.com/user-attachments/assets/c09eb2cc-f087-4720-b816-2e035ae29a85)

### 6. Upload Data Files

Upload the SPIFFS or LittleFS data:

![Upload Data](https://github.com/user-attachments/assets/0d0c9a81-bb19-4a51-bebc-14dab6b0c8b8)

### 7. Connect to Wi-Fi (If Not Preconfigured)

If Wi-Fi credentials were not set beforehand, connect to the device’s hotspot:

* **SSID:** `ws1`
* **Password:** `12345678`

### 8. Done! 🎉

Once connected and configured, your device should automatically appear in **Home Assistant** via MQTT Discovery.
You can find your instance on MDNS_hostname.local (default is `ws1.local`) 

---


