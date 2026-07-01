<h1 align="center">🌦️ WeatherStation</h1>

<p align="center">
  <b>A self-hosted air-quality & weather station for ESP32 / ESP8266,<br>
  with a full web UI, encrypted config, status LEDs and native Home Assistant integration over MQTT.</b>
</p>

<p align="center">
  <b>🌐 <a href="https://nikpesu.github.io/WeatherStation/">Install it from your browser →</a></b>
</p>

<p align="center">
  <a href="https://github.com/Nikpesu/WeatherStation">github.com/Nikpesu/WeatherStation</a> ·
  <a href="https://github.com/Nikpesu/WeatherStation/releases">Releases</a> ·
  <a href="https://github.com/Nikpesu/WeatherStation/actions">CI</a>
</p>

<p align="center">
  <img alt="platform" src="https://img.shields.io/badge/platform-ESP32%20%7C%20ESP8266-blue">
  <img alt="framework" src="https://img.shields.io/badge/framework-Arduino%20%2F%20PlatformIO-orange">
  <img alt="home assistant" src="https://img.shields.io/badge/Home%20Assistant-MQTT%20discovery-41BDF5">
  <img alt="license" src="https://img.shields.io/badge/license-GPL--3.0-green">
</p>

---

## 👋 About this project

I built the **WeatherStation** myself back in **2022** — a hobby project I coded from scratch to
read a few sensors on an ESP and push the values into Home Assistant. The original was a single
`WeatherStation.ino` sketch.

In **2026** I came back to it and rebuilt it into what it is now: a modular, multi-board firmware
with a themed single-page web interface, a proper config/auth system, encrypted secrets, addressable
status LEDs, per-sensor calibration, over-the-air updates straight from GitHub Releases, and CI that
publishes a new build on every commit. Same idea, a lot more device.

