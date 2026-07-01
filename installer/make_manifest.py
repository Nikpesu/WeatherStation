#!/usr/bin/env python3
"""
Assemble the ESP Web Tools site: read each board's build bins, decode its
partition table to find the real app/filesystem offsets, and emit a single
manifest.json (one build per chip, auto-selected by the browser).

Layout expected in --bins:  <bins>/<board>/{bootloader,partitions,firmware,littlefs}.bin
(these are produced by the CI build job / the `firmware maker` scripts).

Output in --out:  manifest.json, boot_app0.bin, and <board>/*.bin copied in.

The chip family + bootloader offset can't be read from the app bins, so they're
mapped here per board (env name). Everything else (app + fs offset) is decoded
from partitions.bin so a change to the partition table needs no edits here.
"""
import argparse, json, os, shutil, struct, sys

# env name -> (esp-web-tools chipFamily, bootloader flash offset)
BOARDS = {
    "seeed_xiao_esp32c3": ("ESP32-C3", 0x0000),
    "seeed_xiao_esp32c6": ("ESP32-C6", 0x0000),
    "seeed_xiao_esp32s3": ("ESP32-S3", 0x0000),
    "lolin_s2_mini":      ("ESP32-S2", 0x1000),
}

PART_MAGIC = 0x50AA          # entries start with bytes 0xAA 0x50 (LE u16 = 0x50AA)
TYPE_APP, TYPE_DATA = 0x00, 0x01
SUBTYPE_SPIFFS = 0x82        # LittleFS lives in a "spiffs" data partition


def parse_partitions(path):
    """Return (app_offset, fs_offset) decoded from a partitions.bin image."""
    data = open(path, "rb").read()
    app_off, fs_off = None, None
    for i in range(0, len(data), 32):
        entry = data[i:i + 32]
        if len(entry) < 32:
            break
        (magic, ptype, psubtype, offset, size) = struct.unpack("<HBBII", entry[:12])
        if magic != PART_MAGIC:
            break                       # end of table
        if ptype == TYPE_APP:
            app_off = offset if app_off is None else min(app_off, offset)
        elif ptype == TYPE_DATA and psubtype == SUBTYPE_SPIFFS:
            fs_off = offset
    if app_off is None:
        raise RuntimeError(f"{path}: no app partition found")
    return app_off, fs_off


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--bins", required=True, help="dir with <board>/*.bin")
    ap.add_argument("--out", required=True, help="site output dir")
    ap.add_argument("--boot-app0", required=True, help="path to boot_app0.bin")
    ap.add_argument("--version", default="dev")
    args = ap.parse_args()

    os.makedirs(args.out, exist_ok=True)
    shutil.copy(args.boot_app0, os.path.join(args.out, "boot_app0.bin"))

    builds = []
    for board, (chip, bl_off) in BOARDS.items():
        # Local scripts drop bins in <bins>/<board>/; the CI download-artifact
        # step leaves them in <bins>/fw-<board>/ — accept either.
        bdir = os.path.join(args.bins, board)
        if not os.path.isdir(bdir):
            alt = os.path.join(args.bins, "fw-" + board)
            if os.path.isdir(alt):
                bdir = alt
        need = {n: os.path.join(bdir, f"{board}-{n}.bin") for n in
                ("bootloader", "partitions", "firmware", "littlefs")}
        # CI names them "<board>-<part>.bin"; fall back to plain "<part>.bin".
        for n, p in list(need.items()):
            if not os.path.exists(p):
                alt = os.path.join(bdir, f"{n}.bin")
                if os.path.exists(alt):
                    need[n] = alt
        if not all(os.path.exists(p) for p in need.values()):
            print(f"skip {board}: missing bins", file=sys.stderr)
            continue

        app_off, fs_off = parse_partitions(need["partitions"])
        outdir = os.path.join(args.out, board)
        os.makedirs(outdir, exist_ok=True)
        for n, p in need.items():
            shutil.copy(p, os.path.join(outdir, f"{n}.bin"))

        parts = [
            {"path": f"{board}/bootloader.bin", "offset": bl_off},
            {"path": f"{board}/partitions.bin", "offset": 0x8000},
            {"path": "boot_app0.bin",           "offset": 0xE000},
            {"path": f"{board}/firmware.bin",   "offset": app_off},
        ]
        if fs_off is not None:
            parts.append({"path": f"{board}/littlefs.bin", "offset": fs_off})
        builds.append({"chipFamily": chip, "parts": parts})
        print(f"{board}: {chip} bl=0x{bl_off:x} app=0x{app_off:x} fs="
              f"{'0x%x' % fs_off if fs_off else 'n/a'}")

    manifest = {
        "name": "WeatherStation",
        "version": args.version,
        "new_install_prompt_erase": True,
        "builds": builds,
    }
    with open(os.path.join(args.out, "manifest.json"), "w") as f:
        json.dump(manifest, f, indent=2)
    print(f"\nwrote manifest with {len(builds)} build(s) -> {args.out}/manifest.json")
    if not builds:
        sys.exit("no builds assembled")


if __name__ == "__main__":
    main()
