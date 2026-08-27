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

#include "SBK_WB_SmokeEngine.h"

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