Everything here — firmware, web UI and tooling — is written and maintained by me,
[**@Nikpesu**](https://github.com/Nikpesu). See the [changelog](#-changelog) for how it got here.

---

## ✨ Features

- 🌡️ **Up to 10 air-quality / climate sensors** (I²C + UART) — temperature, humidity, pressure,
  CO₂, VOC, and particulate matter — each independently toggled from the web UI.
- 🏠 **Home Assistant MQTT auto-discovery** — sensors appear automatically, no YAML.
- 🖥️ **Single-page web UI** served from the device (LittleFS) with **4 themes**
  (dark / light / terminal / amber), a **live readings panel**, and live logs.
- 🎚️ **Per-measurement calibration** — add a live offset to any sensor value; it flows through
  both the UI and the MQTT payload.
- 🚦 **WS2812 status LEDs** — one LED per sensor, coloured on a configurable value scale, with
  optional **LDR auto-brightness**.
- 🔐 **Web login + first-run setup + password change**, with **AES-256-CTR encrypted** secrets
  and **encrypted config backup / restore**.
- 📶 **Mesh Wi-Fi roaming** (jumps to the strongest AP on the same SSID) + **captive-portal**
  hotspot setup mode.
- ⬆️ **Firmware updates without a cable** — upload a `.bin` from the browser, or **OTA from
  GitHub Releases** — and **your settings are preserved** across updates.
- 🛠️ **Multi-board builds & CI** — one script builds every board; a GitHub Action publishes a
  release on every push.
- 🔋 **Low-power (deep-sleep) mode** and an optional **Bluetooth HCI-over-TCP bridge**.

---

## 🔩 Supported hardware

### Boards (PlatformIO environments)

| Board | Chip | Notes |
|-------|------|-------|
| Seeed XIAO ESP32-C3 | ESP32-C3 | ✅ recommended, lots of flash headroom |
| Seeed XIAO ESP32-C6 | ESP32-C6 | ✅ works (flash is tight — see [notes](#-flash-usage)) |
| Seeed XIAO ESP32-S3 | ESP32-S3 | ✅ 8 MB flash, huge headroom |
| Lolin S2 Mini | ESP32-S2 | ✅ Wi-Fi only (no Bluetooth) |
| Wemos D1 Mini | ESP8266 | ✅ lightest build |
| ESP32 DOIT Devkit V1 | ESP32 | ⚠️ needs a pin-label tweak to compile |
| AZ-Delivery Devkit V4 | ESP32 | ⚠️ needs a pin-label tweak to compile |

> **Any** ESP32/ESP8266 board works — just add an environment in
> [`platformio.ini`](platformio.ini). Each board has both a **USB** and an **OTA** (`espota`)
> upload environment.

### Sensors

| Sensor | Bus | Measures |
|--------|-----|----------|
| SHT31 | I²C | temperature, humidity |
| AHT2x | I²C | temperature, humidity |
| BMP280 | I²C | temperature, pressure, altitude |
| BMP580 (BMP581) | I²C | temperature, pressure, altitude |
| SCD4x | I²C | CO₂, temperature, humidity |
| SGP30 | I²C | TVOC, eCO₂, ethanol, H₂ |
| ENS160 | I²C | eCO₂, TVOC, AQI |
| SPS30 | I²C | PM1.0, PM2.5, PM10 |
| PMSx003 | UART | PM1.0, PM2.5, PM10, temperature, humidity, HCHO |
| PM1006K | UART | PM1.0, PM2.5, PM10 |

Every pin is configurable from the web UI and accepts either a raw **GPIO number** (`19`) or the
board's silkscreen **`Dx` label** (`D8`).

---

## 🚀 Quick start

### Requirements

- An **MQTT broker** (e.g. Mosquitto) and a **Home Assistant** instance with the MQTT integration
  (discovery prefix `homeassistant`, the default). Both must reach the same broker.

### Option A — the web installer (easiest, no toolchain) 🌐

Open **[nikpesu.github.io/WeatherStation](https://nikpesu.github.io/WeatherStation/)** in **Chrome or
Edge on desktop**, plug the ESP into USB, and click **Install** — the right build for your chip is
flashed automatically (firmware **and** web UI). The same page also has a **serial log/debug** console
and a shortcut to open a device that's already on your network. Powered by
[ESP Web Tools](https://esphome.github.io/esp-web-tools/) + Web Serial.

### Option B — flash a prebuilt image manually

1. Grab your board's `*-firmware.bin` and `*-littlefs.bin` from the
   [**Releases**](https://github.com/Nikpesu/WeatherStation/releases) page (or build them with the
   [`firmware maker`](firmware%20maker/) scripts).
2. First-time flash over USB with `esptool`, or PlatformIO (`-t upload` then `-t uploadfs`).
   See [`firmware maker/README.md`](firmware%20maker/README.md) for exact commands.
3. After the first flash, **all future updates can be done over Wi-Fi** from the web UI — no cable.

### Option C — build from source (PlatformIO)

1. Install [VS Code](https://code.visualstudio.com/) + the
   [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode).
2. Open this folder. (Optional) copy `secrets.h.example` → `secrets.h` and preload your Wi-Fi/MQTT
   defaults; otherwise configure everything later in the web UI.
3. Pick your environment and upload firmware + filesystem:
   ```bash
   pio run -e seeed_xiao_esp32c3 -t upload      # program
   pio run -e seeed_xiao_esp32c3 -t uploadfs    # web UI (data/)
   ```
   For OTA to a device already on the network, use the `_ota` environment
   (e.g. `seeed_xiao_esp32c3_ota`), which uploads to `wsnikosoba.local` via `espota`.

### First connection

If no Wi-Fi is configured, the device boots into **Access Point** mode:

- **SSID:** `ws1` · **Password:** `12345678`

Connect and you'll be redirected to the setup page (or open `http://192.168.4.1`). Set Wi-Fi + MQTT,
and the device appears in **Home Assistant** automatically. On your network it lives at
`http://<hostname>.local` (default `ws1.local`).

---

## ⬆️ Firmware updates (OTA)

Open the device page → **⬆ Firmware Update**. Three ways to update, and **your settings are kept**
every time:

1. **From GitHub** — *Check GitHub for updates* pulls the latest
   [release](https://github.com/Nikpesu/WeatherStation/releases), compares it to the running version,
   and installs the asset matching your board.
2. **Manual upload** — drop a `firmware.bin` (program) and/or `littlefs.bin` (web UI) straight into
   the browser.
3. **`espota` / PlatformIO** — the classic `-t upload` / `-t uploadfs` over Wi-Fi via the `_ota`
   environments.

**How settings survive an update:** a firmware-only flash never touches the filesystem, so
`/config.json` is untouched. A filesystem flash *would* wipe it, so the device first mirrors the
config into NVS and restores it on the next boot (ESP32).

> Bump `SWversion` in [`src/WS_config.h`](src/WS_config.h) before committing to make a release show
> up as an **available update** on already-deployed devices. On every push to `main`, a
> [GitHub Action](.github/workflows/build-release.yml) builds all boards and publishes the release
> automatically.

### 🧮 Flash usage

The dual-OTA [partition table](my_partitions.csv) splits a 4 MB chip into two 1.73 MB program slots.
The C6 is the tightest fit (~95%) because its newer SDK build is larger; the C3, S2, S3 and ESP8266
all have comfortable headroom. The biggest single optional cost is the Bluetooth stack (~360 KB),
which is only pulled in by the (default-off) HCI bridge.

---

## 🎚️ Sensor calibration & live readings

The main page shows a **live readings** panel (refreshed every few seconds) for every enabled sensor.
Open **🎚 Sensor calibration** to add a **per-measurement offset** — watch the live value update as
you tune. The offset is applied on the device, so it flows through **both** the UI and the value
reported to Home Assistant. Set `0` for no correction.

---

## 🚦 Sensor status LEDs

An optional **WS2812 / NeoPixel** strip shows the live state of the sensors at a glance — each LED is
assigned to a sensor and **coloured by that sensor's live reading** on a scale you configure.

**All LED settings live in one popup** — open it with **🚦 Sensor status LEDs**:

- **Master on/off** (off by default; the data pin is left free while off).
- **Auto brightness from LDR** (ambient-light dimming), or a fixed brightness.
- **Number of LEDs** (default 5, up to 16).
- **Per-LED sensor + measurement** mapping — pick the sensor, then which value of it (e.g. SCD4x →
  CO₂/temp/humidity). Picking one fills in sensible default step values + colours.
- **Per-LED 3-step colour scale** — a value **and** colour per step; the live reading is interpolated
  between them (works ascending like temperature or descending like CO₂).

Saving applies everything **live**, no restart. The data pin is the **Status LEDs (WS2812)** pin in
*Pins* (default **D8** on XIAO).

### Auto-brightness (LDR)

The LDR (default pin **D0**) forms a divider with the on-board 10 kΩ pull-up to 3.3 V: brighter
surroundings → brighter LEDs, darkness → dim. The ambient/brightness ranges are calibratable in the
popup.

> ⚠️ **Pin conflicts:** default LED pin `D8` is also the default **PM1006K / PMSx003 RX** pin (a
> WS2812 and a particulate-sensor UART can't share a GPIO). Default LDR pin `D0` is also
> `RESET_CONFIG_PIN` on XIAO. Move the conflicting pin if you use both.

---

## 📶 Mesh Wi-Fi roaming

If several access points broadcast the **same SSID**, the device connects to the **strongest** one,
re-scans every 10 minutes, and hops to another AP only if it is **>10 dB stronger** (hysteresis
prevents flapping). Tunable in [`src/WS_config.h`](src/WS_config.h) (`ROAM_RSSI_THRESHOLD`,
`ROAM_CHECK_INTERVAL`).

---

## 💾 Backup & restore + security

The whole configuration (Wi-Fi, MQTT, sensor toggles, pins, LEDs, calibration…) lives in a single
`/config.json`. From the **Current Config** window:

- **⬇ Export Config** — download `weatherstation_config.json` (sensitive fields stay encrypted, so
  it's safe to keep).
- **⬆ Import Config** — upload a backup; you're asked for the password it was exported with, then the
  device applies and restarts. Wrong password is rejected **before** anything is changed.

### 🔒 Encrypted secrets

Wi-Fi password, MQTT username/password and the hotspot password are never stored in plain text —
they're **AES-256-CTR** encrypted in `/config.json` and in backups.

- Choose the encryption key via the **Config Password** (blank is allowed — still encrypted, just
  with an empty-derived key).
- The password itself is **never** in `/config.json`: it lives in **NVS (ESP32)** / a `/cfg.key` file
  (ESP8266), so the device decrypts on its own at boot and (on ESP32) survives a filesystem-erasing
  flash.

> **Threat model:** this protects the config file from being read (web UI / a copy of the JSON).
> Because the device must auto-decrypt at boot, the key is on-device — it does **not** protect
> against an attacker who can dump the full flash.

### 🔐 Web login & setup

The same password gates the whole web interface: a one-time **first-run setup**, then a themed
**login** (session cookie) on protected sites, plus a dedicated **change-password** page. Blank
password = open site. **Forgot it?** Hold the **config-reset pin** during boot to wipe the password +
setup flag and restore defaults, so you can never be locked out.

---

## 🗂️ Project structure

```
src/
  main.cpp            entry point (setup/loop)
  WS_config.h         config struct, board pin maps, globals, versions
  WS_json.h           config load/save, encryption, auth, import/export
  WS_network.h        web server + all HTTP endpoints, Wi-Fi/AP
  WS_ota.h            firmware update: web upload + GitHub-release OTA
  WS_mqtt.h           MQTT + Home Assistant discovery
  WS_sensors.h        sensor read loop
  WS_leds.h           WS2812 status LEDs + LDR brightness
  WS_main.h           boot + main loop
  SensorConfigs/      per-sensor config/read (conf_*.h)
  bt_hci_bridge.*     optional Bluetooth HCI-over-TCP bridge
data/index.html       the single-page web UI (LittleFS)
firmware maker/       build scripts → per-board .bin images
.github/workflows/    CI: build every board + publish a release
```

---

## 📜 Changelog

### 2026 — the rebuild (current)
A ground-up modernisation of the whole project:
- **New web UI** — single-page app served from LittleFS, 4 themes, live readings panel, live logs,
  brick-style pin/sensor layout.
- **Firmware update system** — browser `.bin` upload, **OTA from GitHub Releases**, settings
  preserved across updates; `firmware maker` multi-board build scripts and a **GitHub Action** that
  publishes a release on every commit.
- **Per-sensor calibration** offsets with live preview.
- **WS2812 status LEDs** with configurable colour scales + LDR auto-brightness.
- **Security & accounts** — first-run setup, web login, password change, AES-256-CTR encrypted
  secrets, encrypted config backup/restore with password verification.
- **Mesh Wi-Fi roaming**, **Advanced Settings** popup, suggested-area picker, optional **Bluetooth
  HCI bridge**, low-power mode.
- **V3 → V4 refactor** (early–mid 2026): modular `WS_*.h` sources, configurable pins with a UI,
  multi-board PlatformIO setup, SPS30 driver integration.

### 2022 — the original
The first version, written from scratch: a single-sketch ESP8266/ESP32 weather station reading a
handful of sensors and publishing to Home Assistant over MQTT. This is where it all started.

---

## 📄 License

Licensed under the **GNU General Public License v3.0** — use, modify, distribute and share freely,
with attribution and under the same license. Full text: [GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.html).

© [Nikpesu](https://github.com/Nikpesu), 2022–2026
