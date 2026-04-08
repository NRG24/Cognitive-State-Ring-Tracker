#!/bin/bash
if [ $# -eq 0 ]; then
    echo "Usage: ./flash.sh firmware.hex"
    exit 1
fi
openocd -f ~/nrf52_workspace/rpi_debug_probe.cfg -c "program $1 verify reset exit"
