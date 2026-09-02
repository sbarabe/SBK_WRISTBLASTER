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

#include "SBK_WB_LedsStripBaseEngine_V2_0_0.h"

/* DEBUG MESSAGES TO SERIAL */
// comment/uncomment #define DEBUG_TO_SERIAL to receive serial message

// #define DEBUG_TO_SERIAL
#ifdef DEBUG_TO_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

LedsStrip::LedsStrip(Adafruit_NeoPixel *strip)
    : _strip(strip),
      _updateRequired(true),
      _now(0), 
      _prevUpdate(0),
      _updateSpeed(10),
      _iniTime(0),
      _rampTime(0)       
{
}

bool LedsStrip::update() { return update(millis()); }

bool LedsStrip::update(uint32_t now)
{
    _now = now;

    if (!_updateRequired)
        return false;

    // The core combines every engine's dirty flag and transmits the shared
    // NeoPixel strip once after all engines have updated their pixels.
    _updateRequired = false;
    return true;
}

void LedsStrip::_clearStrip() { _setColorAll(0, _strip->numPixels() - 1, 0, 0, 0); }

void LedsStrip::_clearPixel(uint8_t pixel) { _setColor(pixel, 0, 0, 0); }

void LedsStrip::_clearSomePixels(uint8_t start, uint8_t end) { _setColorAll(start, end, 0, 0, 0); }

void LedsStrip::_setColorAll(uint8_t start, uint8_t end, uint8_t red, uint8_t green, uint8_t blue)
{
    end = min(end, _strip->numPixels() - 1);
    for (uint16_t i = start; i <= end ; i++)
    {
        _setColor(i, red, green, blue);
    }
}

void LedsStrip::_setColor(uint8_t pixel, uint8_t red, uint8_t green, uint8_t blue)
{
    if (pixel >= _strip->numPixels())
        return;

    // Read through Adafruit_NeoPixel so every RGB and RGBW byte order remains
    // supported.
    uint8_t currentRed, currentGreen, currentBlue;
    _getCurrentColor(pixel, currentRed, currentGreen, currentBlue);

    // Check if the current color is different from the new one
    if (currentRed != red || currentGreen != green || currentBlue != blue)
    {
        _strip->setPixelColor(pixel, red, green, blue);

        // Mark that an update is needed
        _updateRequired = true;
    }
}

uint16_t LedsStrip::_rampParameter(uint32_t iniTime, uint16_t rampTime, uint16_t iniPara, uint16_t tgPara, uint16_t updateSpeed)
{
    // No ramping needed if the values are the same
    if (iniPara == tgPara)
        return iniPara;

    // Ensure rampDuration is at least 1 to avoid division by zero
    // And apply a small offset to make sure ramp is done before rampTime...
    const uint16_t correction = (uint16_t)updateSpeed * 2U;
    const uint16_t correctedRampTime = rampTime > correction + 5U ? rampTime - correction : 5U;

    const uint32_t elapsed = _now - iniTime;
    if (elapsed >= correctedRampTime)
        return tgPara;

    const bool increasing = iniPara < tgPara;
    const uint16_t difference = increasing ? tgPara - iniPara : iniPara - tgPara;
    const uint16_t change = ((uint32_t)difference * elapsed) / correctedRampTime;

    return increasing ? iniPara + change : iniPara - change;
}

void LedsStrip::_getCurrentColor(uint8_t pixel, uint8_t &red, uint8_t &green, uint8_t &blue)
{
    const uint32_t color = _strip->getPixelColor(pixel);
    red = (color >> 16) & 0xFF;
    green = (color >> 8) & 0xFF;
    blue = color & 0xFF;
}
