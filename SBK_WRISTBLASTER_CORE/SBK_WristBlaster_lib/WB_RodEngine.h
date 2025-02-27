/*
 *  WB_RodEngine.h is a part of SBK_WRISTBLASTER_CORE (Version 0) code for animations of a Wrist Blaster replica
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

#ifndef WB_RODENGINE_H
#define WB_RODENGINE_H

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "WB_LedsStripEngine.h"

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
// Leds shuffling while strobbing
#ifndef NO_SHUFFLE
#define NO_SHUFFLE 0
#endif
#ifndef SHUFFLE
#define SHUFFLE 1
#endif

class FiringRod : public LedsStrip
{
public:
    FiringRod(Adafruit_NeoPixel *strip,
              const uint8_t potPin, const bool potEnable,
              const uint8_t *numLeds, const uint8_t *start, const uint8_t *end);
    ~FiringRod();
    void begin();
    void clear();
    void strobeInit();
    void strobeInit(bool random);
    void strobeInit(bool random, uint8_t tg_brightness);
    void strobeInit(bool random, uint8_t tg_brightness, uint16_t rampTime);
    void strobe();

private:
    const uint8_t _POT_PIN;
    const bool _POT_ENABLE;
    const uint8_t *P_NUMLEDS, *P_START, *P_END;
    uint8_t *_ini_r, *_ini_g, *_ini_b;
    uint8_t _tg_brightness, _brightness;
    uint8_t _ini_brightness;
    uint16_t _rampTime;
    bool _shuffle;
    bool _update;
    uint32_t _prevUpdate;
    uint8_t _updateSpeed;
    uint32_t _iniTime;
    uint8_t _strobeSpeed;
    uint8_t _hue;
    uint8_t _getHue();
    void _hueToRGB(uint8_t hue, uint8_t &r, uint8_t &g, uint8_t &b);
    uint8_t _randomScaledBrightness(uint8_t colorComponent);
};

#endif
