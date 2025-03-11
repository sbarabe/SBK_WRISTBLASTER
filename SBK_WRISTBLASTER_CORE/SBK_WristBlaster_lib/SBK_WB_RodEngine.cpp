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

#include "SBK_WB_RodEngine.h"

// #define DEBUG_TO_SERIAL
#ifdef DEBUG_TO_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

/*  HELPERS  */
const uint8_t FIRE_STROBE_WHITE_COMPONENT = 0; // 0-255, increasing this increase the white level of the firing strobe.
const uint8_t DEFAULT_HUE = 42;

FiringRod::FiringRod(Adafruit_NeoPixel *strip,
                     const uint8_t potPin, const bool potEnable,
                     const uint8_t *numLeds, const uint8_t *start, const uint8_t *end)
    : LedsStrip(strip),
      _POT_PIN(potPin),
      _POT_ENABLE(potEnable),
      P_NUMLEDS(numLeds), P_START(start), P_END(end),
      _tg_brightness(0), _brightness(0),
      _ini_brightness(0),
      _shuffle(true),
      _strobeSpeed(10),
      _hue(42) // Purple hue aka red and blue
{
    _ini_r = new uint8_t[*P_NUMLEDS];
    _ini_g = new uint8_t[*P_NUMLEDS];
    _ini_b = new uint8_t[*P_NUMLEDS];
}

FiringRod::~FiringRod()
{
    delete[] _ini_r;
    delete[] _ini_g;
    delete[] _ini_b;
}

void FiringRod::begin()
{
    pinMode(_POT_PIN, INPUT);
    clear();
}

void FiringRod::clear()
{
    _clearSomePixels(*P_START, *P_END);
    _tg_brightness = 0;
    _rampTime = 0;
}

void FiringRod::strobeInit()
{
    strobeInit(SHUFFLE, 100, NO_RAMP);
}

void FiringRod::strobeInit(bool shuffle)
{
    strobeInit(shuffle, 100, NO_RAMP);
}

void FiringRod::strobeInit(bool shuffle, uint8_t tg_brightness)
{
    strobeInit(shuffle, tg_brightness, NO_RAMP);
}

void FiringRod::strobeInit(bool shuffle, uint8_t tg_brightness, uint16_t rampTime)
{
    _iniTime = _currentTime;
    _ini_brightness = _brightness;
    _shuffle = shuffle;

    // Read potentiometer and map it to a hue range (0-255)

    _hue = _getHue();
    uint8_t r, g, b;
    _hueToRGB(_hue, r, g, b);

    // Set initial colors
    for (uint8_t i = 0; i < *P_NUMLEDS; i++)
    {
        _ini_r[i] = _randomScaledBrightness(r);
        _ini_g[i] = _randomScaledBrightness(g);
        _ini_b[i] = _randomScaledBrightness(b);

        _setColor(*P_START+i, _ini_r[i], _ini_g[i], _ini_b[i]);
    }

    // Boundaries check
    _tg_brightness = constrain(tg_brightness, 0, 100);
    _rampTime = max(0, rampTime);

    // If no ramp time, set the brightness
    if (rampTime == 0)
        _brightness = _tg_brightness;
    return;
}

void FiringRod::strobe()
{

    if (_currentTime - _prevUpdate > _strobeSpeed)
    {
        _strobeSpeed = random(_updateSpeed, 50);

        _prevUpdate = _currentTime;

        _hue = _getHue();
        uint8_t r, g, b;
        _hueToRGB(_hue, r, g, b);

        if (_brightness != _tg_brightness)
            _brightness = _rampParameter(_iniTime, _rampTime, _ini_brightness, _tg_brightness, _updateSpeed);

        // Select the LED(s) to update
        int ledIndex = _shuffle ? random(0, *P_NUMLEDS - 1) : -1;

        for (uint8_t i = 0; i < *P_NUMLEDS; i++)
        {
            if (_shuffle && i != ledIndex)
                continue; // Only update one LED in shuffle mode

            _setColor(*P_START+i,
                      _randomScaledBrightness(r),
                      _randomScaledBrightness(g),
                      _randomScaledBrightness(b));

            if (_shuffle)
                break; // Exit after updating one LED
        }
    }
}

uint8_t FiringRod::_randomScaledBrightness(uint8_t colorComponent)
{
    return (_brightness * random(FIRE_STROBE_WHITE_COMPONENT, max(FIRE_STROBE_WHITE_COMPONENT, colorComponent))) / 100;
}

uint8_t FiringRod::_getHue()
{
    if (!_POT_ENABLE)
        return DEFAULT_HUE;

    int potValue = analogRead(_POT_PIN);
    uint8_t hue = map(potValue, 0, 1023, 0, 255);

    return hue;
}

void FiringRod::_hueToRGB(uint8_t hue, uint8_t &r, uint8_t &g, uint8_t &b)
{
    if (hue < 43) // Red to Blue transition (Purple)
    {
        r = 255;
        g = 0;
        b = map(hue, 0, 42, 0, 255);
    }
    else if (hue < 85) // Blue tone
    {
        r = map(hue, 43, 84, 255, 0);
        g = 0;
        b = 255;
    }
    else if (hue < 127) // Green/Blue transition (Cyan)
    {
        r = 0;
        g = map(hue, 85, 126, 0, 255);
        b = 255;
    }
    else if (hue < 169) // Green tone
    {
        r = 0;
        g = 255;
        b = map(hue, 127, 168, 255, 0);
    }
    else if (hue < 211) // Green to Red transition (Yellow)
    {
        r = map(hue, 169, 210, 0, 255);
        g = 255;
        b = 0;
    }
    else // Red tone
    {
        r = 255;
        g = map(hue, 211, 255, 255, 0);
        b = 0;
    }
}



