/*
 *  WB_LedsStripEngine.h is a part of SBK_WRISTBLASTER_CORE (VERSION 0) code for bar meter animations of a Wrist Blaster replica
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

#ifndef WB_LEDSSTRIPENGINE_H
#define WB_LEDSSTRIPENGINE_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

/*******************************/
/*    SOME GENERAL HELPERS     */
/*******************************/
#ifndef DISABLE
#define DISABLE 0
#endif
#ifndef ENABLE
#define ENABLE 1
#endif
#ifndef SOLID
#define SOLID 0
#endif
#ifndef NO_RAMP
#define NO_RAMP 0
#endif
#ifndef NO_FADE
#define NO_FADE 0
#endif
// Some colors definition
const uint8_t RED[3] = {255, 0, 0};
const uint8_t WHITE[3] = {255, 255, 255};
const uint8_t WARM_WHITE[3] = {255, 200, 150};
const uint8_t BLUE[3] = {0, 0, 255};
const uint8_t YELLOW[3] = {255, 200, 0};
const uint8_t GREEN[3] = {12, 189, 24};
const uint8_t ORANGE[3] = {255, 100, 0};
const uint8_t COOL_BLUE[3] = {50, 50, 255};
const uint8_t BLACK[3] = {0, 0, 0};

class LedsStrip
{
public:
    LedsStrip(Adafruit_NeoPixel *strip);
    virtual ~LedsStrip() = default;

    bool update();
    bool update(uint32_t syncCurrentTime);

protected:
    void _setColorAll(uint8_t start, uint8_t end, uint8_t red, uint8_t green, uint8_t blue);
    void _setColor(uint8_t pixel, uint8_t red, uint8_t green, uint8_t blue);
    uint16_t _rampParameter(uint32_t iniTime, uint16_t rampTime, uint16_t iniPara, uint16_t tgPara, uint16_t updateSpeed);
    void _getCurrentColor(uint8_t pixel, uint8_t &red, uint8_t &green, uint8_t &blue);
    void _clearStrip();
    void _clearPixel(uint8_t pixel);
    void _clearSomePixels(uint8_t start, uint8_t end);

    Adafruit_NeoPixel *_strip; // Pointer to shared NeoPixel instance
    bool _updateRequired;
    uint32_t _currentTime;
};

#endif