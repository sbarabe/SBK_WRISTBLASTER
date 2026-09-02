# SBK_WRISTBLASTER_CORE

Arduino firmware for controlling the lights, sound effects, switches, indicators, and animation sequences of the SBK Wrist Blaster replica or other custom props.

## Current Release

**Version 2.0.0**

This release migrates the firmware to the new 2.0 engine architecture:

- Reworked the core state machine, configuration, and pin definitions.
- Added reusable Cyclotron and Power Cell animation schemes.
- Replaced the legacy switch handling with the `SBK_Button` library.
- Replaced bundled bar-meter drivers with the `SBK_BarDrive`, `SBK_MAX72xx`, and `SBK_HT16K33` libraries.
- Replaced the external DFPlayer dependency with the bundled lightweight player engine.
- Updated the LED-strip, vent, firing-rod, indicator, smoke, battery-monitoring, and Power Cell engines.

The firmware retains the features introduced in previous versions, including:

- Support for multiple SBK PCB configurations and custom pin assignments
- DFPlayer Mini sound playback
- WS2812 lighting effects and animation sequences
- Panel bar-meter support using either a MAX7219/MAX7221 or HT16K33 driver
- Configurable 28-segment bar-meter mapping and animation direction
- Optional Power Cell using either:
  - An addressable WS2812 LED strip
  - A 24-segment bar meter
- Optional battery-voltage monitoring on supported boards
- Configurable switches, buttons, potentiometers, and indicator outputs
- Serial debugging
- Compile-time configuration checks

Most users should only need to modify `SBK_WRISTBLASTER_CONFIG.h`.

The main file, `SBK_WRISTBLASTER_CORE.ino`, contains the state machine and core operating logic and should normally remain unchanged.

## Supported Hardware Configurations

The following predefined pin configurations are available:

- `SBK_WRIST_BLASTER_PCB_V2`
- `SBK_WRIST_BLASTER_II_PCB_Vx`
- `SBK_PROPCORE_ONE_Vx`
- `SBK_PROPCORE_ONE_PLUS_Vx`
- `CUSTOM_PINS_DEFINITION`

The firmware supports the Arduino Nano Every and ATmega328-based Nano targets, subject to the enabled feature set and available memory.

Exactly one hardware configuration must be selected in `SBK_WRISTBLASTER_CONFIG.h`.

## Required Arduino Libraries

Install the following libraries using the Arduino IDE Library Manager:

- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
- [SBK Button](https://github.com/sbarabe/SBK_Button)
- [SBK BarDrive](https://github.com/sbarabe/SBK_BarDrive)
- [SBK MAX72xx](https://github.com/sbarabe/SBK_MAX72xx) when using a MAX72xx bar-meter driver
- [SBK HT16K33](https://github.com/sbarabe/SBK_HT16K33) when using an HT16K33 bar-meter driver

The firmware also uses the standard Arduino `Wire` and `SoftwareSerial` libraries.

The project-specific engine files are included in the `SBK_WristBlaster_lib_V2_0_0` folder.

## Installation

1. Download or clone the complete [SBK_WRISTBLASTER repository](https://github.com/sbarabe/SBK_WRISTBLASTER).

2. Copy the `SBK_WRISTBLASTER_CORE` folder into your Arduino Sketchbook folder.

3. Copy the included `SBK_WristBlaster_lib_V2_0_0` folder into your Arduino Sketchbook `libraries` folder.

4. Install the required external Arduino libraries listed above.

5. Open `SBK_WRISTBLASTER_CORE.ino` in the Arduino IDE.

6. Open `SBK_WRISTBLASTER_CONFIG.h` and select the correct PCB configuration:

   ```cpp
   // Define exactly one:
   // #define CUSTOM_PINS_DEFINITION
   // #define SBK_WRIST_BLASTER_PCB_V2
   // #define SBK_WRIST_BLASTER_II_PCB_Vx
   #define SBK_PROPCORE_ONE_Vx
   // #define SBK_PROPCORE_ONE_PLUS_Vx

7. Work through the configuration file and adjust the available options to match your build, including:

   - Switch and button types and logic
   - Panel bar-meter driver type, segment mapping, and animation direction
   - Smoke-effect support—currently a beta feature that has not yet been tested with a real smoke device
   - Volume potentiometer support
   - Firing-rod hue potentiometer support
   - Power Cell type and configuration, if installed
   - Battery-voltage sensing and low-voltage protection options
   - LED indexes and animation directions
   - Sound-track durations and audio timing
   - Serial debugging
