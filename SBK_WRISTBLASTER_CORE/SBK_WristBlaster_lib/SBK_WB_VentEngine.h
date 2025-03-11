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