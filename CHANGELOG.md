# Changelog

All notable firmware changes to the SBK Wrist Blaster project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and release versions follow [Semantic Versioning](https://semver.org/).

## 2.0.0 - 2026-09-02

### Added

- Reusable Cyclotron and Power Cell animation-scheme headers.
- Support for the external `SBK_Button`, `SBK_BarDrive`, `SBK_MAX72xx`, and `SBK_HT16K33` libraries.
- A lightweight bundled DFPlayer command engine, removing the need for `DFPlayerMini_Fast`.
- Arduino Nano Every and ATmega328-based Nano build support, subject to the selected features and available memory.

### Changed

- Reworked the main firmware state machine, configuration, and hardware pin definitions.
- Updated the battery-monitoring, Cyclotron, indicator, LED-strip base, player, Power Cell, firing-rod, smoke, and vent engines to version 2.0.0.
- Replaced the legacy bundled switch engine with `SBK_Button`.
- Replaced the legacy bundled bar-meter engines and drivers with the standalone SBK bar-meter libraries.
- Renamed the bundled engine directory to `SBK_WristBlaster_lib_V2_0_0`.
- Updated installation and dependency documentation for the 2.0 architecture.

### Removed

- Legacy `V1_1_0` switch, bar-meter, MAX72xx, and HT16K33 implementations from the active firmware bundle.
- The external `DFPlayerMini_Fast` dependency.

### Archived

- Preserved the complete 1.1.0 firmware release under `Previous firmware and sounds FX releases/V1_1_0`.

## 1.1.0 - 2026-08-27

### Added

- `STATE_ALL_ON_TO_OFF` for smoother sound and animation transitions.
- A configuration flag for enabling or disabling smoke-device support.
- Additional and revised sound tracks for the updated operating sequence.

### Changed

- Revised power-up, Cyclotron, firing, overheat, venting, and shutdown sequences.
- Improved smoke-device safety logic.
- Strengthened Cyclotron pixel-index validation.

### Fixed

- Several state-machine transitions.
- Firing-rod animation behavior.
- Additional minor firmware issues.

## 1.0.0

### Added

- Modular Wrist Blaster firmware with configurable controller pin definitions.
- DFPlayer Mini sound playback synchronized with prop states and animations.
- WS2812 effects for the firing rod, indicators, vent, Cyclotron, and optional Power Cell.
- MAX7219/MAX7221 and HT16K33 panel bar-meter support.
- Optional 24-segment bar-meter or WS2812 Power Cell.
- Optional battery-voltage monitoring and low-voltage shutdown.
- Configurable switches, buttons, potentiometers, animation directions, mappings, and serial debugging.

## 0.x

- Initial development firmware. Preserved under `Previous firmware and sounds FX releases/V0`.
