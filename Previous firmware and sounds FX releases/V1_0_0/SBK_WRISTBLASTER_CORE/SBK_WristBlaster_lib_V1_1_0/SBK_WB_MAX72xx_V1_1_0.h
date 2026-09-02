/**
 *  This code is part of SBK_WRISTBLASTER_CORE, a codebase for animations and effects
 *  of a Wrist Blaster prop inspired by the movie Ghostbusters: Frozen Empire.
 * 
 *  @author      Samuel Barabe
 *  @copyright   Copyright (c) 2025-2026 Samuel Barabe
 *  @license     MIT License (code)
 *  @version     1.1.0
 *  @link        https://github.com/sbarabe/SBK_WRISTBLASTER/tree/main/SBK_WRISTBLASTER_CORE
 *
 *  For more information, visit the project page: <https://github.com/sbarabe/SBK_WRISTBLASTER/tree/main/SBK_WRISTBLASTER_CORE>.
 *
* @see         https://opensource.org/licenses/MIT
 *
 *  This code is provided "as-is" without any warranty of any kind, either expressed or implied,
 *  including but not limited to the warranties of merchantability or fitness for a particular purpose.
 */

/*
 *  Adapted from the original MAX72xx library by Eberhard Fahle:
 *  <https://github.com/wayoda/LedControl>
 *
 *  Original library:
 *  MAX72xx.cpp - A library for controlling LEDs with a MAX7219/MAX7221
 *  Copyright (c) 2007 Eberhard Fahle
 *
 *  The purpose of this modified version is solely to drive the bar meter
 *  using the MAX72xx LED driver for the SBK Wrist Blaster project.
 *
 *  All credit for the original library goes to Eberhard Fahle.
 */

#pragma once

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "SBK_WB_BarMeterDriver_V1_1_0.h"

#define MAX72xx_MAX_DEVICES 2

class MAX72xx : public BarMeterDriver
{
public:
    /* Constructor */
    MAX72xx(uint8_t dataPin, uint8_t clkPin, uint8_t csPin, const uint8_t numDevices);

    void begin(uint8_t device);

    /* Power-saving mode */
    void setShutdown(uint8_t device, bool status);

    /* Set number of digits/rows to display */
    void setScanLimit(uint8_t device, uint8_t limit);

    /* Set brightness */
    void setBrightness(uint8_t addr, uint8_t brightness)override;

    /* Clear display */
    void clear(uint8_t addr) override;
    
    /* Set a single LED */
    void setLed(uint8_t addr, uint8_t row, uint8_t col, bool state)override;

    void show(uint8_t addr)override;

private:
    /* SPI pin configuration */
    const uint8_t _SPI_MOSI;
    const uint8_t _SPI_CLK;
    const uint8_t _SPI_CS;

    /* Number of connected MAX72xx devices */
    const uint8_t _NUM_DEVICES;
    const uint8_t _MAX_DEVICES;

    /* LED status array (for a single 8x8 display) */
    byte status[MAX72xx_MAX_DEVICES][8]; // Reduced from 64 to 16 (assuming 2 devices max)

    // Assuming 2 devices and 8 rows per device
    byte displayBuffer[2][8]; // [device][row]

    /* Buffer for SPI data */
    // byte spidata[4]; // Reduced from 16 to 4 (enough for 1 command)

    /* Send out a single command to the device */
    void spiTransfer(uint8_t addr, byte opcode, byte data);
};