/*
 *  WB_SmokeEngine.cpp is a part of SBK_WRISTBLASTER_CORE (Version 0) code for animations of a Wrist Blaster replica
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

#include "WB_SmokeEngine.h"

// #define DEBUG_TO_SERIAL
#ifdef DEBUG_TO_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif


Smoker::Smoker(const uint8_t smoke_pin, const uint8_t fan_pin, const uint16_t *minOffTime, const uint16_t *maxOnTime)
    : _SMOKE_PIN(smoke_pin),
      _FAN_PIN(fan_pin),
      P_MIN_OFF_TIME(minOffTime), P_MAX_ON_TIME(maxOnTime),
      _currentTime(0),
      _stateSmoke(false),
      _stateFan(false),
      _prevStart(0),
      _prevStop(0),
      _prevUpdate(0),
      _prevFanStart(0),
      _burstDuration(0),
      _enable(false),
      _burstWithFan(false),
      _burst(false)
{
}

void Smoker::begin(bool enable)
{
    _enable = enable;
    pinMode(_SMOKE_PIN, OUTPUT);
    pinMode(_FAN_PIN, OUTPUT);
    smoke(DISABLE, DISABLE);
}

void Smoker::setEnable(bool enable)
{
    _enable = enable;
    if (!_enable)
    {
        digitalWrite(_SMOKE_PIN, LOW);
        digitalWrite(_FAN_PIN, LOW);
        _stateSmoke = false;
        _stateFan = false;
        _burst = false;
    }
}

bool Smoker::enable() { return _enable; }

void Smoker::update() { update(millis()); }

void Smoker::update(uint32_t syncCurrentTime)
{
    _currentTime = syncCurrentTime;

    if (!_enable)
    {
        if (_stateSmoke)
        {
            digitalWrite(_SMOKE_PIN, LOW);
            _stateSmoke = false;
        }
        if (_stateFan)
        {
            digitalWrite(_FAN_PIN, LOW);
            _stateFan = false;
        }
    }
    return;

    uint32_t elapsedTime = _currentTime - _prevStart;

    if ((_burst && elapsedTime >= _burstDuration) ||
        elapsedTime >= *P_MAX_ON_TIME)
    {
        if (_stateSmoke)
            smoke(DISABLE);
        if (_burstWithFan && _stateFan)
            fan(DISABLE);
        _burstWithFan = false;
        _burst = false;
    }
}

void Smoker::startBurst(uint16_t duration, bool with_fan)
{
    if (!_enable)
        return;

    _burst = true;
    _burstDuration = duration;
    _burstWithFan = with_fan;
    with_fan ? smoke(true, true) : smoke(true);
}

void Smoker::smoke(bool enable_smoke) // start/stop both smoke and fan
{
    if (!_enable)
        return;

    if (enable_smoke != _stateSmoke)
    {
        digitalWrite(_SMOKE_PIN, enable_smoke);
        _stateSmoke = enable_smoke;
        if (!enable_smoke)
        {
            _prevStop = _currentTime;
            _burst = false;
            _burstWithFan = false;
        }
        else
            _prevStart = _currentTime;
    }
}

void Smoker::smoke(bool enable_smoke, bool enable_fan) // Start/stop smoker and/or fan
{
    if (!_enable)
        return;

    if (enable_smoke != _stateSmoke)
    {
        digitalWrite(_SMOKE_PIN, enable_smoke);
        _stateSmoke = enable_smoke;
        (enable_smoke) ? (_prevStart = _currentTime) : (_prevStop = _currentTime);
    }

    fan(enable_fan);
}

bool Smoker::getSmokeState()
{
    return _stateSmoke;
}

void Smoker::fan(bool fan_enable)
{
    if (!_enable)
        return;

    if (fan_enable != _stateFan)
    {
        digitalWrite(_FAN_PIN, fan_enable);
        _stateFan = fan_enable;
    }
}

bool Smoker::getFanState()
{
    return _stateFan;
}