 /**
 *  This code is part of SBK_WRISTBLASTER_CORE, a codebase for animations and effects
 *  of a Wrist Blaster prop inspired by the movie Ghostbusters: Frozen Empire.
 * 
 *  @author      Samuel Barabé  
 *  @copyright   Copyright (c) 2025-2026 Samuel Barabé  
 *  @license     MIT License (code)  
 *  @version     1.1.0
 *  @link        https://github.com/sbarabe/SBK_WRISTBLASTER/tree/main/SBK_WRISTBLASTER_CORE
 *
 *  For more information, visit the project page: <https://github.com/sbarabe/SBK_WRISTBLASTER/tree/main/SBK_WRISTBLASTER_CORE>.
 *
* @see         https://opensource.org/licenses/MIT
 *
 *  This code is provided "as-is" without any warranty of any kind, either expressed or implied,
 *  including but not limited to the warranties of merchantability or fitness for a particular purpose.
 */

#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "SBK_WB_LedsStripBaseEngine_V1_1_0.h"

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
