# Rhythm Terminal Lite

Repository of Original Rhythm Terminal(alpha test ver.): https://github.com/MPAMlab/RhythmTerminal-OSS

This project provides a specialized firmware for an **ATmega32U4** based PCB integrated with a **PN5180 NFC module**. It is designed to act as a dual-purpose device: a high-performance NFC card reader and a 4x5 matrix joystick (gamepad).

This repository is based on CrazyRedMachine/PN5180-cardio: https://github.com/CrazyRedMachine/PN5180-cardio

Joystick part is based on https://github.com/nonefffds/joystick-atmega32u4 and https://github.com/MHeironimus/ArduinoJoystickLibrary

PN5180 NFC library for reading ISO14443, ISO15693, felica, is based on https://github.com/ATrappmann/PN5180-Library

Another similar project for integrate a PN5132 with rpi pico: https://github.com/MPAMlab/pico-spice-reader/

Warning: This repository includes AIGC contents.

**This Project is not finished and still under maintaince. Core Hardware design will be upload later.**

## Features

### 1. Multi-Protocol NFC Support
The firmware utilizes the PN5180's advanced capabilities to support a wide range of NFC standards:
- **FeliCa**: Supports Suica, Aime, and newer Banapassport cards.
- **ISO15693**: Supports older Banapassport cards.
- **ISO14443 (Mifare)**: Supports generic Type A cards.


### 2. 4x5 Matrix Joystick
The onboard 4x5 keyboard matrix is implemented as a **USB HID Gamepad (Joystick)** rather than a standard keyboard.
- **20 Discrete Buttons**: Each key in the matrix maps to a joystick button (1-20).
- **Custom Scanning**: Includes a debounced scanning routine (5-sample threshold) for high reliability.
- **Reactive LEDs**: The first row of the matrix features 5 dedicated LEDs that provide visual feedback when the corresponding keys are pressed.

### 3. Serial Diagnostics
Real-time logging is provided over USB Serial at **115200 baud**:
- Logs joystick button presses and releases.
- Logs successful NFC card detections with the full (and translated) ID.
- Suppresses polling noise to keep logs clean and actionable.

## Hardware Configuration (Inferred Pinout)

The firmware is pre-configured for a custom PCB with the following pin mapping to the ATmega32U4:

### PN5180 Interface (SPI + Control)
| Signal | ATmega32U4 Pin | Arduino Pin |
| :--- | :--- | :--- |
| **NSS** | PF0 | D23 |
| **BUSY** | PF1 | D22 |
| **RST** | PF4 | D21 |
| **SCK** | PB1 | D15 (ICSP-3) |
| **MOSI** | PB2 | D16 (ICSP-4) |
| **MISO** | PB3 | D14 (ICSP-1) |

Please notice, you should have a 5v-3.3v level converter on spi bus. 

### 4x5 Matrix Keypad
| Row / Col | ATmega32U4 Pin | Arduino Pin |
| :--- | :--- | :--- |
| **ROW 1** | PC6 | D5 |
| **ROW 2** | PD7 | D6 |
| **ROW 3** | PD4 | D4 |
| **ROW 4** | PD3 | D1 |
| **COL 1** | PF5 | D20 |
| **COL 2** | PF6 | D19 |
| **COL 3** | PF7 | D18 |
| **COL 4** | PE6 | D7 |
| **COL 5** | PC7 | D13 |

### Dedicated LEDs
| LED | ATmega32U4 Pin | Arduino Pin |
| :--- | :--- | :--- |
| **LED 1** | PB4 | D8 |
| **LED 2** | PB5 | D9 |
| **LED 3** | PB6 | D10 |
| **LED 4** | PB7 | D11 |
| **LED 5** | PD6 | D12 |

## Build Instructions

1. **Prerequisites**: 
   - Arduino IDE 1.8.x or later.
   - Core support for "Arduino Leonardo" (ATmega32U4).
   - Flash the Caterina Bootloader to the custom PCB.
2. **Library Inclusion**: 
   - The project includes a custom `Joystick` and `DynamicHID` implementation in the root folder to handle the Gamepad HID reports. Do not use the standard Arduino Joystick library if it conflicts.
3. **Configuration**: 
   - Review `Config.h` to enable/disable specific features like `WITH_ISO14443` or `WITH_LEDS`.
4. **Upload**: 
   - Set Board to **Arduino Leonardo**.
   - Compile and upload to the PCB.

## License

This project is using GPL-3.0 license for code to keep the 'Same license' condition of repository PN5180-cardio, CC BY-SA-4.0 license for the documentations, images and multimedia, and CERN-OHL-W license for the Hardware Design. You can use or modify this project as long as you follow the license.
