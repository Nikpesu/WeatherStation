#include "bt_hci_bridge.h"

#if defined(SOC_BT_SUPPORTED)

#include <WiFi.h>
#include "esp_bt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"

// Pick controller mode: original ESP32 supports BLE + classic (BTDM).
// C3 / C6 / S3 are BLE-only.
#if defined(CONFIG_IDF_TARGET_ESP32)
  #define HCI_BT_MODE ESP_BT_MODE_BTDM
#else
  #define HCI_BT_MODE ESP_BT_MODE_BLE
#endif

static WiFiServer        s_server(6789);
static WiFiClient        s_client;
static RingbufHandle_t   s_rxRing = nullptr;   // controller -> TCP (whole HCI packets)
static volatile bool     s_clientUp = false;
static bool              s_started = false;    // bridge/controller brought up this boot
static uint16_t          s_port = 6789;

// ---- VHCI callbacks (run in controller task context, must not block) ----

// Controller has buffer space again. We poll check_send_available() instead,
// so this can stay a no-op.
static void vhci_send_ready(void) {}

// Controller delivered one complete HCI packet (incl. type byte).
static int vhci_recv(uint8_t *data, uint16_t len) {
    if (s_rxRing && s_clientUp) {
        // NOSPLIT keeps each packet intact; drop if ring is full (0 timeout).
        xRingbufferSend(s_rxRing, data, len, 0);
    }
    return 0;
}

static const esp_vhci_host_callback_t s_vhciCb = { vhci_send_ready, vhci_recv };

// ---- helpers ----

static bool readN(WiFiClient &c, uint8_t *buf, size_t n) {
    size_t got = 0;
    while (got < n) {
        if (!c.connected()) return false;
        int r = c.read(buf + got, n - got);
        if (r > 0) got += r;
        else       vTaskDelay(pdMS_TO_TICKS(1));
    }
    return true;
}

// Read one H4 packet from TCP and push it to the controller.
static bool pumpHostToController(WiFiClient &c) {
    static uint8_t pkt[1100];          // BLE/typical ACL fits; bump if you need big classic ACL
    uint8_t type;
    if (!readN(c, &type, 1)) return false;

    size_t hdr, lenField;
    switch (type) {
        case 0x01: hdr = 3; break;     // CMD:  opcode(2) + plen(1)
        case 0x02: hdr = 4; break;     // ACL:  handle(2) + dlen(2, LE)
        case 0x03: hdr = 3; break;     // SCO:  handle(2) + len(1)
        default:   return false;       // host never sends events (0x04)
    }
    pkt[0] = type;
    if (!readN(c, pkt + 1, hdr)) return false;

    if (type == 0x02) lenField = pkt[3] | (pkt[4] << 8);   // ACL: 16-bit LE
    else              lenField = pkt[1 + hdr - 1];         // CMD/SCO: last header byte

    size_t total = 1 + hdr + lenField;
    if (total > sizeof(pkt)) return false;                 // oversized -> resync by dropping client
    if (lenField && !readN(c, pkt + 1 + hdr, lenField)) return false;

    while (!esp_vhci_host_check_send_available()) vTaskDelay(pdMS_TO_TICKS(1));
    esp_vhci_host_send_packet(pkt, total);
    return true;
}

// ---- tasks ----

// controller -> TCP
static void txTask(void *) {
    for (;;) {
        size_t n = 0;
        uint8_t *item = (uint8_t *)xRingbufferReceive(s_rxRing, &n, portMAX_DELAY);
        if (item) {
            if (s_clientUp && s_client.connected()) s_client.write(item, n);
            vRingbufferReturnItem(s_rxRing, item);
        }
    }
}

// accept + TCP -> controller
static void serverTask(void *) {
    for (;;) {
        WiFiClient c = s_server.available();
        if (!c) { vTaskDelay(pdMS_TO_TICKS(20)); continue; }

        s_client = c;
        s_client.setNoDelay(true);     // latency matters for HCI
        s_clientUp = true;

        while (s_client.connected()) {
            if (!pumpHostToController(s_client)) break;
        }

        s_clientUp = false;
        s_client.stop();
        // controller keeps running; next attach will HCI-Reset it
    }
}

// ---- public API ----

void btHciBridgeBegin(uint16_t port) {
    if (s_started) return;   // the BT controller can only be initialised once per boot
    s_port = port;
    s_rxRing = xRingbufferCreate(8192, RINGBUF_TYPE_NOSPLIT);

    esp_bt_controller_config_t cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&cfg);
    esp_bt_controller_enable((esp_bt_mode_t)HCI_BT_MODE);
    esp_vhci_host_register_callback(&s_vhciCb);

    s_server = WiFiServer(port);
    s_server.begin();
    s_server.setNoDelay(true);

    xTaskCreatePinnedToCore(txTask,     "hci_tx", 4096, nullptr, 5, nullptr, 0);
    xTaskCreatePinnedToCore(serverTask, "hci_rx", 4096, nullptr, 5, nullptr, 0);

    s_started = true;
}

bool btHciBridgeSupported() { return true; }
bool btHciBridgeRunning()   { return s_started; }
bool btHciBridgeClient()    { return s_clientUp; }
uint16_t btHciBridgePort()  { return s_port; }

#endif // SOC_BT_SUPPORTED
