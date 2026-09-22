# ∇Calc

A handheld scientific calculator built around an ESP32-S3, featuring a 2.2-inch SPI display, an OV5640 camera for snapping equations, and a custom 4-layer PCB inside a 3D-printed case.

![NablaCalc virtual enclosure assembly](hardware/assets/full_case_assembly.png)

## Why I built this

Phones have calculator apps, but pulling out a phone while studying is an immediate distraction. On the other hand, traditional scientific calculators are expensive and lack the modern features and tech we have today.

I wanted to make a dedicated desk tool with modern open hardware (ESP32-S3, USB-C, SPI screen) and physical keys. The camera is tucked behind a sliding shutter just to snap math problems from textbooks or notes when needed.

## Current Status

This is a complete hardware design ready for prototype fabrication:
- **PCB:** Custom 4-layer board (84 × 85 mm) designed in EasyEDA Pro, fully routed and DRC-clean.
- **Case:** Multi-part 3D model designed in Onshape with a sliding camera shutter.
- **Keyboard:** The PCB has the matrix routing and MCP23017 expander for a 7×7 layout. The physical tactile membrane and keymat will be built once the components arrive.
- **Firmware:** ESP-IDF base with I2C and SPI peripheral setup.

## Hardware

- **MCU:** ESP32-S3-WROOM-1-N16R8 (16 MB Flash, 8 MB PSRAM)
- **Display:** 2.2-inch 320×240 TFT LCD (ILI9341 via SPI)
- **Camera:** 5 MP OV5640 autofocus module (DVP) with a mechanical cover
- **Power:** USB-C charging with TP4056 and TPS63001 buck-boost (3.3V rail)
- **Storage:** MicroSD slot (SPI)
- **Keypad:** 7×7 matrix via MCP23017 I2C expander

## First build / prices

I am going with the JLC Top PCBA because I do not have experience soldering the small parts myself. Their minimum gives me five boards and two Top assemblies. I will do the Bottom side later.

| Item | Price (USD) |
|---|---:|
| JLC Top PCBA: 5 PCBs + 2 Top assemblies, with the ESP32-S3 | 86.18 |
| Bottom components for the two boards | 4.43 |
| Battery, camera, display, FPC cables and screws | 24.51 |
| **Funding request, without shipping or taxes** | **115.12** |

The parts list, links and price evidence are in [external parts](hardware/procurement/NablaCalc_External_Parts.csv).

## Links

- [EasyEDA PCB Source](hardware/pcb/source/NablaCalc_PCB.epro2) & [Schematic](hardware/pcb/source/NablaCalc_Schematic.epro2)
- [Fabrication Gerbers & BOM](hardware/pcb/)
- [Onshape 3D Assembly](mechanical/onshape_exports/NablaCalc_Assembly_2.2in_Display.step)
- [Firmware Source](firmware/nabla_firmware/)
