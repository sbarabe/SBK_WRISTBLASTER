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

#pragma once

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "SBK_WB_LedsStripBaseEngine_V2_0_0.h"

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
              const uint8_t numLeds, const uint8_t start, const uint8_t end);
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
    const uint8_t _NUM_LEDS, _START, _END;
    uint8_t _tg_brightness, _brightness;
    uint8_t _ini_brightness;
    bool _shuffle;
    uint8_t _strobeSpeed;
    uint8_t _hue;
    uint8_t _getHue();
    void _hueToRGB(uint8_t hue, uint8_t &r, uint8_t &g, uint8_t &b);
    uint8_t _randomScaledBrightness(uint8_t colorComponent);
};

