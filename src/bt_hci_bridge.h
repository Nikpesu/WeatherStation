#pragma once
#include <Arduino.h>

// HCI-over-TCP bridge: exposes the ESP32 BT controller's raw HCI interface
// over a TCP socket so a Linux host can attach it as a virtual hciX adapter.
//
// Only compiles on chips with a BT controller (ESP32 / C3 / C6 / S3).
// On ESP32-S2 and ESP8266 these calls become no-ops.

#if defined(SOC_BT_SUPPORTED)
// Start the bridge. Call AFTER WiFi is connected.
// `port` is the TCP port the Linux host will connect to.
void btHciBridgeBegin(uint16_t port = 6789);
#else
inline void btHciBridgeBegin(uint16_t = 6789) {}
#endif
