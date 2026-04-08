#!/bin/bash
openocd -f ~/nrf52_workspace/rpi_debug_probe.cfg -c "init; reset halt; nrf5 mass_erase; exit"
