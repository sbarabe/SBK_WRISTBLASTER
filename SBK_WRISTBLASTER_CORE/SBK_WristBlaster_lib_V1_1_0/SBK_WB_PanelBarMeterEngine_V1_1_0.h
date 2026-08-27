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

class PanelBarMeter : public BarMeter
{
public:
    PanelBarMeter(const uint8_t *segNumber,
                  const bool *direction,
                  BarMeterDriver *driver,
                  const uint8_t address,
                  const uint8_t (*mapping)[2]);

    ~PanelBarMeter();

    void begin();
    void clear();

    void partyModeInit();
    void partyMode();
    void fillUpEmptyDownOnceInit(uint16_t duration);
    void fillUpEmptyDownOnce();
    void fillUpFastEmptyDownSlowOnceInit(uint16_t duration, bool fadeout);
    void fillUpFastEmptyDownSlowOnce();
    void fillDownEmptyDownOnceInit(uint16_t duration, bool fadeIn);
    void fillDownEmptyDownOnce();
    void cyclotronIdleInit(uint8_t heatLevel);
    void cyclotronIdle(uint8_t heatLevel);
    void cyclotronIdleFullInit(uint8_t heatLevel);
    void cyclotronIdleFull(uint8_t heatLevel);
    void fireInit(bool direction);
    void fireInit(bool direction, bool repeat);
    void fire(uint8_t heatLevel);
    void fire(uint8_t heatLevel, bool repeat);

protected:
    bool _fillDownInit, _fillDownComplete;
    bool _emptyDownInit, _emptyDownComplete;
    bool _fillUpInit, _fillUpComplete;
    int8_t _tracker;
    uint8_t _prevHeatLevel;
    bool _repeat;
    uint32_t _lastBeatTime, _lastRandomUpdate;
    bool _isPeak;
    const uint8_t _MIN_BASE_LEVEL; // Default lower base level
    const uint8_t _MIN_PEAK_LEVEL; // Peak should rise above level 20
    uint8_t _randomOffset;
    int8_t _currentLevel;
    uint8_t _peakLevel;            // Track the highest LED reached
    uint32_t _prevPeakUpdate;      // Timer for peak drop
    const uint8_t _PEAK_HOLD_TIME; // Peak LED decay interval
    uint8_t _fadeIn;
    uint8_t _fadeOut;

    bool _fillUp(uint8_t speed, bool initialize);
    bool _fillDown(uint8_t speed, bool initialize, bool fadein);
    bool _emptyDown(uint8_t speed, bool initialize, bool fadeout);
    bool _emptyUp(uint8_t speed, bool initialize);
};

