# NERVA Ring

An open-source, sensor-packed smart ring designed to monitor cognitive health and autonomic nervous system metrics rather than traditional fitness tracking.

## Project Overview

The NERVA Ring shifts the focus of wearable technology away from step-counting and workouts toward real-time mental well-being. By embedding precise biosensors into a compact, flexible PCB form factor, the device continuously captures physiological indicators tied directly to stress, mental fatigue, and psychological arousal.

## Core Features

* **Cognitive Focus:** Tailored strictly for mental wellness monitoring rather than physical fitness or gym tracking.
* **Rigorous Hardware Validation:** Circuitry completely modeled, tested, and validated via LT-Spice to guarantee electrical correctness.
* **Ultra-Compact Layout:** All hardware components optimized to fit onto a highly constrained, circular flex PCB architecture.
* **Wireless Telemetry:** Real-time, low-latency data streaming to mobile devices using Bluetooth Low Energy (BLE).
* **Intelligent Software Pipeline:** Dynamic in-app data filtering to remove motion artifacts and anomalies, powering a custom algorithm that delivers a "Cognitive Score" out of 100 alongside meaningful behavioral insights.

## Hardware Architecture

* **Microcontroller / RF:** Raytac MDBT42V (nRF52832) handling low-power processing and BLE transmission.
* **Biometrics (PPG):** MAX30102 sensor capturing Heart Rate, Heart Rate Variability (HRV), Blood Oxygen Saturation (SpO2), and die temperature.
* **Arousal Tracking (GSR):** LM324-based quad op-amp Galvanic Skin Response circuit (Grove GSR-compatible topology) to measure electrodermal activity and sympathetic nervous system activation.
* **Motion Filtering:** LSM6DSR 6-axis IMU (accelerometer + gyroscope) dedicated to detecting and filtering out motion noise from raw biometric streams.
* **Power Regulation:** TLV70018DDCR ultra-low-noise 1.8V LDO regulator.
* **GSR Electrodes:** Custom pogo-pin electrodes for skin contact.
* **Charging:** Two-pin magnetic pogo charger connector (Alibaba).
* **Battery:** Curved LiPo cell (ring-form-factor).
* **PCB:** 2-layer flex PCB with components on both sides, 0402/0201 passives, manufactured and assembled by JLCPCB.

> **Prototyping note:** The `firmware/` directory contains sketches for both the final ring hardware (MDBT42V / nRF52832) and the Seeed XIAO nRF52840 development board used during prototyping. The XIAO-based sketches (e.g. `gsr_heart_ble.ino`) use an external DS18B20 temperature sensor and standalone pulse/GSR sensor modules, while the ring PCB integrates the MAX30102 and a custom analog GSR circuit directly.

## Project Structure

```
NERVA_Ring/
├── app/                        # Flutter mobile app ("GSR Streamer")
│   ├── lib/
│   │   ├── main.dart           # App entry point
│   │   ├── ble_service.dart    # BLE communication layer
│   │   ├── cognitive_scorer.dart
│   │   ├── gsr_analyzer.dart
│   │   ├── models/             # Data models
│   │   ├── screens/            # UI screens
│   │   ├── services/           # Business logic services
│   │   └── widgets/            # Reusable UI components
│   └── pubspec.yaml
├── firmware/                   # Arduino firmware sketches
│   ├── gsr_heart_ble/          # Main firmware + docs
│   │   ├── gsr_heart_ble.ino   # BLE biometric streaming (XIAO prototype)
│   │   └── *.ino              # Variant sketches (power-optimized, all-features, etc.)
│   ├── simple_ble/             # Minimal BLE test
│   ├── POWER_MANAGEMENT.md     # BQ25120A boot sequence & watchdog requirements
│   └── *.ino                  # Standalone test sketches
├── hardware/                   # Hardware design files
│   ├── NERVA RING-bom.csv      # JLCPCB bill of materials (source of truth)
│   ├── BOM.md                  # Formatted BOM reference
│   ├── SCHEMATIC_DECISIONS.md  # Locked decisions for the next PCB revision
│   ├── gsr_wearable.kicad_sch  # KiCad schematic
│   ├── gsr_wearable.kicad_pro  # KiCad project
│   ├── Ring CAD.step           # 3D CAD model
│   ├── pcb/                    # PCB layout files
│   ├── SCHEMATIC_CHECKLIST.md  # Design review checklist
│   ├── GSR_CIRCUIT_INTEGRATION.md
│   ├── PCB_LAYOUT_STRATEGY.md
│   └── *.md                   # Additional design docs
├── docs/                       # Software architecture docs
│   ├── ACTIVITY_RECOGNITION.md
│   ├── STRESS_PATTERN_ANALYSIS.md
│   ├── HAPTIC_NOTIFICATIONS.md
│   └── *.md
└── scripts/                    # Build/utility scripts
```

