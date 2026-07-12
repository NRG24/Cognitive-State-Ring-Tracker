# Power Management — BQ25120A

Firmware requirements for the BQ25120A PMIC on the next PCB revision (see
`hardware/SCHEMATIC_DECISIONS.md`). These are hard requirements, not
suggestions — getting them wrong either bricks charging behavior or collapses
the 3.3V rail at runtime.

## I2C Watchdog

The BQ25120A has a **non-disableable** ~50 second I2C watchdog timer. If it
isn't serviced, it silently resets all registers to power-on-reset defaults —
including `SYS_VOUT`, which collapses the regulated 3.3V rail. This failure
mode is silent: there's no interrupt or flag, the rail just drops.

A dedicated task must service the watchdog on an interval comfortably under
50s (recommend 20–30s) for the lifetime of the device being powered on.

## Mandatory Boot Order

On every boot, before anything else touches the BQ25120A:

1. **Disable TS_EN.** The TS (temperature sense) pin is left unrouted on the
   PCB (unroutable at 0.4mm BGA pitch — see `hardware/SCHEMATIC_DECISIONS.md`),
   so the battery temperature qualification must be disabled in firmware via
   register `0x02`, bit 7. This must be the very first I2C transaction after
   boot — the chip will otherwise fault on a floating TS input.
2. **Set `SYS_VOUT` to 1.9V.**
3. **Start the watchdog service task.**

Do not reorder these steps. In particular, do not start the watchdog task
before TS_EN is disabled — a chip fault on TS during that window can prevent
the subsequent register writes from taking effect.

---
*Last updated: 2026-07-10*
