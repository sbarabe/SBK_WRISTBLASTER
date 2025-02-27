/*
 *  WB_CyclotronEngine.h is a part of SBK_WRISTBLASTER_CORE (Version 0) code for animations of a Wrist Blaster replica
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

#ifndef WB_CYCLOTRONENGINE_H
#define WB_CYCLOTRONENGINE_H

#include <Arduino.h>
#include "WB_LedsStripEngine.h"

/* GENERAL HELPERS */
#ifndef DISABLE
#define DISABLE 0
#endif

#ifndef ENABLE
#define ENABLE 1
#endif

// Cyclotron animation parameters structure : cycle speed in mHz, min brigthness, max brigthness
struct CycParams
{
    uint16_t cycle_mHz;
    uint8_t minBrightness;
    uint8_t maxBrightness;
};

// Define the animation parameters as constants
const CycParams CYC_OFF = {250, 0, 0};
const CycParams CYC_ON = {1000, 0, 204};
const CycParams CYC_FULL = {4000, 0, 204};
const CycParams CYC_CAPTURE_MAX = {3000, 25, 204};
const CycParams CYC_CAPTURE_WARNING = {4000, 75, 255};
const CycParams CYC_BURST_MAX = {6000, 50, 204};
const CycParams CYC_BURST_WARNING = {8000, 100, 255};

class Cyclotron : public LedsStrip
{
public:
    Cyclotron(Adafruit_NeoPixel *strip,const uint8_t *numLed, const uint8_t *start, const uint8_t *end,
              const uint8_t *ring1st, const uint8_t *ringLast, const uint8_t *center,
              const bool *direction);
    ~Cyclotron();
    void begin();
    void clear();
    void rampInit(const CycParams &tg_params, uint16_t rampTime);
    void ramp();

private:
    const uint8_t *P_NUMLEDS, *P_START, *P_END;
    const uint8_t *P_RING_1ST, *P_RING_LAST, *P_CENTER;
    const bool *P_DIRECTION;
    uint32_t _prevUpdate;
    uint8_t _minBrightness;
    uint8_t _maxBrightness;
    uint16_t _cycle_mHz;
    uint8_t _updateSpeed;
    bool _update;
    uint16_t _ini_mHz;  //, _incr_mHz;
    uint8_t _ini_minBr; //, _incr_minBr;
    uint8_t _ini_maxBr; //, _incr_maxBr;
    uint16_t _tg_cycle_mHz;
    uint8_t _tg_minBrightness;
    uint8_t _tg_maxBrightness;
    uint32_t _iniTime;
    uint16_t _rampTime;

    void _setColor_(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue);
    void _rotation(uint16_t cycle_mHz, uint8_t minBrightness, uint8_t maxBrightness);
};

#endif
