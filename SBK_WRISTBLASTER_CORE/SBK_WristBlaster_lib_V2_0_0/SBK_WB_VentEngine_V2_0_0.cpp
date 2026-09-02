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

#include "SBK_WB_VentEngine_V2_0_0.h"

// #define DEBUG_TO_SERIAL
#ifdef DEBUG_TO_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

Vent::Vent(Adafruit_NeoPixel *strip, const uint8_t pixel)
    : LedsStrip(strip),
      _PIXEL(pixel),
      _ini_r(0), _tg_r(0),
      _ini_g(0), _tg_g(0),
      _ini_b(0), _tg_b(0),
      _flickerSpeed(10)
{
}

void Vent::begin()
{
  clear();
}

void Vent::clear()
{
  _clearPixel(_PIXEL);
  _rampTime = 0;
}

void Vent::initParam(const uint8_t color[3], uint8_t tg_brightness)
{
  initParam(color, tg_brightness, NO_RAMP);
}

void Vent::initParam(const uint8_t color[3], uint8_t tg_brightness, int16_t rampTime)
{
  _iniTime = _now;
  _flickerSpeed = _updateSpeed;

  // Check boundaries
  const uint8_t brightnessPercent = min(tg_brightness, static_cast<uint8_t>(100));
  _rampTime = max(0, rampTime);
  _tg_r = constrain(color[0], 0, 255);
  _tg_g = constrain(color[1], 0, 255);
  _tg_b = constrain(color[2], 0, 255);

  // Set targets according to brightness
  _tg_r = (_tg_r * brightnessPercent) / 100;
  _tg_g = (_tg_g * brightnessPercent) / 100;
  _tg_b = (_tg_b * brightnessPercent) / 100;

  // If ramp time is ZERO, there is no ramp,put color to target
  if (_rampTime == 0)
  {
    _setColor(_PIXEL, _tg_r, _tg_g, _tg_b);
    return;
  }

  // Extract the individual color components from the packed value
  uint8_t currentRed, currentGreen, currentBlue;
  _getCurrentColor(_PIXEL, currentRed, currentGreen, currentBlue);

  // Record initial vent color trackers
  _ini_r = currentRed;
  _ini_g = currentGreen;
  _ini_b = currentBlue;
}

void Vent::solid()
{
  _setColor(_PIXEL, _tg_r, _tg_g, _tg_b);
}

void Vent::flicker(uint8_t flickerAmount, uint16_t maxSpeed)
{
  // Check if it's time to update the flicker effect
  if (_now - _prevUpdate >= _flickerSpeed)
  {
    _prevUpdate = _now;

    _flickerSpeed = maxSpeed > _updateSpeed
                        ? random(_updateSpeed, maxSpeed)
                        : _updateSpeed;

    // Follow the configured ramp before adding the random flicker. This keeps
    // the average flicker color moving from the captured initial color toward
    // the brightness-adjusted target color.
    const uint8_t base_r = _rampTime == 0
                               ? _tg_r
                               : _rampParameter(_iniTime, _rampTime, _ini_r, _tg_r, _updateSpeed);
    const uint8_t base_g = _rampTime == 0
                               ? _tg_g
                               : _rampParameter(_iniTime, _rampTime, _ini_g, _tg_g, _updateSpeed);
    const uint8_t base_b = _rampTime == 0
                               ? _tg_b
                               : _rampParameter(_iniTime, _rampTime, _ini_b, _tg_b, _updateSpeed);

    const uint8_t amountPercent = min(flickerAmount, static_cast<uint8_t>(100));
    const int16_t flickerPercent = random(100 - amountPercent, 101 + amountPercent);

    // Use one brightness variation for all channels to retain the base hue,
    // then add a small independent color variation for a natural flame effect.
    const int16_t scaled_r = (static_cast<uint32_t>(base_r) * flickerPercent) / 100;
    const int16_t scaled_g = (static_cast<uint32_t>(base_g) * flickerPercent) / 100;
    const int16_t scaled_b = (static_cast<uint32_t>(base_b) * flickerPercent) / 100;
    const uint8_t flicker_r = constrain(scaled_r, 0, 255);
    const uint8_t flicker_g = constrain(scaled_g, 0, 255);
    const uint8_t flicker_b = constrain(scaled_b, 0, 255);

    // Apply the flicker effect
    _setColor(_PIXEL, flicker_r, flicker_g, flicker_b);
  }
}

bool Vent::ramp()
{
  // If no ramp, color has been set in rampInit()
  if (_rampTime == 0)
  {
    _setColor(_PIXEL, _tg_r, _tg_g, _tg_b);
    return true;
  }

  // Extract the individual color components from the packed value
  uint8_t currentRed, currentGreen, currentBlue;
  _getCurrentColor(_PIXEL, currentRed, currentGreen, currentBlue);

  // check if ramp is done ramping
  if ((currentRed == _tg_r) && (currentGreen == _tg_g) && (currentBlue == _tg_b))
    return true;

  // Ramp not done yet - Update color tracker
  if (_now - _prevUpdate >= _updateSpeed)
  {
    _prevUpdate = _now;

    _setColor(_PIXEL,
              _rampParameter(_iniTime, _rampTime, _ini_r, _tg_r, _updateSpeed),
              _rampParameter(_iniTime, _rampTime, _ini_g, _tg_g, _updateSpeed),
              _rampParameter(_iniTime, _rampTime, _ini_b, _tg_b, _updateSpeed));
  }

  return false;
}