## Bill of Materials (Summary)

See [`hardware/BOM.md`](hardware/BOM.md) for the complete, formatted BOM. See [`hardware/NERVA RING-bom.csv`](hardware/NERVA%20RING-bom.csv) for the raw JLCPCB export.

| Component | Part | Package | Qty |
|-----------|------|---------|-----|
| MCU / BLE | Raytac MDBT42V (nRF52832) | SMD Module | 1 |
| PPG Sensor | MAX30102EFD+T | OESIP-14 | 1 |
| IMU | LSM6DSRTR (LSM6DSR) | LGA-14 | 1 |
| GSR Op-Amp | LM324PWR | TSSOP-14 | 1 |
| LDO Regulator | TLV70018DDCR | SOT-23-5 | 1 |
| GSR Electrodes | Custom pogo pins | — | 2 |
| Charger Connector | 2-pin magnetic pogo | — | 1 |
| Battery | Curved ring-type LiPo | Custom | 1 |

## Sensors & Data Streams

| Metric | Sensor | BLE UUID (suffix) | Type |
|--------|--------|--------------------|------|
| GSR (Skin Conductance) | Analog circuit → ADC | `...0002` | Int |
| Heart Rate | MAX30102 PPG | `...0003` | Int |
| Temperature | MAX30102 die temp | `...0004` | Float |
| HRV (RMSSD) | Derived from PPG | `...0005` | Float |
| SpO2 | MAX30102 PPG | `...0006` | Int |
| Battery Level | Voltage divider → ADC | `...0007` | Int |

**BLE Service UUID:** `19B10000-E8F2-537E-4F6C-D104768A1214`

## Known Documentation Inconsistencies

Some design documents still reference parts from earlier board revisions. This README reflects the actual current hardware. Key differences in older docs:

| What | Stale Docs Say | Actual (Current PCB) |
|------|----------------|----------------------|
| IMU | LSM6DS3 | LSM6DSR (LSM6DSRTR) |
| Op-Amp | TLV9004 (UQFN-14) | LM324PWR (TSSOP-14) |
| LDO | MCP1700T-3302E (3.3V) | TLV70018DDCR (1.8V) |

The JLCPCB BOM CSV (`hardware/NERVA RING-bom.csv`) lists MDBT50Q-1MV2 for the MCU slot, but the actual module used is the **MDBT42V** (nRF52832). The `SCHEMATIC_CHECKLIST.md`, `PCB_LAYOUT_STRATEGY.md`, and `GSR_CIRCUIT_INTEGRATION.md` have not yet been fully updated. The firmware docs in `firmware/gsr_heart_ble/` describe the XIAO nRF52840 prototyping setup, not the final ring hardware.

## Next Revision (In Progress)

Work is underway on the next PCB revision. Nothing in this section has been
fabricated yet — it's schematic-stage only. Full detail also lives in
[`hardware/SCHEMATIC_DECISIONS.md`](hardware/SCHEMATIC_DECISIONS.md) and
[`firmware/POWER_MANAGEMENT.md`](firmware/POWER_MANAGEMENT.md).

### Microcontroller / RF

Switched from the Raytac MDBT42V (nRF52832) to a **u-blox ANNA-B402
(nRF52833)**. This is now designator **U5** on the schematic (the MDBT42V
module was U10 on the current, as-built BOM). All `U5.*` pin references below
refer to the ANNA-B402.

### GSR Front End (OPA2333 Transimpedance Amplifier)

Replaces the LM324-based Grove-style GSR circuit used on the current PCB.

* **Op-amp:** OPA2333, configured as a TIA.
  * Rf = 91kΩ
  * Cf = 330nF
  * R_lpf = 100kΩ **(not 1MΩ)** — required to meet the ANNA-B402's (nRF52833) SAADC source impedance limit on the ADC input.
  * C_lpf = 1µF
