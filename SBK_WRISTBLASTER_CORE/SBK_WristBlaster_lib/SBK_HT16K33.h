/**********************************************************************
 *
 * This is a borrowed and modified library from this GitHub repository :
 * https://github.com/MikeS11/ProtonPack/tree/master/Source/Libraries/ht16k33-arduino-master
 * 
 * It's only purpose is to drive the bar meter with the HT16K33 I2C
 * LEDs driver.
 *
 **********************************************************************/

#ifndef SBK_HT16K33_H
#define SBK_HT16K33_H

#include "Arduino.h"

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
class HT16K33
{
public:
  void init(uint8_t addr);

  // brightness control
  void setBrightness(uint8_t brightness);

  // blink controls
  void setBlink(uint8_t blink);

  // orientation
  void resetOrientation(void);
  void reverse(void);
  void flipVertical(void);
  void flipHorizontal(void);

  // buffer stuff
  void clear(void);
  void setPixel(uint8_t row, uint8_t col, uint8_t onff);
  void setRow(uint8_t row, uint16_t value);
  void setColumn(uint8_t col, uint8_t value);

  // read/write
  void write(void);

private:
  uint16_t *_buffer;
  uint8_t _i2c_addr;
  bool _reversed;
  bool _vFlipped;
  bool _hFlipped;

  void writeRow(uint8_t row);
};

#endif 
