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
#ifndef NO_SYNC
#define NO_SYNC 0
#endif
#ifndef NO_FADE
#define NO_FADE 0
#endif
#ifndef DISABLE_RAMP
#define DISABLE_RAMP 0
#endif
#ifndef ENABLE_RAMP
#define ENABLE_RAMP 1
#endif


class Indicator : public LedsStrip
{
public:
    Indicator(Adafruit_NeoPixel *strip, uint8_t pixel);
    void begin();
    void clear();
    void initParam(const uint8_t color[3], uint8_t tg_brightness);
    void initParam(const uint8_t color[3], uint8_t tg_brightness, uint16_t tg_blinkInt);
    void initParam(const uint8_t color[3], uint8_t tg_brightness, uint16_t solidMode, uint16_t rampTime);
    void initParam(const uint8_t color[3], uint8_t tg_brightness, uint16_t tg_blinkInt, uint32_t syncPrevBlink, bool syncPulse);
    void initParam(const uint8_t color[3], uint8_t tg_brightness, uint16_t tg_blinkInt, uint16_t rampTime, uint32_t syncPrevBlink, bool syncPulse);
    void solid();
    void blink(bool enableBlinkIntRamp);
    void flash(uint16_t flashInterval); // Fast flash at interval
    bool ramp();
    uint32_t getPrevBlink() const;
    bool getPulse() const;
    

private:
    const uint8_t _PIXEL;
    uint8_t _tg_r, _ini_r;
    uint8_t _tg_g, _ini_g;
    uint8_t _tg_b, _ini_b;
    bool _pulse;
    uint32_t _prevBlink;
    uint16_t _blinkInt;
    bool _wasBlinking;
    uint16_t _ini_blinkInt, _tg_blinkInt;
};

class SingleColorIndicator
{
public:
    SingleColorIndicator(uint8_t indicator_pin, bool enable);
    void begin();
    bool update();
    bool update(uint32_t now);
    void clear();
    void on();
    void off();
    void blinkInit(uint16_t blinkInterval); // flashing
    void blink();

private:
    const uint8_t _PIN;
    const bool _ENABLE;
    uint32_t _now;
    bool _state;
    bool _update; // True if update required
    uint32_t _prevUpdate;
    bool _pulse;
    void _write(bool state);
    uint16_t _blinkInt;
    bool _wasBlinking;
};

