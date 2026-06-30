# ESP32/ESP8266 MQTT WeatherStation for Home Assistant

This project is designed for **ESP32** and **ESP8266** microcontrollers. It uses **MQTT** to send data to **Home Assistant** via the MQTT integration.

## 📚 Table of Contents

- [Overview](#esp32esp8266-mqtt-weatherstation-for-home-assistant)
- [✅ Supported Boards](#-supported-boards)
- [🚀 Setup Instructions](#-setup-instructions)
  - [1. Install Required Tools](#1-install-required-tools)
  - [2. Open the Project in PlatformIO](#2-open-the-project-in-platformio)
  - [3. Select Your Board](#3-select-your-board)
  - [4. Configure Settings (Optional)](#4-configure-settings-optional)
  - [5. Upload the Firmware](#5-upload-the-firmware)
  - [6. Upload Data Files](#6-upload-data-files)
  - [7. Connect to Wi-Fi (If Not Preconfigured)](#7-connect-to-wi-fi-if-not-preconfigured)
  - [8. Done! 🎉](#8-done-)
- [📶 Mesh Wi-Fi roaming](#-mesh-wi-fi-roaming)
- [🚦 Sensor status LEDs](#-sensor-status-leds)
- [💾 Backup & Restore Configuration](#-backup--restore-configuration)
  - [🔒 Encrypted sensitive fields](#-encrypted-sensitive-fields)
  - [🔐 Web login & first-run setup](#-web-login--first-run-setup)
- [📄 License](#-license)

---

### Requirements

- An MQTT broker/server (e.g. Mosquitto)
- A running Home Assistant instance with MQTT integration configured
- Both your device and Home Assistant must connect to the **same MQTT server**
- The MQTT discovery prefix in Home Assistant must be set to `homeassistant` (this is the default)

---

## ✅ Supported Boards

This release is compatible with the following boards via **PlatformIO**:

### **ESP32 Boards**

- AZ-Delivery Devkit V4
- ESP32 DOIT Devkit V1
- Lolin S2 Mini
- Seeed XIAO ESP32-C3
- Seeed XIAO ESP32-C6
- Seeed XIAO ESP32-S3

### **ESP8266 Boards**

- Wemos D1 Mini

### **Any ESP8266/ESP32 board**

- Just need to make custom board in `platformio.ini`

Each board has both **USB upload** and **OTA update** environments available in the PlatformIO configuration.

---

## 🚀 Setup Instructions

### 1. Install Required Tools

- Download and install [Visual Studio Code](https://code.visualstudio.com/)
- Install the [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode) from the [Visual Studio Marketplace](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)

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

If you haven't set the Wi-Fi credentials in advance, the device will start in Access Point (AP) mode. To configure it:

1. Connect to the device's hotspot:
   - **SSID:** `ws1`
   - **Password:** `12345678`

2. After connecting, you should be automatically redirected to the configuration homepage.
   If not, open a browser and navigate to: `http://192.168.4.1`

### 8. Done! 🎉

Once connected and configured, your device should automatically appear in **Home Assistant** via MQTT Discovery.  
You can find your instance on `MDNS_hostname.local` (default is `ws1.local`).

---

## 📶 Mesh Wi-Fi roaming

If several access points broadcast the **same SSID** (a typical mesh / multi-AP
setup), the device connects to the **strongest** one. Every 10 minutes it
re-scans and, if another AP with the same SSID is **more than 10 dB stronger**
than the current one, it hops to it. The 10 dB hysteresis prevents constant
flapping between APs of similar strength. Thresholds are tunable in
[`src/WS_config.h`](src/WS_config.h) (`ROAM_RSSI_THRESHOLD`, `ROAM_CHECK_INTERVAL`).

> In hotspot/config mode the loop is paced (`HOTSPOT_LOOP_TIME`) so the board
> idles instead of spinning, which keeps it from running hot.

---

## 🚦 Sensor status LEDs

An optional **WS2812 / NeoPixel** strip can show, at a glance, the live state of
the sensors. A single data wire drives the LEDs; each LED is assigned to a sensor
and **coloured by that sensor's live reading** on a red → yellow → green scale you
configure. An LED is **off** when it is unmapped, its sensor is disabled, or there
is no data (NaN).

**All LED settings live in one popup** — open it with the **🚦 Sensor status
LEDs** button on the config page. Inside the popup:

- **Sensor status LEDs (WS2812 on LED pin)** — master on/off. It is **off by
  default**; while off the data pin is left untouched so a sensor can use it.
- **Auto LED brightness from LDR** — dim/brighten the strip with ambient light
  (see below). When off, a fixed brightness is used.
- **Number of LEDs** — configurable, **default 5** (up to `LED_MAX_COUNT`, 16).
- **Per-LED sensor + part mapping** — two dropdowns on each LED row: first the
  **sensor**, then the **part / measurement** of that sensor (e.g. SHT31 →
  Temperature or Humidity; SCD4x → CO₂, Temperature or Humidity; PMSx003 → PM1.0,
  PM2.5, PM10, Temperature, Humidity or HCHO). Only sensors currently **enabled**
  under *Sensors* are listed; disabling a sensor drops its LED to *— none —* and
  turns it off. Picking a part **auto-fills sensible default step values and
  colours** for that measurement type (e.g. CO₂ 400/1000/1500, PM low=green,
  temperature cold→warm→hot).
- **Per-LED colours** — each LED has **3 steps**; for each step set a **value**
  and a **colour** (colour picker, default red/yellow/green). The live reading is
  interpolated between the step colours across the value points (values past
  either end clamp), so the scale works ascending (e.g. temperature) or descending
  (e.g. CO₂, where a high value is red). Each row also shows the LED's current
  reading.

Saving the popup applies everything **live** (no device restart). The data pin
itself is the **Status LEDs (WS2812)** pin in the main *Pins* section (default
**D8** on the XIAO boards).

### Auto-brightness (LDR)

The **Auto LED brightness from LDR** toggle (in the popup) dims/brightens the
strip with ambient light, read from an **LDR** on the **LDR pin** (default
**D0**). The LDR forms a divider with the on-board 10 kΩ pull-up to 3.3 V (LDR to
GND): brighter surroundings → brighter LEDs, darkness → dim, so they don't glare
at night.

> ⚠️ **Pin conflicts:** the default LED pin `D8` is also the default **PM1006K /
> PMSx003 RX** pin — a WS2812 (RMT output) and a particulate-sensor UART **cannot
> share a GPIO**. The default LDR pin `D0` is also `RESET_CONFIG_PIN` on the XIAO
> boards, where a dark LDR could read LOW at boot and trigger a config reset. Move
> the conflicting pin in the pin settings if you use both.

Defaults (`LED_PIN`, `LED_COUNT`, `LED_MAX_COUNT`, `LED_BRIGHTNESS`, `LDR_PIN`,
`LED_BRIGHTNESS_MIN`/`MAX`) live in [`src/WS_config.h`](src/WS_config.h).

> 💡 **Pin entry:** every pin field accepts either a raw **GPIO number** (e.g.
> `19`) or the board's **`Dx` label** (e.g. `D8`); the device resolves `Dx` to the
> right GPIO for the selected board.

---

## 💾 Backup & Restore Configuration

The whole device configuration (Wi-Fi, MQTT, sensor toggles, pins and other
settings) is stored on the device as a single JSON file (`/config.json`) and is
loaded into one in-memory `Config` structure. You can back it up and restore it
straight from the web interface:

Both live in the **Current Config** window (opened from the main page):

- **⬇ Export Config** — downloads the current configuration as
  `weatherstation_config.json`. Sensitive fields stay encrypted (see below), so
  the file is safe to keep as a backup.
- **⬆ Import Config** — upload a previously exported JSON file. You're asked for
  the password the file was exported with (leave it blank if it had none); the
  device decrypts, applies and saves it, then restarts.

This makes it easy to move a configuration between devices or restore it after a
firmware flash that erased the filesystem.

The same actions are also available over HTTP for automation:

| Endpoint | Method | Description |
| --- | --- | --- |
| `/exportConfig` | `GET` | Returns the current config JSON as a download |
| `/importConfig` | `POST` | Body `{ "config": "<json>", "password": "<may be blank>" }`; applies it, then restarts |

### 🔒 Encrypted sensitive fields

The sensitive fields — **WiFi password, MQTT username, MQTT password and hotspot
password** — are never stored in plain text. They are encrypted with
**AES‑256‑CTR** in `/config.json`, both on the device and inside exported
backups. The rest of the config stays human‑readable.

- Set a **Config Password** in the configuration form to choose the encryption
  key. Leaving it blank is allowed: the fields are still encrypted, just with an
  empty‑password key (so they're never readable as plain text).
- The password itself is **never written to `/config.json`**. It is kept in
  **NVS on ESP32** and in a small key file (`/cfg.key`) on **ESP8266**, so the
  device can decrypt and boot on its own.
- On ESP32 the password survives a firmware flash that erases the filesystem, so
  an encrypted backup made earlier can still be re‑imported afterwards. On
  ESP8266 the key file lives on the filesystem, so note your password before
  re‑flashing the data partition.

> **Threat model:** this protects the config file from being read directly
> (e.g. via the web UI or a copy of `/config.json`). Because the device must
> decrypt automatically at boot, the key is stored on the device, so it does
> **not** protect against an attacker who can dump the full flash.

### 🔐 Web login & first-run setup

The same password also gates the **whole web interface**:

- **First run:** the device shows a one-time setup screen where you either
  **choose a password** (it can be left blank) or **upload a configuration** to
  restore. Uploading asks for that file's password.
- **After that:** if a password is set, the site shows a themed **login** screen
  and every data endpoint requires a valid session (a cookie token issued on
  login). If the password is blank, the site stays open.
- Sessions live only in RAM, so a reboot logs everyone out. Use the **Log out**
  button to end a session manually.
- **Forgot the password?** Hold the **config-reset pin** (`RESET_CONFIG_PIN`)
  during boot. This wipes the password and setup flag and restores the default
  config, so you can never be permanently locked out.

Auth endpoints: `/status` (what screen to show), `/login`, `/logout`, `/setup`.

---

## 📄 License

This project is licensed under the **GNU General Public License v3.0**.

You are free to:

- **Use** this software for any purpose
- **Distribute** it freely
- **Modify** the source code
- **Share** your modified versions

Under the following conditions:

- **Attribution**: You must give appropriate credit to the original author.
- **Share Alike**: Any modifications must also be released under the **same GPL v3 license**.
- **License Notice**: You must include a copy of the license in any distribution.

Read the full license text here: [GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.html)

© Nikpesu, 2025
