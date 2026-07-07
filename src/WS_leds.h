// ───────────────────────── sensor status-indicator LEDs ─────────────────────────
// Drives a short WS2812 / NeoPixel strip (LED_COUNT LEDs) that mirrors the health
// of the enabled sensors. The LEDs fill in sensor order: each enabled sensor takes
// the next free LED (up to LED_COUNT), the rest stay off.
//   green = sensor is producing valid (non-NaN) data
//   red   = sensor is enabled but has no data yet / is not responding
//   off   = unused slot
// The whole feature is gated by config.led_toggle and uses the user-configurable
// config.LED_pin (see WS_config.h).

// Catalogue of every measurement an LED can show. Each entry knows which sensor
// it belongs to (index into toggles[]/toggleIDName, for enable-gating + name), a
// human label for the "part" dropdown, the live value, and a `key` that selects
// per-measurement-type defaults in the popup. config.led_map[i] stores an index
// into THIS table (or -1 for none).
struct LedMeasure { int sensor; const char* label; float* value; const char* key; };
LedMeasure ledMeasures[] = {
    {0, "Temperature", &aht2x_temp,    "temp"},
    {0, "Humidity",    &aht2x_hum,     "hum"},
    {1, "Temperature", &bmp280_temp,   "temp"},
    {1, "Pressure",    &bmp280_press,  "press"},
    {1, "Altitude",    &bmp280_alt,    "alt"},
    {2, "eCO2",        &ens160_eco2,   "co2"},
    {2, "TVOC",        &ens160_tvoc,   "tvoc"},
    {2, "AQI",         &ens160_aqi,    "aqi"},
    {3, "PM1.0",       &pm1006k_pm1_0, "pm"},
    {3, "PM2.5",       &pm1006k_pm2_5, "pm"},
    {3, "PM10",        &pm1006k_pm10_0,"pm"},
    {4, "PM1.0",       &pmsx003_pm1_0, "pm"},
    {4, "PM2.5",       &pmsx003_pm2_5, "pm"},
    {4, "PM10",        &pmsx003_pm10_0,"pm"},
    {4, "Temperature", &pmsx003_temp,  "temp"},
    {4, "Humidity",    &pmsx003_hum,   "hum"},
    {4, "HCHO",        &pmsx003_hcho,  "hcho"},
    {5, "CO2",         &scd4x_co2,     "co2"},
    {5, "Temperature", &scd4x_temp,    "temp"},
    {5, "Humidity",    &scd4x_hum,     "hum"},
    {6, "TVOC",        &sgp30_tvoc,    "tvoc"},
    {6, "eCO2",        &sgp30_co2,     "co2"},
    {6, "Ethanol",     &sgp30_eth,     "raw"},
    {6, "H2",          &sgp30_h2,      "raw"},
    {7, "Temperature", &sht31_temp,    "temp"},
    {7, "Humidity",    &sht31_hum,     "hum"},
    {8, "PM1.0",       &sps30_pm1_0,   "pm"},
    {8, "PM2.5",       &sps30_pm2_5,   "pm"},
    {8, "PM10",        &sps30_pm10_0,  "pm"},
    {9, "Temperature", &bmp580_temp,   "temp"},
    {9, "Pressure",    &bmp580_press,  "press"},
    {9, "Altitude",    &bmp580_alt,    "alt"}
};
#define MEASURE_COUNT ((int)(sizeof(ledMeasures)/sizeof(ledMeasures[0])))
static_assert(MEASURE_COUNT <= SENSOR_MEASURE_COUNT,
              "SENSOR_MEASURE_COUNT (WS_config.h) must be >= number of ledMeasures[] entries");

// Add the per-measurement calibration offset to every live value of the given
// sensor. Call right after a sensor's xRead() (which reassigns the globals from the
// hardware each cycle), so the offset is applied exactly once and flows through both
// the MQTT payload and the LED / live-readings displays. NaN readings are left alone.
void applySensorOffset(int sensorIdx)
{
  for (int m = 0; m < MEASURE_COUNT; m++)
    if (ledMeasures[m].sensor == sensorIdx && !isnan(*ledMeasures[m].value))
      *ledMeasures[m].value += config.sensor_offset[m];
}

