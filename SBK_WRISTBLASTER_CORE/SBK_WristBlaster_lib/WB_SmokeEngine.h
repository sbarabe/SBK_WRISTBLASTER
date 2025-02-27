/*
 *  WB_SmokeEngine.h is a part of SBK_WRISTBLASTER_CORE (Version 0) code for animations of a Wrist Blaster replica
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

#ifndef WB_SMOKEENGINE_H
#define WB_SMOKEENGINE_H

#include "Arduino.h"

/* GENERAL HELPERS */
#ifndef DISABLE
#define DISABLE 0
#endif
#ifndef ENABLE
#define ENABLE 1
#endif

#ifndef DISABLE_FAN
#define DISABLE_FAN 0
#endif
#ifndef ENABLE_FAN
#define ENABLE_FAN 1
#endif

#ifndef NO_FAN
#define NO_FAN 0
#endif
#ifndef WITH_FAN
#define WITH_FAN 1
#endif

class Smoker
{
public:
    Smoker(const uint8_t smoke_pin, const uint8_t fan_pin, const uint16_t *minOffTime, const uint16_t *maxOnTime);
    void begin(bool enable);
    bool enable();
    void setEnable(bool enable);
    void update();
    void update(uint32_t syncCurrentTime);
    void startBurst(uint16_t duration, bool with_fan);
    bool getSmokeState();
    void smoke(bool smoke_enable);
    void smoke(bool smoke_enable, bool fan_enable);
    bool getFanState();
    void fan(bool fan_enable);

private:
    const uint8_t _SMOKE_PIN;
    const uint8_t _FAN_PIN;
    const uint16_t *P_MIN_OFF_TIME, *P_MAX_ON_TIME;
    uint32_t _currentTime;
    bool _stateSmoke;
    bool _stateFan;
    uint32_t _prevStart;
    uint32_t _prevStop;
    uint32_t _prevUpdate;
    uint32_t _prevFanStart;
    uint16_t _burstDuration;
    bool _enable;
    bool _burstWithFan;
    bool _burst;
};

#endif