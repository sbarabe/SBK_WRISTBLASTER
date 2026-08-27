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

/**********************************************************************
 *
 *  This library is a modified version of the original HT16K33 library
 *  from this GitHub repository:
 *  <https://github.com/MikeS11/ProtonPack/tree/master/Source/Libraries/ht16k33-arduino-master>
 *
 *  The purpose of this modified library is solely to drive the bar meter
 *  using the HT16K33 I2C LED driver for the SBK Wrist Blaster project.
 *
 *  All credit for the original library goes to Mike S11.
 *
 **********************************************************************/

#pragma once

#include "Arduino.h"
#include "SBK_WB_BarMeterDriver_V1_1_0.h"

// include Wire for I2C comms
#include <Wire.h>

// different commands
#define HT16K33_CMD_RAM 0x00
#define HT16K33_CMD_KEYS 0x40
#define HT16K33_CMD_SETUP 0x80
#define HT16K33_CMD_ROWINT 0xA0
#define HT16K33_CMD_DIMMING 0xE0

// other options
#define HT16K33_DISPLAY_OFF 0x00
#define HT16K33_DISPLAY_ON 0x01
#define HT16K33_BLINK_OFF 0x00
#define HT16K33_BLINK_1HZ 0x02
#define HT16K33_BLINK_2HZ 0x04
#define HT16K33_BLINK_0HZ5 0x06

// actual class
class HT16K33 : public BarMeterDriver
{
public:
  void begin(uint8_t addr);

  void clear(uint8_t addr)override;
  void clear();

  // brightness control
  void setBrightness(uint8_t addr, uint8_t brightness)override;
  void setBrightness(uint8_t brightness);

  // buffer stuff
  void setLed(uint8_t addr, uint8_t row, uint8_t col, bool state)override;
  void setLed(uint8_t row, uint8_t col, bool state);

  // push updates dot display
  void show(uint8_t addr)override; 
  

private:
  uint16_t *_buffer;
  uint8_t _i2c_addr;
  void write();
  void writeRow(uint8_t row);
};

