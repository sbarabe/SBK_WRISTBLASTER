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

#include "SBK_WB_MAX72xx_V1_1_0.h"

// MAX7219 / MAX7221 opcodes
#define OP_NOOP 0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE 9
#define OP_INTENSITY 10
#define OP_SCANLIMIT 11
#define OP_SHUTDOWN 12
#define OP_DISPLAYTEST 15

MAX72xx::MAX72xx(uint8_t dataPin,
                 uint8_t clkPin,
                 uint8_t csPin,
                 const uint8_t numDevices)
    : _SPI_MOSI(dataPin),
      _SPI_CLK(clkPin),
      _SPI_CS(csPin),
      _NUM_DEVICES(numDevices),
      _MAX_DEVICES(
          numDevices < 1
              ? 1
              : (numDevices > MAX72xx_MAX_DEVICES
                     ? MAX72xx_MAX_DEVICES
                     : numDevices))
{
    pinMode(_SPI_MOSI, OUTPUT);
    pinMode(_SPI_CLK, OUTPUT);
    pinMode(_SPI_CS, OUTPUT);
    digitalWrite(_SPI_CS, HIGH);

    // Initialize all devices
    for (uint8_t i = 0; i < _MAX_DEVICES; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            status[i][j] = 0x00; // Initialize each row for each device
        }
    }
    for (uint8_t i = 0; i < _MAX_DEVICES; i++)
    {
        spiTransfer(i, OP_DISPLAYTEST, 0); // Disable test mode
        setScanLimit(i, 7);                // Set scan limit to full 8 digits
        spiTransfer(i, OP_DECODEMODE, 0);  // Use raw values (no BCD decoding)
        setBrightness(i, 15);              // Set maxBri level (0 is min, 15 is max)
        clear(i);                          // Clear display
        setShutdown(i, true);              // Start in shutdown mode
    }
}

void MAX72xx::begin(uint8_t device)
{
    setShutdown(device, false);
}

void MAX72xx::setShutdown(uint8_t device, bool status)
{
    if (device < _MAX_DEVICES)
    {
        spiTransfer(device, OP_SHUTDOWN, status ? 0 : 1);
    }
}

void MAX72xx::setScanLimit(uint8_t device, uint8_t limit)
{
    if (device < _MAX_DEVICES && limit < 8)
    {
        spiTransfer(device, OP_SCANLIMIT, limit);
    }
}

void MAX72xx::setBrightness(uint8_t addr, uint8_t brightness)
{
    uint8_t device = addr;
    if (device < _MAX_DEVICES && brightness < 16)
    {
        spiTransfer(device, OP_INTENSITY, brightness);
    }
}

void MAX72xx::clear(uint8_t addr)
{
    uint8_t device = addr;
    if (device >= _MAX_DEVICES)
        return;

    for (uint8_t row = 0; row < 8; row++)
    {
        status[device][row] = 0;
        spiTransfer(device, row + 1, 0);
    }
}

void MAX72xx::setLed(uint8_t addr, uint8_t row, uint8_t col, boolean state)
{
    uint8_t device = addr;
    if (device >= _MAX_DEVICES || row > 7 || col > 7)
        return;

    byte mask = B10000000 >> col;
    if (state)
    {
        status[device][row] |= mask;
    }
    else
    {
        status[device][row] &= ~mask;
    }
}

void MAX72xx::show(uint8_t device)
{
    if (device >= _MAX_DEVICES)
        return;

    for (uint8_t row = 0; row < 8; row++)
    {
        spiTransfer(device, row + 1, status[device][row]);
    }
}

void MAX72xx::spiTransfer(uint8_t device, byte opcode, byte data)
{
    if (device >= _MAX_DEVICES)
        return;

    digitalWrite(_SPI_CS, LOW);

    // Send data for the farthest device first.
    for (int8_t currentDevice = _MAX_DEVICES - 1;
         currentDevice >= 0;
         currentDevice--)
    {
        if (currentDevice == device)
        {
            shiftOut(_SPI_MOSI, _SPI_CLK, MSBFIRST, opcode);
            shiftOut(_SPI_MOSI, _SPI_CLK, MSBFIRST, data);
        }
        else
        {
            shiftOut(_SPI_MOSI, _SPI_CLK, MSBFIRST, OP_NOOP);
            shiftOut(_SPI_MOSI, _SPI_CLK, MSBFIRST, 0);
        }
    }
    digitalWrite(_SPI_CS, HIGH);
}