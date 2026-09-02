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

/**********************************************************************
 *
 *  This library is a modified version of the original HT16K33 library
 *  from this GitHub repository:
 *  <https://github.com/MikeS11/ProtonPack/tree/master/Source/Libraries/ht16k33-arduino-master>
 *
 *  The purpose of this modified library is solely to drive the bar meter
 *  using the HT16K33 I2C LED driver for the SBK Wrist Blaster project.
 *
 *  All credit for the original library goes to Mike S11.
 *
 **********************************************************************/

#pragma once

#include "Arduino.h"
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
    Indicator(Adafruit_NeoPixel *strip, const uint8_t *pixel, const char *name);
    void begin();
    void clear();
    void initParam(const uint8_t color[3], uint8_t tg_brightness);
    void initParam(const uint8_t color[3], uint8_t tg_brightness, uint16_t tg_blinkInt);
    void initParam(const uint8_t color[3], uint8_t tg_brightness, uint16_t tg_blinkInt, uint16_t rampTime);
    void initParam(const uint8_t color[3], uint8_t tg_brightness, uint16_t tg_blinkInt, uint32_t syncPrevBlink, bool syncPulse);
    void initParam(const uint8_t color[3], uint8_t tg_brightness, uint16_t tg_blinkInt, uint16_t rampTime, uint32_t syncPrevBlink, bool syncPulse);
    void solid();
    void blink(bool enableBlinkIntRamp);
    void flash(uint16_t flashInterval); // Fast flash at interval
    void rampInit();
    bool ramp();
    uint32_t getPrevBlink() const;
    bool getPulse() const;
    

private:
    const uint8_t *P_PIXEL;
    const char *_NAME; // Store name
    uint8_t _tg_r, _ini_r;
    uint8_t _tg_g, _ini_g;
    uint8_t _tg_b, _ini_b;
    uint8_t _tg_brightness;
    bool _pulse;
    uint32_t _prevBlink;
    uint16_t _blinkInt;
    bool _wasBlinking;
    uint16_t _ini_blinkInt, _tg_blinkInt;
};

class SingleColorIndicator
{
public:
    SingleColorIndicator(const uint8_t indicator_pin, const bool enable, const char *name);
    void begin();
    bool update();
    bool update(uint32_t syncCurrentTime);
    void clear();
    void on();
    void off();
    void blinkInit(uint16_t blinkInterval); // flashing
    void blink();

private:
    const uint8_t _PIN;
    const bool _ENABLE;
    const char *_NAME;
    uint32_t _currentTime;
    bool _state;
    bool _update; // True if update required
    uint32_t _prevUpdate;
    bool _pulse;
    void _write(bool state);
    uint16_t _blinkInt;
    bool _wasBlinking;
};

