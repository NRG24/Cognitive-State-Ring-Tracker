#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CFG="$SCRIPT_DIR/rpi_debug_probe.cfg"

echo "Mass erasing nRF52832 via RPi debug probe..."
openocd -f "$CFG" -c "init; reset halt; nrf5 mass_erase; exit"
echo "Erase complete."
