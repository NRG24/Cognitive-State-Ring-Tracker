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

* **Microcontroller / RF:** Raytac MDBT50Q-1MV2 (nRF52833) handling low-power processing and BLE transmission.
* **Biometrics (PPG):** MAX30102 sensor capturing Heart Rate, Heart Rate Variability (HRV), Blood Oxygen Saturation (SpO2), and die temperature.
* **Arousal Tracking (GSR):** LM324-based quad op-amp Galvanic Skin Response circuit (Grove GSR-compatible topology) to measure electrodermal activity and sympathetic nervous system activation.
* **Motion Filtering:** LSM6DSR 6-axis IMU (accelerometer + gyroscope) dedicated to detecting and filtering out motion noise from raw biometric streams.
* **Power Regulation:** TLV70018DDCR ultra-low-noise 1.8V LDO regulator.
* **Battery:** Curved LiPo cell (ring-form-factor).
* **PCB:** 2-layer flex PCB with components on both sides, 0402/0201 passives, manufactured and assembled by JLCPCB.

> **Prototyping note:** The `firmware/` directory contains sketches for both the final ring hardware (MDBT50Q / nRF52833) and the Seeed XIAO nRF52840 development board used during prototyping. The XIAO-based sketches (e.g. `gsr_heart_ble.ino`) use an external DS18B20 temperature sensor and standalone pulse/GSR sensor modules, while the ring PCB integrates the MAX30102 and a custom analog GSR circuit directly.

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
│   └── *.ino                  # Standalone test sketches
├── hardware/                   # Hardware design files
│   ├── NERVA RING-bom.csv      # JLCPCB bill of materials (source of truth)
│   ├── BOM.md                  # Formatted BOM reference
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
| MCU / BLE | Raytac MDBT50Q-1MV2 (nRF52833) | SMD Module | 1 |
| PPG Sensor | MAX30102EFD+T | OESIP-14 | 1 |
| IMU | LSM6DSRTR (LSM6DSR) | LGA-14 | 1 |
| GSR Op-Amp | LM324PWR | TSSOP-14 | 1 |
| LDO Regulator | TLV70018DDCR | SOT-23-5 | 1 |
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

Some older design documents still reference parts from earlier board revisions. The CSV BOM (`hardware/NERVA RING-bom.csv`) and this README reflect the **actual current PCB** as ordered from JLCPCB. Key differences from older docs:

| What | Older Docs Say | Actual (Current PCB) |
|------|----------------|----------------------|
| MCU Module | MDBT42V-512KV2 (nRF52832) | MDBT50Q-1MV2 (nRF52833) |
| IMU | LSM6DS3 | LSM6DSR (LSM6DSRTR) |
| Op-Amp | TLV9004 (UQFN-14) | LM324PWR (TSSOP-14) |
| LDO | MCP1700T-3302E (3.3V) | TLV70018DDCR (1.8V) |

The `SCHEMATIC_CHECKLIST.md`, `PCB_LAYOUT_STRATEGY.md`, and `GSR_CIRCUIT_INTEGRATION.md` have not yet been updated to match the current BOM. The firmware docs in `firmware/gsr_heart_ble/` describe the XIAO nRF52840 prototyping setup, not the final ring hardware.

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
