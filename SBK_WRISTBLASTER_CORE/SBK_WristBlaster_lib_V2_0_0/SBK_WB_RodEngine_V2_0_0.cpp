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

#include "SBK_WB_RodEngine_V2_0_0.h"

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
                     const uint8_t numLeds, const uint8_t start, const uint8_t end)
    : LedsStrip(strip),
      _POT_PIN(potPin),
      _POT_ENABLE(potEnable),
      _NUM_LEDS(numLeds), _START(start), _END(end),
      _tg_brightness(0), _brightness(0),
      _ini_brightness(0),
      _shuffle(true),
      _strobeSpeed(10),
      _hue(42) // Purple hue aka red and blue
{
}

void FiringRod::begin()
{
    pinMode(_POT_PIN, INPUT);
    clear();
}

void FiringRod::clear()
{
    _clearSomePixels(_START, _END);
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
    _iniTime = _now;
    _ini_brightness = _brightness;
    _shuffle = shuffle;

    // Read potentiometer and map it to a hue range (0-255)
    _hue = _getHue();
    uint8_t r, g, b;
    _hueToRGB(_hue, r, g, b);

    // Set initial colors
    for (uint8_t i = 0; i < _NUM_LEDS; i++)
    {
        const uint8_t initialRed = _randomScaledBrightness(r);
        const uint8_t initialGreen = _randomScaledBrightness(g);
        const uint8_t initialBlue = _randomScaledBrightness(b);

        _setColor(_START + i, initialRed, initialGreen, initialBlue);
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
    if (_now - _prevUpdate > _strobeSpeed)
    {
        _strobeSpeed = random(_updateSpeed, 50);
        _prevUpdate = _now;

        _hue = _getHue();
        uint8_t r, g, b;
        _hueToRGB(_hue, r, g, b);

        if (_brightness != _tg_brightness)
            _brightness = _rampParameter(_iniTime, _rampTime, _ini_brightness, _tg_brightness, _updateSpeed);

        // Select the LED(s) to update
        int ledIndex = _shuffle ? random(0, _NUM_LEDS) : -1;

        for (uint8_t i = 0; i < _NUM_LEDS; i++)
        {
            if (_shuffle && i != ledIndex)
                continue; // Only update one LED in shuffle mode

            _setColor(_START+i,
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

    // Convert the 10-bit ADC reading to an 8-bit hue by discarding its two
    // least-significant bits. This replaces map(0..1023, 0..255).
    return analogRead(_POT_PIN) >> 2;
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



