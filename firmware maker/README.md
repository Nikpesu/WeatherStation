# Firmware maker

Build ready-to-flash images for every supported board in one go.

```bash
# Linux / macOS / Git-Bash
./build.sh                       # all boards
./build.sh seeed_xiao_esp32c3    # just one

# Windows
build.bat
build.bat seeed_xiao_esp32c3
```

Each board gets its own folder with:

| File | What it is |
|------|-----------|
| `<board>-firmware.bin`   | the program — flash to update the firmware |
| `<board>-littlefs.bin`   | the web UI (contents of `data/`) — flash to update the site |
| `bootloader.bin`, `partitions.bin` | ESP32 only, needed for a **first-time** cable flash |

The `<board>` key is the PlatformIO env name from [`platformio.ini`](../platformio.ini),
e.g. `seeed_xiao_esp32c3`. These exact names are what the **OTA updater** and the
**GitHub release** workflow look for, so a board folder here can be dropped straight
into a GitHub release.

## Flashing

**Easiest — over Wi-Fi, no cable (web UI):** open the device page → **Firmware Update**
→ upload `<board>-firmware.bin` (and/or `<board>-littlefs.bin`). Settings are kept.

**Over the air from GitHub:** device page → **Firmware Update** → *Check GitHub for
updates* → *Download & install*. Pulls the matching asset from
<https://github.com/Nikpesu/WeatherStation/releases>.

**First-time flash over USB (esptool):**

```bash
# ESP32 (adjust port; offsets are the defaults for the shipped partition table)
esptool.py --chip esp32 --port COM5 write_flash \
  0x1000  bootloader.bin \
  0x8000  partitions.bin \
  0x10000 <board>-firmware.bin \
  0x290000 <board>-littlefs.bin        # filesystem offset from my_partitions.csv
```

Or just use PlatformIO: `pio run -e <board> -t upload` then `-t uploadfs`.

## Notes

- Some boards are known **not** to compile (`esp32doit_devkit_v1`,
  `az_delivery_devkit_v4`) because they don't define the `Dx` pin labels the code
  uses. The scripts keep going and just report them as failed.
- The generated `.bin` files are git-ignored (see `.gitignore`); only the scripts
  are tracked.
