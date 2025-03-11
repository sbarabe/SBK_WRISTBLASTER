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

#include "SBK_WB_VentEngine.h"

// #define DEBUG_TO_SERIAL
#ifdef DEBUG_TO_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

Vent::Vent(Adafruit_NeoPixel *strip, const uint8_t *pixel)
    : LedsStrip(strip),
      P_PIXEL(pixel),
      _rPrev(0), _gPrev(0), _bPrev(0),
      _tg_brightness(0),
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
  _clearPixel(*P_PIXEL);
  _rampTime = 0;
  _tg_brightness = 0;
}

void Vent::initParam(const uint8_t color[3], uint8_t tg_brightness)
{
  initParam(color, tg_brightness, NO_RAMP);
}

void Vent::initParam(const uint8_t color[3], uint8_t tg_brightness, int16_t rampTime)
{
  _iniTime = _currentTime;
  _flickerSpeed = _updateSpeed;

  // Check boundaries
  _tg_brightness = constrain(tg_brightness, 0, 100);
  _rampTime = max(0, rampTime);
  _tg_r = constrain(color[0], 0, 255);
  _tg_g = constrain(color[1], 0, 255);
  _tg_b = constrain(color[2], 0, 255);

  // Set targets according to brightness
  _tg_r = (_tg_r * _tg_brightness) / 100;
  _tg_g = (_tg_g * _tg_brightness) / 100;
  _tg_b = (_tg_b * _tg_brightness) / 100;

  // If ramp time is ZERO, there is no ramp,put color to target
  if (_rampTime == 0)
  {
    _setColor(*P_PIXEL, _tg_r, _tg_g, _tg_b);
    return;
  }

  // Extract the individual color components from the packed value
  uint8_t currentRed, currentGreen, currentBlue;
  _getCurrentColor(*P_PIXEL, currentRed, currentGreen, currentBlue);

  // Record initial vent color trackers
  _ini_r = currentRed;
  _ini_g = currentGreen;
  _ini_b = currentBlue;
}

void Vent::solid()
{
  _setColor(*P_PIXEL, _tg_r, _tg_g, _tg_b);
}

void Vent::flicker(uint8_t flickerAmount, uint16_t maxSpeed)
{
  // Check if it's time to update the flicker effect
  if (_currentTime - _prevUpdate >= _flickerSpeed)
  {
    _prevUpdate = _currentTime;

    _flickerSpeed = random(_updateSpeed, maxSpeed);

    // Generate random brightness variation within flickerAmount
    int8_t variation = random(-_tg_brightness * flickerAmount / 100, _tg_brightness * flickerAmount / 100);
    uint8_t flickerBrightness = constrain(_tg_brightness + variation, 0, 100);

    // Calculate flickered color based on brightness variation
    uint8_t flicker_r = constrain((_tg_r * flickerBrightness) / 100 + random(-15, 15), 0, 255);
    uint8_t flicker_g = constrain((_tg_g * flickerBrightness) / 100 + random(-15, 15), 0, 255);
    uint8_t flicker_b = constrain((_tg_b * flickerBrightness) / 100 + random(-15, 15), 0, 255);

    // Apply the flicker effect
    _setColor(*P_PIXEL, flicker_r, flicker_g, flicker_b);
  }
}

bool Vent::ramp()
{
  // If no ramp, color has been set in rampInit()
  if (_rampTime == 0)
  {
    _setColor(*P_PIXEL, _tg_r, _tg_g, _tg_b);
    return true;
  }

  // Extract the individual color components from the packed value
  uint8_t currentRed, currentGreen, currentBlue;
  _getCurrentColor(*P_PIXEL, currentRed, currentGreen, currentBlue);

  // check if ramp is done ramping
  if ((currentRed == _tg_r) && (currentGreen == _tg_g) && (currentBlue == _tg_b))
    return true;

  // Ramp not done yet - Update color tracker
  if (_currentTime - _prevUpdate >= _updateSpeed)
  {
    _prevUpdate = _currentTime;

    _setColor(*P_PIXEL,
              _rampParameter(_iniTime, _rampTime, _ini_r, _tg_r, _updateSpeed),
              _rampParameter(_iniTime, _rampTime, _ini_g, _tg_g, _updateSpeed),
              _rampParameter(_iniTime, _rampTime, _ini_b, _tg_b, _updateSpeed));
  }

  return false;
}
