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
#include "SBK_WB_LedsStripBaseEngine_V1_1_0.h"
#include "SBK_WB_PowerCellParameters_V1_1_0.h"

class PowerCell : public LedsStrip
{
public:
    PowerCell(Adafruit_NeoPixel *strip,
              const uint8_t *numLed, const uint8_t *start, const uint8_t *end, const bool *direction);
    ~PowerCell();
    void begin(uint8_t batt_level, uint8_t battType);
    void clear();
    bool update(uint8_t batt_level);
    bool update(uint32_t syncCurrentTime, uint8_t batt_level);
    void bootInit(uint16_t duration);
    bool boot();
    void shutDownInit(int16_t duration);
    bool shutDown();
    void poweredDownInit(uint16_t flashInterval);
    void poweredDown();
    void rampToIdleInit(uint16_t tg_speed, uint16_t ramp_time);
    void rampToIdle();
    void lowBattInit(uint16_t blinkInterval);
    void lowBatt();

private:
    const uint8_t *P_NUMLEDS, *P_START, *P_END;
    const bool *P_DIRECTION;
    uint8_t *_ini_r, *_ini_g, *_ini_b;
    uint16_t _ini_speed, _tg_speed;
    int8_t _tracker;
    int8_t _levelTracker;
    bool _bootState;
    bool _pulse;
    uint16_t _flashInterval;
    bool _lowBatt;
    uint8_t _battRed, _battGreen, _battBlue;
    uint8_t _battLevel;
    uint8_t _battType;

    bool _fallingPixelStackUp(uint16_t duration, bool initialize);
    bool _risingPixelStackDown(uint16_t duration, bool initialize);
    bool _fillUp(bool initialize);
    void _PwrCellSetColor(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue);
    void _getBattLevelColors();
    float _mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
};


