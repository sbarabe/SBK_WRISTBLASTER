/*
 *  SBK_WB_LedsStripBaseEngine.cpp is a part of SBK_WRISTBLASTER_CORE (VERSION 0) code for bar meter animations of a Wrist Blaster replica
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

#include "SBK_WB_LedsStripBaseEngine.h"

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
      _currentTime(0), 
      _prevUpdate(0),
      _updateSpeed(10),
      _iniTime(0),
      _rampTime(0)       
{
}

bool LedsStrip::update() { return update(millis()); }

bool LedsStrip::update(uint32_t syncCurrentTime)
{
    _currentTime = syncCurrentTime;

    if (!_updateRequired)
        return false;

    _strip->show();
    // Reset update tracker
    _updateRequired = false;
    return true;
}

void LedsStrip::_clearStrip() { _setColorAll(0, _strip->numPixels() - 1, 0, 0, 0); }

void LedsStrip::_clearPixel(uint8_t pixel) { _setColor(pixel, 0, 0, 0); }

void LedsStrip::_clearSomePixels(uint8_t start, uint8_t end) { _setColorAll(start, end, 0, 0, 0); }

void LedsStrip::_setColorAll(uint8_t start, uint8_t end, uint8_t red, uint8_t green, uint8_t blue)
{
    start = max(0, start);
    end = min(end, _strip->numPixels() - 1);
    for (uint16_t i = start; i < end + 1; i++)
    {
        _setColor(i, red, green, blue);
    }
}

void LedsStrip::_setColor(uint8_t pixel, uint8_t red, uint8_t green, uint8_t blue)
{
    if (pixel >= _strip->numPixels())
        return;

    // Extract the individual color components from the packed value
    uint8_t currentRed, currentGreen, currentBlue;
    _getCurrentColor(pixel, currentRed, currentGreen, currentBlue);

    // Check if the current color is different from the new one
    if (currentRed != red || currentGreen != green || currentBlue != blue)
    {
        // If the color is different, set the new color
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
    uint16_t correctedRampTime = max(5, rampTime - (updateSpeed * 2));

    // Map the timeElapsed to the new value and constrain to prevent overshooting
    uint16_t mappedValue = constrain(map(_currentTime - iniTime, 0, correctedRampTime, iniPara, tgPara),
                                     min(iniPara, tgPara),
                                     max(iniPara, tgPara));

    return mappedValue;
}

void LedsStrip::_getCurrentColor(uint8_t pixel, uint8_t &red, uint8_t &green, uint8_t &blue)
{
    uint32_t color = _strip->getPixelColor(pixel);
    red = (color >> 16) & 0xFF;
    green = (color >> 8) & 0xFF;
    blue = color & 0xFF;
}