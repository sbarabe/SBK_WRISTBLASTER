/*
 *  WB_SwitchEngine.h is a part of SBK_WRISTBLASTER_CORE (Version 0) code for bar meter animations of a Wrist Blaster replica
 *  Copyright (c) 2025 Samuel Barabé
 *
 *  See this page for reference <https://github.com/sbarabe/SBK_WRISTBLASTER_CORE>.
 *
 *  SBK_WRISTBLASTER_CORE is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Foundation, either version
 *  3 of the License, or (at your option) any later version.
 *
 *  SBK_WRISTBLASTER_CORE is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with Foobar. If not,
 *  see <https://www.gnu.org/licenses/>
 */

#ifndef WB_SWITCHESENGINE_H
#define WB_SWITCHESENGINE_H

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
