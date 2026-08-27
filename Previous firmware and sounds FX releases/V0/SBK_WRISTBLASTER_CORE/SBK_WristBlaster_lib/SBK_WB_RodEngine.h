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

#ifndef SBK_WB_RODENGINE_H
#define SBK_WB_RODENGINE_H

#include "Arduino.h"
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
    bool _shuffle;
    uint8_t _strobeSpeed;
    uint8_t _hue;
    uint8_t _getHue();
    void _hueToRGB(uint8_t hue, uint8_t &r, uint8_t &g, uint8_t &b);
    uint8_t _randomScaledBrightness(uint8_t colorComponent);
};

#endif
