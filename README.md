# ∇Calc / NablaCalc

**A dedicated scientific calculator with a physical 7×7 keyboard, ESP32-S3, local storage, 2.2-inch SPI display and a bonus OV5640 camera for capturing equations.**

![NablaCalc virtual enclosure assembly](hardware/assets/full_case_assembly.png)

![NablaCalc logo](hardware/assets/nablacalc_silkscreen_logo.png)

> **Project stage:** complete virtual hardware design prepared for prototype funding. The `PCB` board, enclosure CAD and base firmware exist; physical operation is not claimed until the first funded prototype is assembled and tested.

## Why I built it

Cell phones can solve equations, but they’re distracting and aren’t really math tools or anything like that. The ∇Calc explores a different product: a real handheld scientific calculator with physical keys, adding modern features only when requested. Its camera stays protected behind a mechanical cover and is meant for occasional equation/OCR capture, not continuous recording. Camera setups designed for other recording applications heat up and need a proper cooling system, maybe some space—usually a dissipation pad. Here, that’s not necessary.

The goal of the design is a compact, manufacturable device, with electronics, power path, camera interface, keyboard, and casing designed together. And the bonuses and improvements that current technology offers. I’m also thinking of using some kind of TinyModel, more like OCR, to see if I can turn this into an edge IoT device...

## What is included

- Four-layer, **84 × 85 mm** custom main PCB built around an `ESP32-S3-WROOM-1-N16R8`.
- USB-C native data on GPIO19/20, LiPo charging and a `TPS63001` 3.3 V buck-boost supply.
- Physical 7×7 keyboard scanned through an `MCP23017` I2C expander.
- Shared SPI interfaces for the display and microSD storage.
- RAW `OV5640` 5 MP autofocus DVP camera interface with four local rails, SCCB level translation and DVP level shifters.
- A 24-pin, 0.5 mm `FH12-24S-0.5SH(55)` FPC camera connector.
- Parametric enclosure parts, a camera privacy slider and full STEP assemblies. The type of material and membrane will still be better studied and properly structured for the build once we have the components.
- ESP-IDF firmware baseline with NVS, I2C and keyboard scanning implemented.

## First build / prices

I am going with the JLC Top PCBA because I do not have experience soldering the
small parts myself. Their minimum gives me five boards and two Top assemblies.
I will do the Bottom side later.

| Item | Price (USD) |
|---|---:|
| JLC Top PCBA: 5 PCBs + 2 Top assemblies, with the ESP32-S3 | 86.18 |
| Bottom components for the two boards | 4.43 |
| Battery, camera, display, FPC cables and screws | 24.51 |
| **Funding request, without shipping or taxes** | **115.12** |

Shipping and taxes are not included. The parts list, links and price evidence
are in [external parts](hardware/procurement/NablaCalc_External_Parts.csv).

I will decide the keyboard contact material, microSD and case printing after I
have the real modules in hand. If I decide to solder everything myself instead,
the other option is about US$45.12.

## Quick Links

- [Complete purchase BOM](BOM.csv)
- [JLC/LCSC-only PCB component BOM](hardware/procurement/NablaCalc_JLC_LCSC_Only_BOM.csv)
- [External parts list](hardware/procurement/NablaCalc_External_Parts.csv)
- [Editable EasyEDA PCB source](hardware/pcb/source/NablaCalc_PCB.epro2)
- [Editable EasyEDA schematic source](hardware/pcb/source/NablaCalc_Schematic.epro2)
- [Current 2.2-inch-display mechanical assembly](mechanical/onshape_exports/NablaCalc_Assembly_2.2in_Display.step)
- [Earlier complete mechanical assembly](mechanical/onshape_exports/NablaCalc_Assembly_Legacy.step)
- [ESP32-S3 pin map](firmware/nabla_firmware/main/board_pins.h)
- [Main firmware entry point](firmware/nabla_firmware/main/nabla_firmware.c)

## Some things still to do

- I still need to build and test the real prototype. For now this is the full virtual design for funding.
- The 2.2-inch display is the one I am getting. I will adjust the case better when I have the real parts here. Some old 2.4-inch files are still in the repo history.
- OCR and AI solving are ideas for later, after the calculator itself is working.
