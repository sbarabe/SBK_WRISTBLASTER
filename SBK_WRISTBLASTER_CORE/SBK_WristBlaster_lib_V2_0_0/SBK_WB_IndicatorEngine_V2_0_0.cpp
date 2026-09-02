 /**
 *  This code is part of SBK_WRISTBLASTER_CORE, a codebase for animations and effects
 *  of a Wrist Blaster prop inspired by the movie Ghostbusters: Frozen Empire.
 * 
 *  @author      Samuel Barabé  
 *  @copyright   Copyright (c) 2025-2026 Samuel Barabé  
 *  @license     MIT License (code)  
 *  @version     2.0.0
 *  @link        https://github.com/sbarabe/SBK_WRISTBLASTER/tree/main/SBK_WRISTBLASTER_CORE
 *
 *  For more information, visit the project page: <https://github.com/sbarabe/SBK_WRISTBLASTER/tree/main/SBK_WRISTBLASTER_CORE>.
 *
* @see         https://opensource.org/licenses/MIT
 *
 *  This code is provided "as-is" without any warranty of any kind, either expressed or implied,
 *  including but not limited to the warranties of merchantability or fitness for a particular purpose.
 */

#include "SBK_WB_IndicatorEngine_V2_0_0.h"

Indicator::Indicator(Adafruit_NeoPixel *strip, uint8_t pixel)
    : LedsStrip(strip),
      _PIXEL(pixel),
      _tg_r(0), _ini_r(0),
      _tg_g(0), _ini_g(0),
      _tg_b(0), _ini_b(0),
      _pulse(false),
      _prevBlink(0),
      _blinkInt(0), _wasBlinking(false),
      _ini_blinkInt(0), _tg_blinkInt(0)
{
}

void Indicator::begin()
{
    clear();
}

void Indicator::clear()
{
    _clearPixel(_PIXEL);
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
                          uint16_t /* solidMode */,
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
    // to ensure steady blinking between stages
    if (!_wasBlinking)
        _pulse = false; // Enforce an off pulse at new blinking start

    // Extract the individual color components from the packed value
    uint8_t currentRed, currentGreen, currentBlue;
    _getCurrentColor(_PIXEL, currentRed, currentGreen, currentBlue);

    // Get initial parameters
    _iniTime = _now;
    _ini_r = currentRed;
    _ini_g = currentGreen;
    _ini_b = currentBlue;
    _ini_blinkInt = _blinkInt;

    // Apply targets
    _rampTime = rampTime;
    const uint8_t brightnessPercent = min(tg_brightness, static_cast<uint8_t>(100));
    _tg_blinkInt = tg_blinkInt;
    _tg_r = color[0];
    _tg_g = color[1];
    _tg_b = color[2];

    // Correct targets according to brightness
    _tg_r = (_tg_r * brightnessPercent) / 100;
    _tg_g = (_tg_g * brightnessPercent) / 100;
    _tg_b = (_tg_b * brightnessPercent) / 100;

    if (_rampTime == 0)
    {
        _setColor(_PIXEL, _tg_r, _tg_g, _tg_b);
        _blinkInt = _tg_blinkInt;
    }

    // Synchronize blinking if a previous blink timestamp is provided
    if (syncPrevBlink > 0)
    {
        _prevBlink = syncPrevBlink;
        _pulse = syncPulse; // Synchronize pulse state
    }

}

void Indicator::solid()
{
    _setColor(_PIXEL, _tg_r, _tg_g, _tg_b);

    // Update blinking status
    _wasBlinking = false;
}

void Indicator::blink(bool enableBlinkIntRamp)
{
    // there is no blinking interval,
    if (_tg_blinkInt == 0)
    {
        _setColor(_PIXEL, _tg_r, _tg_g, _tg_b);
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
    if (_now - _prevBlink >= _blinkInt)
    {
        // DEBUG_PRINTLN("_blinkInt " + String(_blinkInt) + "  _pulse " + String(_pulse));

        _prevBlink = _now;
        _pulse = !_pulse;

        _setColor(_PIXEL, _tg_r * _pulse, _tg_g * _pulse, _tg_b * _pulse);
    }
}

bool Indicator::ramp()
{
    // Update blinking status
    _wasBlinking = false;

    // Extract the individual color components from the packed value
    uint8_t currentRed, currentGreen, currentBlue;
    _getCurrentColor(_PIXEL, currentRed, currentGreen, currentBlue);

    // Check if ramp is done
    if ((currentRed == _tg_r) && (currentGreen == _tg_g) && (currentBlue == _tg_b))
        return true;

    if (_rampTime == 0)
    {
        _setColor(_PIXEL, _tg_r, _tg_g, _tg_b);
        return true;
    }

    // Ramp not done yet - Update color tracker
    if (_now - _prevUpdate >= _updateSpeed)
    {
        _prevUpdate = _now;

        _setColor(_PIXEL, _rampParameter(_iniTime, _rampTime, _ini_r, _tg_r, _updateSpeed),
                  _rampParameter(_iniTime, _rampTime, _ini_g, _tg_g, _updateSpeed),
                  _rampParameter(_iniTime, _rampTime, _ini_b, _tg_b, _updateSpeed));
    }
    return false;
}

void Indicator::flash(uint16_t flashInterval)
{
    // Turn on LED
    if (_now - _prevUpdate >= flashInterval)
    {
        _prevUpdate = _now;

        _pulse = false;
    }

    // Turn off LED after short flash
    if (_now - _prevUpdate >= flashInterval - 50)
    {
        // clear();
        _pulse = true;
    }

    _setColor(_PIXEL,
              _pulse * _tg_r,
              _pulse * _tg_g,
              _pulse * _tg_b);
}

uint32_t Indicator::getPrevBlink() const { return _prevBlink; }

bool Indicator::getPulse() const { return _pulse; }

////////////////////////////////////////////////////////////////////

SingleColorIndicator::SingleColorIndicator(uint8_t indicator_pin, bool enable)
    : _PIN(indicator_pin),
      _ENABLE(enable),
      _now(0),
      _state(false),
      _update(true),
      _prevUpdate(0),
      _pulse(false),
      _blinkInt(500),
      _wasBlinking(false)
{
}

void SingleColorIndicator::begin()
{
    pinMode(_PIN, OUTPUT);
    _state = false;
    _write(_state);

}

bool SingleColorIndicator::update()
{
    return update(millis());
}

bool SingleColorIndicator::update(uint32_t now)
{
    _now = now;

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

    _blinkInt = blinkInterval;

    // Reset blinking pulse only if it was not blinking in this previous state
    // to ensure steady blinking between stages
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

    if ((_now - _prevUpdate) >= _blinkInt)
    {
        _prevUpdate = _now;
        _pulse ? _write(true) : _write(false);
        _pulse = !_pulse;
    }
}
