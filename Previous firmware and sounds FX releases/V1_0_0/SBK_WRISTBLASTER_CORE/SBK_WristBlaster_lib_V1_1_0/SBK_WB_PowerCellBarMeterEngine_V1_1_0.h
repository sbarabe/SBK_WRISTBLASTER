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
#include "SBK_WB_BarMeterBaseEngine_V1_1_0.h"
#include "SBK_WB_PowerCellParameters_V1_1_0.h"

/* GENERAL HELPERS */
#ifndef DISABLE
#define DISABLE 0
#endif
#ifndef ENABLE
#define ENABLE 1
#endif
// Sequence looping
#ifndef END_SEQ
#define END_SEQ 0
#endif
#ifndef REPEAT_SEQ
#define REPEAT_SEQ 1
#endif
// Fire type
#ifndef CAPTURE
#define CAPTURE 0
#endif
#ifndef BURST
#define BURST 1
#endif
// animation directions
#ifndef FORWARD
#define FORWARD 0
#endif
#ifndef REVERSE
#define REVERSE 1
#endif

class PowerCellBarMeter : public BarMeter
{
public:
    PowerCellBarMeter(const uint8_t *segNumber, 
        const bool *direction,
        BarMeterDriver *driver,
        const uint8_t address,
        const uint8_t (*mapping)[2]);
    ~PowerCellBarMeter();


    void begin(uint8_t batt_level, uint8_t battType);
    void clear();
    void update(uint8_t battLevel);
    void update(uint32_t syncCurrentTime, uint8_t battLevel);
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

protected:
   
    bool _fillDownInit, _fillDownComplete;
    bool _emptyDownInit, _emptyDownComplete;
    bool _fillUpInit, _fillUpComplete;
    int8_t _tracker, _levelTracker;
    bool _repeat;
    uint8_t _updateInterval, _corrSpeed;
    bool _update;
    uint8_t _fadeIn, _fadeOut;
    bool _bootState;
    bool _pulse;
    uint16_t _flashInterval;
    uint8_t _battLevel;
    uint8_t _battType;

    bool _fallingPixelStackUp(uint16_t duration, bool initialize);
    bool _risingPixelStackDown(uint16_t duration, bool initialize);
    bool _fillUp(uint8_t max_level, bool initialize);
    uint8_t getMaxLevelfromBattLevel();
   
};


