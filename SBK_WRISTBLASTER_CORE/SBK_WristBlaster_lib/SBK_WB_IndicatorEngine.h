/*
 *  SBK_WB_IndicatorEngine.h is a part of SBK_WRISTBLASTER_CORE (Version 0) code for animations of a Wrist Blaster replica
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

#ifndef SBK_WB_INDICATORENGINE_H
#define SBK_WB_INDICATORENGINE_H

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

#endif