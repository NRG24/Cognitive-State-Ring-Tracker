# nRF52832 Programming with the Raspberry Pi Debug Probe

A complete, step-by-step guide to flashing firmware onto the **nRF52832** using the official **Raspberry Pi Debug Probe** over SWD.

---

## Table of Contents
1. [What You Need](#1-what-you-need)
2. [Wiring / Connections](#2-wiring--connections)
3. [Install Dependencies](#3-install-dependencies)
4. [Verify the Probe is Detected](#4-verify-the-probe-is-detected)
5. [Test the OpenOCD Connection](#5-test-the-openocd-connection)
6. [Flash Your Firmware](#6-flash-your-firmware)
7. [Erase the Chip](#7-erase-the-chip-full-chip-erase)
8. [Read / Dump Existing Firmware](#8-read--dump-existing-firmware)
9. [Using pyOCD (Alternative to OpenOCD)](#9-using-pyocd-alternative-to-openocd)
10. [Troubleshooting](#10-troubleshooting)

---

## 1. What You Need

| Item | Notes |
|---|---|
| Raspberry Pi Debug Probe | Official RPi probe, uses CMSIS-DAP |
| nRF52832 target board | Your ring PCB or dev board |
| 3× jumper wires | SWDIO, SWDCLK, GND |
| Optional 4th wire | If target needs power from the probe (VTREF/3.3 V) |
| Host computer | Linux, macOS, or Windows with OpenOCD or pyOCD |
| Firmware file | `.hex` or `.bin` built from your project |

> **Note:** The Raspberry Pi Debug Probe outputs **3.3 V logic** — compatible with the nRF52832 directly.

---

## 2. Wiring / Connections

Connect the **3-pin SWD JST connector** on the RPi Debug Probe to the SWD pads on your nRF52832 board.

| Debug Probe Pin | nRF52832 Pin | Wire Color (typical) |
|---|---|---|
| `SWDIO` | `SWDIO` (P0.18 default) | Yellow |
| `SWDCLK` | `SWDCLK` (P0.19 default) | Green |
| `GND` | `GND` | Black |
| `VTref` (optional) | `VCC` / `3.3 V` | Red |

> If your nRF52832 board is **self-powered**, only connect SWDIO, SWDCLK, and GND.
> If you want the probe to **power the target**, connect VTref to the board's 3.3 V rail (max 300 mA from probe).

The Debug Probe also has a **UART connector** — you can use that for serial output (TX→RX, RX→TX, GND) separately.

---

## 3. Install Dependencies

### On Linux (Debian/Ubuntu/Raspberry Pi OS)

```bash
sudo apt-get update
sudo apt-get install -y openocd
```

Verify the installed version (needs to be **0.11+** for full CMSIS-DAP support):

```bash
openocd --version
```

### On macOS (Homebrew)

```bash
brew install open-ocd
```

### On Windows

Download the latest OpenOCD release from:  
https://github.com/openocd-org/openocd/releases

Extract it and add the `bin/` folder to your system `PATH`.

---

## 4. Verify the Probe is Detected

Plug the Debug Probe in via USB, then run:

```bash
lsusb
```

You should see a line like:

```
Bus 001 Device 003: ID 2e8a:000c Raspberry Pi Debug Probe
```

If not, try a different USB port or cable.

---

## 5. Test the OpenOCD Connection

Use the provided `openocd.cfg` file in this folder, or run the command inline.

### Using the config file (recommended)

Copy `openocd.cfg` to your project directory (same folder as your firmware), then run:

```bash
openocd -f openocd.cfg
```

### Inline command (no config file needed)

```bash
openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg -c "adapter speed 1000"
```

**Expected output (successful connection):**

```
Open On-Chip Debugger ...
Info : CMSIS-DAP: ...
Info : clock speed 1000 kHz
Info : SWD DPIDR 0x2ba01477
Info : nrf52.cpu: hardware has 6 breakpoints, 4 watchpoints
```

If you see `SWD DPIDR 0x2ba01477` — you're connected to the nRF52832. Press `Ctrl+C` to exit.

---

## 6. Flash Your Firmware

### Flash a `.hex` file (most common — from Arduino, Zephyr, nRF SDK)

```bash
openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg \
  -c "adapter speed 1000" \
  -c "program your_firmware.hex verify reset exit"
```

Replace `your_firmware.hex` with your actual file path.

### Flash a `.bin` file

You must specify the load address. For nRF52832 the application starts at `0x00000000` (or `0x00001000` if a SoftDevice is present):

```bash
openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg \
  -c "adapter speed 1000" \
  -c "program your_firmware.bin verify reset exit 0x00000000"
```

### Flash with SoftDevice (e.g. s132)

Flash the SoftDevice first, then your application:

```bash
# 1. Flash SoftDevice (starts at address 0x0)
openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg \
  -c "adapter speed 1000" \
  -c "program s132_nrf52_7.3.0_softdevice.hex verify reset exit"

# 2. Flash your application (starts after SoftDevice, typically 0x27000)
openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg \
  -c "adapter speed 1000" \
  -c "program your_app.hex verify reset exit"
```

> Check the SoftDevice release notes for the exact application start address.

---

## 7. Erase the Chip (Full Chip Erase)

Run this before flashing if you're getting errors or want a clean slate:

```bash
openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg \
  -c "adapter speed 1000; init; halt; nrf5 mass_erase; exit"
```

---

## 8. Read / Dump Existing Firmware

To back up the current firmware before overwriting:

```bash
openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg \
  -c "adapter speed 1000" \
  -c "init; halt; dump_image backup_firmware.bin 0x00000000 0x80000; exit"
```

This dumps the full 512 KB flash to `backup_firmware.bin`.

---

## 9. Using pyOCD (Alternative to OpenOCD)

pyOCD is often easier to install and works great with the RPi Debug Probe.

### Install pyOCD

```bash
pip install pyocd
```

### Install nRF52832 support pack

```bash
pyocd pack install nrf52832
```

### Flash firmware with pyOCD

```bash
pyocd flash -t nrf52832 your_firmware.hex
```

### Erase with pyOCD

```bash
pyocd erase -t nrf52832 --chip
```

### Check connected probes

```bash
pyocd list
```

---

## 10. Troubleshooting

| Problem | Fix |
|---|---|
| `Error: open failed` / probe not found | Check USB connection; run `lsusb` to confirm probe is visible |
| `SWD DPIDR` not shown / `error connecting` | Check SWDIO/SWDCLK/GND wiring; make sure target is powered |
| `Error: nRF52832-QFAA unknown` | Update OpenOCD to 0.11+ or use pyOCD |
| `Error: timed out waiting for target halt` | Try reducing adapter speed: `-c "adapter speed 500"` |
| Flashing succeeds but device doesn't run | Ensure reset pin isn't held low; try adding `-c "reset run"` after programming |
| `Error: couldn't halt target` after mass erase | This is normal — just re-run the flash command |
| UDEV permissions error on Linux | Add udev rule (see below) |

### Fix USB permissions on Linux (udev rule)

```bash
sudo tee /etc/udev/rules.d/99-raspberry-pi-debug-probe.rules << 'EOF'
SUBSYSTEM=="usb", ATTR{idVendor}=="2e8a", ATTR{idProduct}=="000c", MODE="0666", GROUP="plugdev"
EOF
```

```bash
sudo udevadm control --reload-rules && sudo udevadm trigger
```

Then log out and back in (or run `sudo usermod -aG plugdev $USER`).

---

## Quick Reference — All Commands

```bash
# Check probe is connected
lsusb | grep -i "2e8a"

# Test connection to nRF52832
openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg -c "adapter speed 1000"

# Flash .hex firmware
openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg \
  -c "adapter speed 1000" \
  -c "program your_firmware.hex verify reset exit"

# Flash .bin firmware
openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg \
  -c "adapter speed 1000" \
  -c "program your_firmware.bin verify reset exit 0x00000000"

# Full chip erase
openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg \
  -c "adapter speed 1000; init; halt; nrf5 mass_erase; exit"

# Dump existing firmware
openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg \
  -c "adapter speed 1000" \
  -c "init; halt; dump_image backup_firmware.bin 0x00000000 0x80000; exit"

# pyOCD flash (alternative)
pyocd flash -t nrf52832 your_firmware.hex

# pyOCD erase (alternative)
pyocd erase -t nrf52832 --chip

# Fix udev permissions (Linux only)
sudo tee /etc/udev/rules.d/99-raspberry-pi-debug-probe.rules << 'EOF'
SUBSYSTEM=="usb", ATTR{idVendor}=="2e8a", ATTR{idProduct}=="000c", MODE="0666", GROUP="plugdev"
EOF
sudo udevadm control --reload-rules && sudo udevadm trigger
```
