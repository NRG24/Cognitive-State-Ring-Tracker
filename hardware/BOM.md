# NERVA Ring - Bill of Materials

> **Source of truth:** [`NERVA RING-bom.csv`](NERVA%20RING-bom.csv) is the JLCPCB export. This document is a formatted reference.

## Order Summary

| Item | Description | Qty | Cost (USD) | Supplier |
|------|-------------|-----|------------|----------|
| Curved LiPo Battery | Ring-type rechargeable Li-Ion cell for wearable devices | 5 | $27.00 | Alibaba |
| Flex PCB + Assembly | 2-layer flex PCB with SMT assembly (all components except battery) | 2 | $160.60 | JLCPCB |
| Raytac MDBT42V-P | MCU module pre-order for PCB assembly (earlier revision, see note below) | 2 | $11.06 | JLCPCB Pre-Order |
| | **Total** | | **$198.66** | |

> **Note on MCU:** The MDBT42V-P was pre-ordered separately, but the PCBA parts list below shows the **MDBT50Q-1MV2** as the module actually placed on the board. The design evolved from nRF52832 (MDBT42V) to nRF52833 (MDBT50Q) during development.

## PCBA Parts List

Components placed by JLCPCB assembly service.

### Active Components (ICs)

| Ref | Part Number | Description | Package | Qty | Manufacturer | LCSC | Unit Price |
|-----|-------------|-------------|---------|-----|-------------|------|------------|
| U10 | MDBT50Q-1MV2 | BLE MCU module (nRF52833) | SMD Module | 1 | Raytac | C5118826 | $1.02 |
| U1 | MAX30102EFD+T | PPG sensor (HR, SpO2, temp) | OESIP-14 (5.6x3.3mm) | 1 | Analog Devices / Maxim | C6454833 | $1.79 |
| U4 | LSM6DSRTR | 6-axis IMU (accel + gyro) | LGA-14 (3.0x2.5mm) | 1 | STMicroelectronics | C784817 | $0.41 |
| U5 | LM324PWR | Quad op-amp (GSR circuit) | TSSOP-14 (5.0x4.4mm) | 1 | Texas Instruments | C7942 | $0.03 |
| U7 | TLV70018DDCR | 1.8V LDO regulator | SOT-23-5 (3.0x1.7mm) | 1 | Texas Instruments | C79924 | $0.02 |

### Passive Components

#### Capacitors

| Ref | Value | Designators | Package | Qty | Notes |
|-----|-------|-------------|---------|-----|-------|
| C6-C7, C9, C12, C14-C15, C_VDD1, C_VDDH1 | 100nF | Bypass / decoupling | 0402 | 8 | |
| C8, C17 | 4.7uF | Bulk decoupling | 0603 | 2 | |
| C11, C13, C16, C_VDD, C_VDDH | 10uF | Bulk decoupling | 0603 | 5 | |
| CR1, CR2, CR3 | 100nF | GSR circuit | 0402 | 3 | |

**Total capacitors: 18**

#### Resistors

| Ref | Value | Designators | Package | Qty | Notes |
|-----|-------|-------------|---------|-----|-------|
| R1, R2, R5 | 4.7k | I2C pull-ups / general | 0402 | 3 | |
| R6 | 10K | General | 0402 | 1 | |
| RG1, RG2 | 4.7K | GSR circuit | 0402 | 2 | |
| RG3, RG4, RG8, RG9 | 200K | GSR feedback | 0402 | 4 | |
| RG5 | 100K | GSR circuit | 0402 | 1 | |
| RG6, RG7 | 100k | GSR circuit | 0402 | 2 | |
| RG10 | 1M | GSR output | 0402 | 1 | |

**Total resistors: 14**

#### Inductors

| Ref | Value | Designator | Package | Qty | Notes |
|-----|-------|------------|---------|-----|-------|
| L1 | 10uH | L1 | 0603 | 1 | DCDC circuit |

**Total inductors: 1**

### Connectors & Test Points

| Ref | Part | Designators | Qty | Notes |
|-----|------|-------------|-----|-------|
| H4 | PZ254V-11-02P | H4 | 1 | 2-pin header (battery connector), LCSC C492401 |
| BAT+, BAT- | Test-Point | BAT+, BAT- | 2 | Battery connection test points (2mm) |
| TP_VDD | Test-Point | TP_VDD | 1 | Power rail test point (0.5mm) |

## Component Count Summary

| Category | Count |
|----------|-------|
| ICs | 5 |
| Capacitors | 18 |
| Resistors | 14 |
| Inductors | 1 |
| Connectors | 1 |
| Test Points | 3 |
| **Total** | **42** |

## Discrepancies With Other Documentation

The following hardware docs reference parts from earlier board revisions and have not been updated:

| Document | References | Actual Part on PCB |
|----------|------------|--------------------|
| `SCHEMATIC_CHECKLIST.md` | MDBT42V (nRF52832) | MDBT50Q-1MV2 (nRF52833) |
| `SCHEMATIC_CHECKLIST.md` | LSM6DS3 | LSM6DSR (LSM6DSRTR) |
| `SCHEMATIC_CHECKLIST.md` | MCP1700T-3302E (3.3V LDO) | TLV70018DDCR (1.8V LDO) |
| `SCHEMATIC_CHECKLIST.md` | TLV9004 quad op-amp | LM324PWR |
| `PCB_LAYOUT_STRATEGY.md` | MDBT42V-512KV2, TLV9004IRUCR, LSM6DS3, MCP1700T-3302E | See above |
| `GSR_CIRCUIT_INTEGRATION.md` | TLV9004IRUCR (UQFN-14), 0201 passives | LM324PWR (TSSOP-14), 0402 passives |
| `GSR_CIRCUIT_INTEGRATION.md` | RG1 = 100kΩ | RG1 = 4.7K (per CSV BOM) |
| `GSR_CIRCUIT_INTEGRATION.md` | 100µF bulk cap (0603/0805) | No 100µF cap in BOM; largest is 10µF |
