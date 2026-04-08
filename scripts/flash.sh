#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CFG="$SCRIPT_DIR/rpi_debug_probe.cfg"

if [ $# -eq 0 ]; then
    echo "Usage: ./flash.sh <firmware.hex>"
    echo "Example: ./flash.sh ../firmware/gsr_heart_ble/build/nrf52832_xxaa.hex"
    exit 1
fi

HEX="$1"
if [ ! -f "$HEX" ]; then
    echo "ERROR: firmware file not found: $HEX"
    exit 1
fi

echo "Flashing $HEX via RPi debug probe..."
openocd -f "$CFG" -c "program \"$HEX\" verify reset exit"
echo "Done."
