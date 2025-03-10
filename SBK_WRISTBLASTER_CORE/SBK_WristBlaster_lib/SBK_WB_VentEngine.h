/*
 *  SBK_WB_VentEngine.h is a part of SBK_WRISTBLASTER_CORE (Version 0) code for animations of a Wrist Blaster replica
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

#ifndef SBK_WB_VENTENGINE_H
#define SBK_WB_VENTENGINE_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "SBK_WB_LedsStripBaseEngine.h"

/* GENERAL HELPERS */
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

class Vent : public LedsStrip
{
public:
    Vent(Adafruit_NeoPixel *strip, const uint8_t *pixel);
    void begin();
    void clear();
    void initParam(const uint8_t color[3], uint8_t tg_brightness);
    void initParam(const uint8_t color[3], uint8_t tg_brightness, int16_t rampTime);
    bool ramp();
    void flicker(uint8_t flickerAmount, uint16_t maxSpeed);
    void solid();

private:
    const uint8_t *P_PIXEL;
    uint8_t _rPrev, _gPrev, _bPrev;
    uint8_t _tg_brightness;
    uint8_t _ini_r, _tg_r;
    uint8_t _ini_g, _tg_g;
    uint8_t _ini_b, _tg_b;
    uint8_t _flickerSpeed;
};

#endif