* **DC excitation:** 0.5V, generated by a 560kΩ / 330kΩ divider off the 3.3V rail.
* **Baseline trim:** the physical trim-pot was removed; baseline offset is now tracked in firmware instead.
* **Supply:** OPA2333 VCC ties directly to LS_3V3, gated via the BQ25120A's `EN_LS_LDO` over I2C (not a dedicated GPIO).
* **ADC decoupling cap:** relocated to MCU pin U5.24, at the MCU input rather than at the op-amp output.

### Power Management (BQ25120A)

* **TS pin:** left NC. The C3 ball is fully interior at 0.4mm BGA pitch with no edge-adjacent neighbors, so it's unroutable without HDI. Battery temp-sense is disabled entirely via firmware instead.
* **VINLS** (balls B4/C4): tied to PMID, not LS_3V3.
* **I2C watchdog:** the BQ25120A has a non-disableable ~50s I2C watchdog. If it isn't serviced, it silently resets all registers to power-on-reset defaults — including `SYS_VOUT`, which collapses the regulated 3.3V rail. A dedicated firmware task must service the watchdog on an interval comfortably under 50s (recommend 20–30s).
* **Mandatory boot order:**
  1. Disable TS_EN via register `0x02`, bit 7 — must be the very first I2C transaction after boot, since the chip will otherwise fault on the floating TS input.
  2. Set `SYS_VOUT` to 1.9V.
  3. Start the watchdog service task.

### Sensor Interrupts

* **LSM6DSR INT1** → MCU `ACC_INT`, pin U5.36. Push-pull output; no external pull-up needed (PP_OD = 0 default). INT2 left unconnected.
* **MAX30102 INT** → MCU GPIO_36. Using the MCU's internal pull-up here is acceptable — this line is lower-criticality than the I2C bus.

### I2C Bus

External 4.7kΩ pull-ups to 1.8–1.9V SYS on both SDA and SCL, on every bus. The MCU's internal ~13kΩ pull-ups are too weak for reliable 400kHz operation.

### ESD Protection

Low-capacitance TVS diodes added on the exposed GSR electrode pads. **Status: tentative, TBD** — not yet finalized.

### Mechanical

Electrode contacts are partial-arc for now (not full-ring contacts).

### Tooling Notes

The EasyEDA MCP bridge's `connect_pin_to_net` / `connect_pins_by_net` tools report success but do not actually write changes. Treat them as read-only. All schematic edits go through the EasyEDA UI manually.

## Getting Started

### Mobile App
```bash
cd app
flutter pub get
flutter run
```

### Firmware (Prototype with XIAO nRF52840)
1. Install Arduino IDE with Seeed nRF52 board support
2. Install libraries: `ArduinoBLE`, `OneWire`, `DallasTemperature`
3. Wire sensors per `firmware/gsr_heart_ble/SETUP_INSTRUCTIONS.md`
4. Upload `firmware/gsr_heart_ble/gsr_complete_all_features.ino`

### Hardware
KiCad project files are in `hardware/`. The PCB is a 2-layer flex design assembled by JLCPCB.

---

https://github.com/user-attachments/assets/21d7508c-1ee5-49b8-9717-f8bbbc1bc319

<img width="1526" height="1260" alt="image" src="https://github.com/user-attachments/assets/9e2d3639-5bc1-4c2b-b5d9-fed1e8b23c1c" />

<img width="1280" height="1024" alt="image" src="https://github.com/user-attachments/assets/bb193762-28db-4391-b3c4-8594ef432642" />

<img width="436" height="368" alt="Screenshot 2026-02-07 at 9 29 02 PM" src="https://github.com/user-attachments/assets/2e2c0f2e-898e-42cc-9a81-37765b89f813" />
<br>

<img width="1077" height="370" alt="Screenshot 2026-03-21 at 9 27 09 PM" src="https://github.com/user-attachments/assets/76b5d239-362e-46cb-b721-c6f8a9353247" />
<img width="718" height="494" alt="Screenshot 2026-02-11 at 9 21 46 PM" src="https://github.com/user-attachments/assets/ed82f334-0ea8-4d68-94f6-a0611016e22b" />
<img width="1101" height="417" alt="Screenshot 2026-03-21 at 9 28 39 PM" src="https://github.com/user-attachments/assets/bfdef878-dc1f-4e6c-975c-ed55c9a32da5" />
