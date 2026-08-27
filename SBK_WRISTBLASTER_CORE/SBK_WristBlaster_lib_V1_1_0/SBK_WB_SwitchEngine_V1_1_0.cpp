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

#include "SBK_WB_SwitchEngine_V1_1_0.h"

// #define DEBUG_TO_SERIAL
#ifdef DEBUG_TO_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

// Class constants definitions
constexpr uint8_t DEBOUNCE_DELAY = 50;

Switch::Switch(const uint8_t pin, SwitchLogicType logic_type, const char *name)
    : _PIN(pin),
      _LOGIC_TYPE(logic_type),
      _currentTime(0),
      _state(false),
      _statePrev(false),
      _reading(false),
      _readingPrev(false),
      _toggleNow(0)
{
    _NAME = name;
}

void Switch::begin()
{
    pinMode(_PIN, INPUT_PULLUP);
    update();
    DEBUG_PRINTLN(String(_NAME) + " DEBUG ON");
}

bool Switch::update() { return update(millis()); }

bool Switch::update(uint32_t syncCurrentTime)
{
    _currentTime = syncCurrentTime;

    // Register previous state for toggle functions
    _statePrev = _state;

    // Read switch pin with logic inversion if needed
    _reading = (_LOGIC_TYPE == REVERSE_LOGIC) ? digitalRead(_PIN) : !digitalRead(_PIN);

    // check if reading has changed
    if (_reading != _readingPrev)
    {
        _readingPrev = _reading;
        _toggleNow = _currentTime;
    }

    // check if reading is maintained for the debounce delay
    if (_currentTime - _toggleNow > DEBOUNCE_DELAY)
    {
        // If true and reading is different from current state, change state
        if (_reading != _state)
        {
            _state = _reading;
            DEBUG_PRINTLN(_state ? String(_NAME) + " is ON" : String(_NAME) + " is OFF");
        }
    }
    return _state;
}

bool Switch::isON() // Retirn true if switch is on
{
    return _state;
}

bool Switch::isOFF() // Return true if swith is off
{
    return !_state;
}

bool Switch::toggleON()
{
    return (!_statePrev && _state);
}

bool Switch::toggleOFF()
{
    return (_statePrev && !_state);
}