// (Re)initialise the strip on the currently configured pin. Called from
// sensorsBegin(), which also runs on every WiFi reconnect, so this MUST be
// idempotent: calling Adafruit_NeoPixel::begin() again does pinMode() on the
// data pin, which detaches the already-attached RMT channel while the backend
// keeps caching the old pin and never re-inits — so every later show() fails
// with "GPIO N is not attached to an RMT channel". We therefore only touch the
// hardware on the first init or when the configured pin actually changes (a real
// pin change re-attaches correctly because the backend sees pin != cached pin).
void ledsBegin()
{
  static int initedPin = -2;  // -2 = hardware never initialised
  static int initedCount = -1;

  if (!led_toggle)
  {
    // Feature disabled: if the strip was previously running, blank it once;
    // if it was never initialised, leave the pin untouched so a sensor (e.g. a
    // PM sensor sharing D8) can use it freely. Reset the pin cache so re-enabling
    // does a full re-init (the RMT channel may have been detached meanwhile).
    if (initedPin >= 0) { leds.clear(); leds.show(); initedPin = -2; }
    return;
  }

  if (initedPin == config.LED_pin)
  {
    // Same pin: only the LED count may have changed. updateLength() just
    // reallocates the pixel buffer; it does NOT call begin()/pinMode(), so it
    // won't detach the RMT channel (see the note above), unlike a re-begin().
    if (initedCount != config.led_count)
    {
      leds.updateLength(config.led_count);
      leds.clear();
      leds.show();
      initedCount = config.led_count;
    }
    return;
  }

  // First init or a real pin change: a full (re)init is needed.
  leds.setPin(config.LED_pin);
  leds.updateLength(config.led_count);
  leds.begin();
  leds.setBrightness(LED_BRIGHTNESS);
  leds.clear();
  leds.show(); // attaches the RMT channel on the (possibly new) pin
  initedPin = config.LED_pin;
  initedCount = config.led_count;
}

// Strip brightness (0-255). With auto-brightness off it's a fixed value; with the
// LDR enabled it's mapped from ambient light (bright -> brighter so LEDs stay
// visible by day and dim at night), lightly smoothed to avoid flicker.
uint8_t ledBrightness()
{
  if (!ldr_brightness_toggle) return LED_BRIGHTNESS;

  static float smoothed = -1.0f;
  int raw = analogRead(config.LDR_pin); // 0 = bright .. LDR_ADC_MAX = dark
  if (smoothed < 0) smoothed = raw;
  smoothed += (raw - smoothed) * 0.2f; // exponential moving average

  long b;
  if (config.ldr_ambient_max == config.ldr_ambient_min)
    b = config.ldr_bright_min;  // avoid division by zero in map()
  else
    b = map((long)smoothed, config.ldr_ambient_min, config.ldr_ambient_max,
            config.ldr_bright_max, config.ldr_bright_min);
  if (b < config.ldr_bright_min) b = config.ldr_bright_min;
  if (b > config.ldr_bright_max) b = config.ldr_bright_max;
  return (uint8_t)b;
}

// Blend two packed 0xRRGGBB colours by fraction t (0..1).
uint32_t lerpColor(uint32_t c1, uint32_t c2, float t)
{
  if (t < 0.0f) t = 0.0f;
  if (t > 1.0f) t = 1.0f;
  uint8_t r1 = (c1 >> 16) & 0xFF, g1 = (c1 >> 8) & 0xFF, b1 = c1 & 0xFF;
  uint8_t r2 = (c2 >> 16) & 0xFF, g2 = (c2 >> 8) & 0xFF, b2 = c2 & 0xFF;
  uint8_t r = (uint8_t)(r1 + (r2 - r1) * t + 0.5f);
  uint8_t g = (uint8_t)(g1 + (g2 - g1) * t + 0.5f);
  uint8_t b = (uint8_t)(b1 + (b2 - b1) * t + 0.5f);
  return leds.Color(r, g, b);
}

// Interpolate a colour from a live value along a 3-step scale: value v1 shows
// colour c1, v2 shows c2 and v3 shows c3, blending between them. Works for
// ascending or descending value scales (v1<v3 or v1>v3); values past an end
// clamp to that end colour.
uint32_t ledColorFromValue(float v, float v1, uint32_t c1, float v2, uint32_t c2, float v3, uint32_t c3)
{
  float d1 = v2 - v1;
  float f1 = (d1 != 0.0f) ? (v - v1) / d1 : 2.0f; // degenerate v1==v2 -> segment 2
  if (f1 <= 1.0f) // segment c1 -> c2
    return lerpColor(c1, c2, f1);
  // segment c2 -> c3
  float d2 = v3 - v2;
  float f2 = (d2 != 0.0f) ? (v - v2) / d2 : 1.0f;
  return lerpColor(c2, c3, f2);
}

