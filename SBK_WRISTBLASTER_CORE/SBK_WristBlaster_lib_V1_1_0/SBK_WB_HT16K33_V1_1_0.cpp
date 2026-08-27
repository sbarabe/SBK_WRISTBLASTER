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

#include "SBK_WB_HT16K33_V1_1_0.h"

void HT16K33::begin(uint8_t addr)
{
  // set the I2C address
  _i2c_addr = addr;

  // assign + zero some buffer data
  _buffer = (uint16_t *)calloc(8, sizeof(uint16_t));

  // start everything
  Wire.begin();
  Wire.beginTransmission(_i2c_addr);
  Wire.write(0x21); // turn it on
  Wire.endTransmission();

  // set blink off + brightness all the way up
  // setBlink(HT16K33_BLINK_OFF);
  clear();
  setBrightness(15);

  // write the matrix, just in case
  show(0);
}

void HT16K33::clear(uint8_t addr)
{
  (void)addr;
  clear();
}

void HT16K33::clear()
{
  for (uint8_t i = 0; i < 8; i++)
  {
    _buffer[i] = 0;
  }
}

/**
 * Sets the brightness of the display.
 */
void HT16K33::setBrightness(uint8_t addr, uint8_t brightness)
{
  (void)addr;
  setBrightness(brightness);
}

void HT16K33::setBrightness(uint8_t brightness)
{
  // constrain the brightness to a 4-bit number (0–15)
  brightness = brightness & 0x0F;

  // send the command
  Wire.beginTransmission(_i2c_addr);
  Wire.write(HT16K33_CMD_DIMMING | brightness);
  Wire.endTransmission();
}

/**
 * Sets the value of a particular pixel.
 */
void HT16K33::setLed(uint8_t addr, uint8_t col, uint8_t row, bool state)
{
  (void)addr;
  setLed(col, row, state);
}
void HT16K33::setLed(uint8_t col, uint8_t row, bool state)
{
  // bounds checking
  col = col & 0x0F;
  row = row & 0x07;
  state = state & 0x01;

  // write the buffer
  if (state == 1)
  {
    _buffer[row] |= 1 << col;
  }
  else
  {
    _buffer[row] &= ~(1 << col);
  }
}

void HT16K33::show(uint8_t addr) {
  (void)addr;
  write(); }

/**
 * Write the RAM buffer to the matrix.
 */
void HT16K33::write()
{
  Wire.beginTransmission(_i2c_addr);
  Wire.write(HT16K33_CMD_RAM);

  for (uint8_t row = 0; row < 8; row++)
  {
    writeRow(row);
  }

  Wire.endTransmission();
}

/**
 * Write a row to the chip.
 */
void HT16K33::writeRow(uint8_t row)
{
  Wire.write(_buffer[row] & 0xFF); // first byte
  Wire.write(_buffer[row] >> 8);   // second byte
}