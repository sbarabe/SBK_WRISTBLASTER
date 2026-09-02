/**
 *  This code is part of SBK_WRISTBLASTER_CORE, a codebase for animations and effects
 *  of a Wrist Blaster prop inspired by the movie Ghostbusters: Frozen Empire.
 * 
 *  @author      Samuel Barabé  
 *  @copyright   Copyright (c) 2025-2026 Samuel Barabé  
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

#pragma once

#include <Arduino.h>
#include <stdint.h>

class BarMeterDriver
{
public:
// Clears the display buffer or screen for the specified device address
virtual void clear(uint8_t addr) = 0;

// Sets the brightness level for the device at the given address
virtual void setBrightness(uint8_t addr, uint8_t brightness) = 0;

// Sets the pixel (x, y) on or off for the specified device address
virtual void setLed(uint8_t addr, uint8_t x, uint8_t y, bool state) = 0;

// Pushes current buffer contents to all devices (or the managed range)
virtual void show(uint8_t addr) = 0;

// Virtual destructor for safe polymorphic cleanup
virtual ~BarMeterDriver() {}
};