// Refresh the LED colours from the live sensor state. Cheap, but self-throttled so
// it can be called every loop iteration without flooding the strip. Each LED shows
// the sensor it is mapped to (config.led_map), coloured by where its live reading
// falls on that LED's red/yellow/green value scale. Off = unmapped slot, disabled
// sensor, or no data (NaN).
// Set true the first time the WS2812 strip actually lights a pixel (i.e. the status
// LEDs "turn on"). The boot-blink watches this to know when to stop blinking.
bool ledsHaveLit = false;

void ledsUpdate()
{
  if (!led_toggle) return;

  static unsigned long lastLedUpdate = 0;
  const unsigned long LED_REFRESH_MS = 500;
  if (millis() - lastLedUpdate < LED_REFRESH_MS) return;
  lastLedUpdate = millis();

  // Set brightness BEFORE writing pixels: Adafruit_NeoPixel scales colours at
  // setPixelColor() time, so the order keeps the mapping lossless per frame.
  leds.setBrightness(ledBrightness());
  leds.clear();

  bool lit = false;
  for (int led = 0; led < config.led_count; led++)
  {
    int m = config.led_map[led];
    if (m < 0 || m >= MEASURE_COUNT) continue;   // unmapped slot -> off
    if (!*toggles[ledMeasures[m].sensor]) continue; // mapped sensor disabled -> off
    float v = *ledMeasures[m].value;
    if (isnan(v)) continue;                       // no data -> off
    int pixel = config.led_reverse ? (config.led_count - 1 - led) : led;
    leds.setPixelColor(pixel, ledColorFromValue(v, config.led_red[led],    config.led_col1[led],
                                                  config.led_yellow[led], config.led_col2[led],
                                                  config.led_green[led],  config.led_col3[led]));
    lit = true;
  }
  leds.show();
  if (lit) ledsHaveLit = true;
}

// ── Boot blink on the onboard ESP LED ────────────────────────────────────────
// On startup blink the ESP's own LED. If the WS2812 status LEDs are enabled, keep
// blinking until they light up (first coloured frame); if they are disabled, just
// blink 10 times. Non-blocking: armed by espBootBlinkBegin() and driven by
// espBootBlinkTick() from the main loop (and the WiFi-connect wait).
static bool espBlinkDone   = true;   // starts idle until armed
static bool espBlinkTenMode = false; // true = blink 10x then stop (status LEDs off)
static bool espBlinkLevel  = false;  // current logical state (true = lit)
static int  espBlinkCount  = 0;      // completed blinks (on->off cycles)
static unsigned long espBlinkLast  = 0;
static unsigned long espBlinkStart = 0;

inline void espLedSet(bool on)
{
  espBlinkLevel = on;
#if ESP_STATUS_LED_ACTIVE_LOW
  digitalWrite(ESP_STATUS_LED_PIN, on ? LOW : HIGH);
#else
  digitalWrite(ESP_STATUS_LED_PIN, on ? HIGH : LOW);
#endif
}

void espBootBlinkBegin()
{
  pinMode(ESP_STATUS_LED_PIN, OUTPUT);
  espBlinkTenMode = !led_toggle;   // status LEDs on -> blink until they light
  espBlinkCount   = 0;
  espBlinkLast    = millis();
  espBlinkStart   = millis();
  espBlinkDone    = false;
  espLedSet(true);                 // start lit
}

void espBootBlinkTick()
{
  if (espBlinkDone) return;

  // Stop conditions.
  if (espBlinkTenMode)
  {
    if (espBlinkCount >= 10) { espLedSet(false); espBlinkDone = true; return; }
  }
  else
  {
    // Blink until the status LEDs light, with a safety cap so a misconfigured or
    // AP-mode device never blinks forever.
    if (ledsHaveLit || millis() - espBlinkStart > 60000UL)
      { espLedSet(false); espBlinkDone = true; return; }
  }

  const unsigned long BLINK_MS = 150;
  if (millis() - espBlinkLast >= BLINK_MS)
  {
    espBlinkLast = millis();
    espLedSet(!espBlinkLevel);
    if (!espBlinkLevel) espBlinkCount++;  // count each full on->off blink
  }
}
