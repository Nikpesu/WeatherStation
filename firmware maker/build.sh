#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# WeatherStation firmware maker (Linux / macOS / Git-Bash).
#
# Builds the firmware (firmware.bin) and the web-UI filesystem image
# (littlefs.bin) for every board and drops them into
#     firmware maker/<board>/<board>-firmware.bin
#     firmware maker/<board>/<board>-littlefs.bin
# plus the ESP32 bootloader.bin / partitions.bin needed for a first-time
# (cable) flash. These names match what the OTA updater and the GitHub release
# workflow expect, so a folder produced here can be uploaded straight into a
# release, or a single .bin uploaded via the web UI (Firmware Update popup).
#
# Usage:   ./build.sh                 # all boards
#          ./build.sh seeed_xiao_esp32c3 d1_mini   # only these
# ---------------------------------------------------------------------------
set -u
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$ROOT_DIR" || exit 1

# Find PlatformIO on PATH, or fall back to the penv that ships with the install.
PIO="pio"
if ! command -v "$PIO" >/dev/null 2>&1; then
  for c in "$HOME/.platformio/penv/bin/pio" "$HOME/.platformio/penv/Scripts/pio.exe"; do
    if [ -x "$c" ]; then PIO="$c"; break; fi
  done
fi
echo "Using PlatformIO: $PIO"

# All boards (env names from platformio.ini). Pass args to build a subset.
ALL_BOARDS="d1_mini lolin_s2_mini seeed_xiao_esp32c3 seeed_xiao_esp32c6 seeed_xiao_esp32s3 esp32doit_devkit_v1 az_delivery_devkit_v4"
BOARDS="${*:-$ALL_BOARDS}"

ok=""; fail=""
for env in $BOARDS; do
  echo
  echo "=============================================================="
  echo "  Building $env"
  echo "=============================================================="
  out="$SCRIPT_DIR/$env"
  mkdir -p "$out"

  if "$PIO" run -e "$env" && "$PIO" run -e "$env" -t buildfs; then
    b=".pio/build/$env"
    cp "$b/firmware.bin" "$out/${env}-firmware.bin" 2>/dev/null || true
    # Filesystem image name varies by core (littlefs/spiffs); normalise it.
    for fs in littlefs.bin spiffs.bin fs.bin; do
      if [ -f "$b/$fs" ]; then cp "$b/$fs" "$out/${env}-littlefs.bin"; break; fi
    done
    # Extras for a full first-time flash over USB (ESP32).
    [ -f "$b/bootloader.bin" ] && cp "$b/bootloader.bin" "$out/"
    [ -f "$b/partitions.bin" ] && cp "$b/partitions.bin" "$out/"
    ok="$ok $env"
  else
    echo "!!! $env FAILED to build"
    fail="$fail $env"
  fi
done

echo
echo "=============================================================="
echo "Built:$ok"
[ -n "$fail" ] && echo "Failed:$fail  (some boards are known not to compile — see README.md)"
echo "Output: $SCRIPT_DIR/<board>/"
