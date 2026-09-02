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
#include "SBK_WB_BarMeterDriver_V1_1_0.h"

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

class BarMeter
{
public:
    BarMeter(BarMeterDriver *driver, 
        const uint8_t address,
             const uint8_t *segNumber,
             const bool *direction,
             const uint8_t (*mapping)[2]);
    ~BarMeter();
    void update();
    void update(uint32_t syncCurrentTime);

protected:
    BarMeterDriver *_driver;
    const uint8_t _ADDRESS;
    const uint8_t *P_SEG_NUMBER;
    const bool *P_DIRECTION;
    const uint8_t (*_SEG_MAP)[2];
    uint32_t _currentTime;
    uint32_t _prevUpdate;
    uint16_t _ledsStatesLow;       // Stores the state for LEDs 0-15
    uint16_t _ledsStatesHigh;      // Stores the state for LEDs 16-31
    uint16_t _ledsStatesExtraLow;  // Stores the state for LEDs 32-47
    uint16_t _ledsStatesExtraHigh; // Stores the state for LEDs 48-63
    uint8_t _brightness;
    uint8_t _brightnessPrev;
    bool _direction;
    uint8_t _updateInterval, _corrSpeed;
    bool _update;
    uint16_t _duration;
    uint32_t _iniTime;
    uint16_t _ini_speed, _tg_speed;

    uint16_t _rampParameter(uint32_t iniTime, uint16_t rampTime, uint16_t iniPara, uint16_t tgPara, uint16_t updateSpeed);
    uint8_t _getLedState(uint8_t index);
    void _setHigh();
    void _setLow();
    void _setLed(uint8_t index, bool state);
};

