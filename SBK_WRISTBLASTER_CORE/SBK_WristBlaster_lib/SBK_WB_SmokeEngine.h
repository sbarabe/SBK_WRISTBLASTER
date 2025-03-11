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

#ifndef SBK_WB_SMOKEENGINE_H
#define SBK_WB_SMOKEENGINE_H

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