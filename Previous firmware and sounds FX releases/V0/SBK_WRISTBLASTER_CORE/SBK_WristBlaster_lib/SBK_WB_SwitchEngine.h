/*
 *  This code is part of SBK_WRISTBLASTER_CORE (VERSION 0), a codebase for animations and effects
 *  of a Wrist Blaster prop inspired by the movie Ghostbusters: Frozen Empire.
 *  Copyright (c) 2025 Samuel Barabé
 *
 *  For more information, visit the project page: <https://github.com/sbarabe/SBK_WRISTBLASTER_CORE>.
 *
 *  This work is licensed under the Creative Commons Attribution 4.0 International License (CC BY 4.0).
 *  You are free to share, copy, and modify this code as long as appropriate credit is given to the author.
 *  See the full license at: <https://creativecommons.org/licenses/by/4.0/>.
 *
 *  This code is provided "as-is" without any warranty of any kind, either expressed or implied,
 *  including but not limited to the warranties of merchantability or fitness for a particular purpose.
 *  See the full license text for more details.
 */

#ifndef SBK_WB_SWITCHESENGINE_H
#define SBK_WB_SWITCHESENGINE_H

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

#endif
