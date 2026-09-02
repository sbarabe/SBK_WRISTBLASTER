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

/* GENERAL HELPERS */
#ifndef DISABLE
#define DISABLE 0
#endif
#ifndef ENABLE
#define ENABLE 1
#endif
enum SwitchLogicType { DIRECT_LOGIC, REVERSE_LOGIC };


class Switch
{
public:
    Switch(const uint8_t pin, SwitchLogicType logic_type,  const char *name);
    void begin();
    bool update();
    bool update(uint32_t syncCurrentTime);
    bool isON();
    bool isOFF();
    bool toggleON();
    bool toggleOFF();

private:
    void _getReading();
    uint32_t getDebounce(bool reading, bool prev_state, uint32_t last_debounce);
    bool getPushButtonState(bool reading, bool state, uint32_t last_debounce);
    const uint8_t _PIN;
    SwitchLogicType _LOGIC_TYPE;
    const char *_NAME;
     uint32_t _currentTime;
    bool _state;
    bool _statePrev;
    bool _reading;
    bool _readingPrev;
    uint32_t _toggleNow;
};

