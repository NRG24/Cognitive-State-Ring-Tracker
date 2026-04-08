# scripts/ — nRF52832 Flash Tools (Raspberry Pi Debug Probe)

Scripts to erase and flash the Raytac MDBT42 (nRF52832) using a Raspberry Pi Debug Probe over SWD via OpenOCD.

---

## Hardware Wiring

| Raspberry Pi Debug Probe | nRF52832 (MDBT42) |
|--------------------------|-------------------|
| SWDIO                    | SWDIO (P0.26)     |
| SWCLK                    | SWDCLK (P0.27)    |
| GND                      | GND               |
| VTref (3.3V)             | VDD (3.3V)        |
| RESET (optional)         | RESET             |

> **Note:** Power the nRF52832 from an external supply or your PCB — do not rely on the probe to power the target unless it's a low-power test setup.

---

## Requirements

Install OpenOCD on the Raspberry Pi:

```bash
sudo apt update
sudo apt install openocd
```

Verify OpenOCD supports CMSIS-DAP (it should on modern Raspberry Pi OS):

```bash
openocd --version
```

---

## Files

| File                   | Purpose                                         |
|------------------------|-------------------------------------------------|
| `rpi_debug_probe.cfg`  | OpenOCD config — CMSIS-DAP/SWD, nRF52832 target |
| `flash.sh`             | Flash a firmware `.hex` to the nRF52832         |
| `erase.sh`             | Mass-erase the nRF52832 (clears all flash)      |

---

## Usage

### 1. Copy scripts to Raspberry Pi

```bash
scp scripts/rpi_debug_probe.cfg scripts/flash.sh scripts/erase.sh pi@<PI_IP>:~/nrf52_tools/
ssh pi@<PI_IP>
chmod +x ~/nrf52_tools/flash.sh ~/nrf52_tools/erase.sh
```

### 2. Test connection (verify OpenOCD can see target)

```bash
openocd -f ~/nrf52_tools/rpi_debug_probe.cfg -c "init; reset halt; mdw 0x10000000; exit"
```

Expected output includes the chip ID — if you see `0x10000000: XXXXXXXX` the probe is connected.

### 3. Erase the chip (optional, do before flashing if chip is locked)

```bash
cd ~/nrf52_tools
./erase.sh
```

### 4. Flash firmware

```bash
cd ~/nrf52_tools
./flash.sh /path/to/firmware.hex
```

Example with Arduino-exported hex:

```bash
./flash.sh ~/firmware/gsr_heart_ble.ino.hex
```

---

## Building Firmware (two options)

### Option A — Arduino IDE (easiest)

1. Install **Arduino IDE 2.x**
2. Add nRF5 board support URL in preferences:
   ```
   https://sandeepmistry.github.io/arduino-nRF5/package_nRF5_index.json
   ```
3. Install board: **Nordic Semiconductor nRF5 Boards** → select **Nordic nRF52832**
4. Open `firmware/gsr_heart_ble/` sketch
5. Sketch → Export Compiled Binary → outputs a `.hex`
6. Copy `.hex` to RPi and flash with `./flash.sh`

### Option B — nRF5 SDK + ARM GCC

1. Download [nRF5 SDK 17.1.0](https://www.nordicsemi.com/Software-and-tools/Software/nRF5-SDK)
2. Install arm-none-eabi-gcc:
   ```bash
   sudo apt install gcc-arm-none-eabi
   ```
3. Build:
   ```bash
   cd <sdk>/examples/ble_peripheral/ble_app_uart/pca10040/s132/armgcc
   make
   ```
4. Flash the merged softdevice + app hex:
   ```bash
   ./flash.sh _build/nrf52832_xxaa.hex
   ```

---

## Softdevice Note

If using SoftDevice (BLE stack), you must flash it separately **once** before flashing your app:

```bash
# flash S132 softdevice first
./flash.sh /path/to/s132_nrf52_7.3.0_softdevice.hex

# then flash your app
./flash.sh /path/to/your_app.hex
```

Download S132 from: https://www.nordicsemi.com/Software-and-tools/Software/S132
