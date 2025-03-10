/*
 *  SBK_WB_IndicatorEngine.cpp is a part of SBK_WRISTBLASTER_CORE (Version 0) code for animations of a Wrist Blaster replica
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

#include "SBK_WB_IndicatorEngine.h"

// #define DEBUG_TO_SERIAL
#ifdef DEBUG_TO_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif


Indicator::Indicator(Adafruit_NeoPixel *strip, const uint8_t *pixel, const char *name)
    : LedsStrip(strip),
      P_PIXEL(pixel),
      _tg_r(0), _ini_r(0),
      _tg_g(0), _ini_g(0),
      _tg_b(0), _ini_b(0),
      _tg_brightness(100),
      _pulse(false),
      _blinkInt(0), _wasBlinking(false),
      _ini_blinkInt(0), _tg_blinkInt(0)
{
    _NAME = name;
}

void Indicator::begin()
{
    clear();
    DEBUG_PRINTLN(String(_NAME) + " DEBUG ON");
}

void Indicator::clear()
{
    _clearPixel(*P_PIXEL);
    _tg_brightness = 100;
    _rampTime = 0;
    _pulse = false;
    _wasBlinking = false;
}

void Indicator::initParam(const uint8_t color[3], uint8_t tg_brightness)
{
    initParam(color, tg_brightness, SOLID, NO_RAMP, NO_SYNC, false); // Solid color, no blinking
}

void Indicator::initParam(const uint8_t color[3], uint8_t tg_brightness,
                          uint16_t tg_blinkInt)
{
    initParam(color, tg_brightness, tg_blinkInt, NO_RAMP, NO_SYNC, false); // Blinking but no synch
}

void Indicator::initParam(const uint8_t color[3], uint8_t tg_brightness,
                          uint16_t tg_blinkInt,
                          uint16_t rampTime)
{
    initParam(color, tg_brightness, 0, rampTime, NO_SYNC, false); // No blink, apply ramp
}

void Indicator::initParam(const uint8_t color[3], uint8_t tg_brightness,
                          uint16_t tg_blinkInt, uint32_t syncPrevBlink, bool syncPulse)
{
    initParam(color, tg_brightness, tg_blinkInt, NO_RAMP, syncPrevBlink, syncPulse); // No ramp, sync the previous blink and pulse state
}

void Indicator::initParam(const uint8_t color[3], uint8_t tg_brightness,
                          uint16_t tg_blinkInt,
                          uint16_t rampTime,
                          uint32_t syncPrevBlink = 0, bool syncPulse = false)
{
    // Reset blinking pulse only if it was not blinking in this previous state
    // to esure steady blinking between stage
    if (!_wasBlinking)
        _pulse = false; // Enforce an off pulse at new blinking start

    // Extract the individual color components from the packed value
    uint8_t currentRed, currentGreen, currentBlue;
    _getCurrentColor(*P_PIXEL, currentRed, currentGreen, currentBlue);

    // Get initial parameters
    _iniTime = _currentTime;
    _ini_r = currentRed;
    _ini_g = currentGreen;
    _ini_b = currentBlue;
    _ini_blinkInt = _blinkInt;

    // Check boundaries
    _rampTime = max(0, rampTime);
    _tg_brightness = constrain(tg_brightness, 0, 100);
    _tg_blinkInt = max(0, tg_blinkInt);
    _tg_r = constrain(color[0], 0, 255);
    _tg_g = constrain(color[1], 0, 255);
    _tg_b = constrain(color[2], 0, 255);

    // Correct targets according to brightness
    _tg_r = (_tg_r * _tg_brightness) / 100;
    _tg_g = (_tg_g * _tg_brightness) / 100;
    _tg_b = (_tg_b * _tg_brightness) / 100;

    if (_rampTime == 0)
    {
        _setColor(*P_PIXEL, _tg_r, _tg_g, _tg_b);
        _blinkInt = _tg_blinkInt;
    }

    // Synchronize blinking if a previous blink timestamp is provided
    if (syncPrevBlink > 0)
    {
        _prevBlink = syncPrevBlink;
        _pulse = syncPulse; // Synchronize pulse state
    }

    // if (_NAME == "IND_SlBlw")
    // {
    // DEBUG_PRINTLN("Indicator " + String(_NAME) + "  animation init:");
    // DEBUG_PRINTLN("_iniTime " + String(_iniTime) + "  _ini_blinkInt " + String(_ini_blinkInt) + "  _rampTime " + String(_rampTime));
    // DEBUG_PRINTLN("_tg_brightness " + String(_tg_brightness) + "  tg_blinkInt " + String(tg_blinkInt));
    // DEBUG_PRINTLN("_tg_r " + String(_tg_r) + "  _ini_r " + String(_ini_r) + "  currentRed " + String(currentRed));
    // DEBUG_PRINTLN("_tg_g " + String(_tg_g) + "  _ini_g " + String(_ini_g) + "  currentGreen " + String(currentGreen));
    // DEBUG_PRINTLN("_tg_b " + String(_tg_b) + "  _ini_b " + String(_ini_b) + "  currentBlue " + String(currentBlue));
    // DEBUG_PRINTLN();
    // }
}

void Indicator::solid()
{
    _setColor(*P_PIXEL, _tg_r, _tg_g, _tg_b);

    // Update blinking status
    _wasBlinking = false;
}

void Indicator::blink(bool enableBlinkIntRamp)
{
    // there is no blinking interval,
    if (_tg_blinkInt == 0)
    {
        _setColor(*P_PIXEL, _tg_r, _tg_g, _tg_b);
        // Update blinking status
        _wasBlinking = false;
        return;
    }

    // Update blinking status
    _wasBlinking = true;

    // Ramp blink interval if enable
    if (_blinkInt != _tg_blinkInt)
        _blinkInt = (enableBlinkIntRamp) ? _rampParameter(_iniTime, _rampTime, _ini_blinkInt, _tg_blinkInt, _updateSpeed) : _tg_blinkInt;

    // Toggle blinking pulse
    if (_currentTime - _prevBlink >= _blinkInt)
    {
        // DEBUG_PRINTLN("_blinkInt " + String(_blinkInt) + "  _pulse " + String(_pulse));

        _prevBlink = _currentTime;
        _pulse = !_pulse;

        _setColor(*P_PIXEL, _tg_r * _pulse, _tg_g * _pulse, _tg_b * _pulse);
    }
}

bool Indicator::ramp()
{
    // if (_NAME == "IND_SlBlw")
    // {
    //    DEBUG_PRINTLN(String(_NAME)+"currentRed " + String(currentRed) + "  _ini_g " + String(currentGreen) + "  currentBlue " + String(currentBlue));
    // }

    // Update blinking status
    _wasBlinking = false;

    // Extract the individual color components from the packed value
    uint8_t currentRed, currentGreen, currentBlue;
    _getCurrentColor(*P_PIXEL, currentRed, currentGreen, currentBlue);

    // Check if ramp is done
    if ((currentRed == _tg_r) && (currentGreen == _tg_g) && (currentBlue == _tg_b))
        return true;

    if (_rampTime == 0)
    {
        _setColor(*P_PIXEL, _tg_r, _tg_g, _tg_b);
        return true;
    }

    // Ramp not done yet - Update color tracker
    if (_currentTime - _prevUpdate >= _updateSpeed)
    {
        _prevUpdate = _currentTime;

        _setColor(*P_PIXEL, _rampParameter(_iniTime, _rampTime, _ini_r, _tg_r, _updateSpeed),
                  _rampParameter(_iniTime, _rampTime, _ini_g, _tg_g, _updateSpeed),
                  _rampParameter(_iniTime, _rampTime, _ini_b, _tg_b, _updateSpeed));
    }
    return false;
}

void Indicator::flash(uint16_t flashInterval)
{
    // Turn on LED
    if (_currentTime - _prevUpdate >= flashInterval)
    {
        _prevUpdate = _currentTime;

        // _setColor((_tg_r * _tg_brightness) / 100,
        //           (_tg_g * _tg_brightness) / 100,
        //          (_tg_b * _tg_brightness) / 100);

        _pulse = false;
    }

    // Turn off LED after short flash
    if (_currentTime - _prevUpdate >= flashInterval - 50)
    {
        // clear();
        _pulse = true;
    }

    _setColor(*P_PIXEL,
              _pulse * (_tg_r * _tg_brightness) / 100,
              _pulse * (_tg_g * _tg_brightness) / 100,
              _pulse * (_tg_b * _tg_brightness) / 100);
}

uint32_t Indicator::getPrevBlink() const { return _prevBlink; }

bool Indicator::getPulse() const { return _pulse; }

////////////////////////////////////////////////////////////////////

SingleColorIndicator::SingleColorIndicator(const uint8_t indicator_pin, const bool enable, const char *name)
    : _PIN(indicator_pin),
      _ENABLE(enable),
      _currentTime(0),
      _state(false),
      _update(true),
      _prevUpdate(0),
      _pulse(false),
      _blinkInt(500),
      _wasBlinking(false)
{
    _NAME = name;
}

void SingleColorIndicator::begin()
{
    pinMode(_PIN, OUTPUT);
    _state = false;
    _write(_state);

    DEBUG_PRINTLN(_NAME + String(" DEBUG ON"));
}

bool SingleColorIndicator::update()
{
    return update(millis());
}

bool SingleColorIndicator::update(uint32_t syncCurrentTime)
{
    _currentTime = syncCurrentTime;

    if (_update)
    {
        _update = false;
        return true;
    }
    return false;
}

void SingleColorIndicator::_write(bool state)
{
    if (!_ENABLE && _state)
    {
        digitalWrite(_PIN, LOW);
        _state = false;
        _update = true;
        return;
    }

    if (state != _state)
    {
        digitalWrite(_PIN, state);
        _state = state;
        _update = true;
    }
}

void SingleColorIndicator::on()
{
    if (!_ENABLE)
        return;

    _write(true);
    _wasBlinking = false;
}

void SingleColorIndicator::off()
{
    if (!_ENABLE)
        return;

    _write(false);
    _wasBlinking = false;
}

void SingleColorIndicator::clear()
{
    if (!_ENABLE)
        return;

    _write(false);
    _pulse = false;
    _wasBlinking = false;
}

void SingleColorIndicator::blinkInit(uint16_t blinkInterval) // flashing
{
    if (!_ENABLE)
        return;

    _blinkInt = max(0, blinkInterval);

    // Reset blinking pulse only if it was not blinking in this previous state
    // to esure steady blinking between stage
    if (!_wasBlinking)
        _pulse = false; // Enforce an off pulse at new blinking start

    _wasBlinking = true;
}

void SingleColorIndicator::blink() // flashing
{
    if (!_ENABLE)
        return;

    if (_blinkInt == 0)
    {
        on();
        return;
    }

    if ((_currentTime - _prevUpdate) >= _blinkInt)
    {
        _prevUpdate = _currentTime;
        _pulse ? _write(true) : _write(false);
        _pulse = !_pulse;
    }
}
