# ∇Calc (NablaCalc)

A handheld scientific calculator built around an ESP32-S3, featuring a 2.2-inch SPI display, an OV5640 camera for equation capture, and a custom 4-layer PCB inside a 3D-printed enclosure.

![NablaCalc virtual enclosure assembly](hardware/assets/full_case_assembly.png)

## Why I built this

Phones have calculator apps, but pulling out a phone while studying is an immediate trap for notifications and doomscrolling. On the flip side, commercial scientific calculators are overpriced and feel stuck in the early 2000s—slow microcontrollers, proprietary cables, and no modern conveniences.

I wanted to build a dedicated math device that actually belongs on a student's desk: physical tactile keys, modern open hardware (ESP32-S3, USB-C, SPI display), and a discreet camera to snap equations straight out of textbooks or notebooks without needing a phone.

## Current Status

This is a complete hardware design ready for first prototype fabrication:
- **PCB:** Custom 4-layer board (84 × 85 mm) designed in EasyEDA Pro, fully routed and DRC-verified.
- **Enclosure:** Multi-part 3D model designed in Onshape with a sliding camera shutter and protective cover.
- **Firmware:** ESP-IDF baseline with basic I2C and SPI peripheral drivers.
- **Keyboard:** The motherboard includes the matrix routing and MCP23017 expander for a 7×7 layout; the physical keymat and tactile membrane will be built and tested once the prototype parts arrive.

## Hardware Overview

- **MCU:** ESP32-S3-WROOM-1-N16R8 (16 MB Flash, 8 MB PSRAM)
- **Display:** 2.2-inch 320×240 TFT LCD (ILI9341 over SPI)
- **Camera:** 5 MP OV5640 autofocus module (DVP) behind a mechanical privacy slider
- **Power:** USB-C charging via TP4056 with a TPS63001 buck-boost converter for the 3.3V rail
- **Storage:** MicroSD slot (SPI)
- **Inputs:** 7×7 matrix routed to an MCP23017 I2C expander (tactile membrane in progress)

## First Build & Funding

I am going with the JLC Top PCBA because I do not have experience soldering the small parts myself. Their minimum gives me five boards and two Top assemblies. I will do the Bottom side later.

| Item | Price (USD) |
|---|---:|
| JLC Top PCBA: 5 PCBs + 2 Top assemblies, with the ESP32-S3 | 86.18 |
| Bottom components for the two boards | 4.43 |
| Battery, camera, display, FPC cables and screws | 24.51 |
| **Funding request, without shipping or taxes** | **115.12** |

The parts list, links, and price evidence are in [external parts](hardware/procurement/NablaCalc_External_Parts.csv).

## Quick Links

- [EasyEDA PCB Source](hardware/pcb/source/NablaCalc_PCB.epro2) & [Schematic](hardware/pcb/source/NablaCalc_Schematic.epro2)
- [Fabrication Gerbers & BOM](hardware/pcb/)
- [Onshape 3D Assembly (2.2" Display)](mechanical/onshape_exports/NablaCalc_Assembly_2.2in_Display.step)
- [Firmware Source](firmware/nabla_firmware/)
- [External Parts List](hardware/procurement/NablaCalc_External_Parts.csv)
