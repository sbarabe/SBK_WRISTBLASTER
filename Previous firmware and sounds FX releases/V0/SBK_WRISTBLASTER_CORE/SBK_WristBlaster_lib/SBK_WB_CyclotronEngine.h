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

#ifndef SBK_WB_CYCLOTRONENGINE_H
#define SBK_WB_CYCLOTRONENGINE_H

#include <Arduino.h>
#include "SBK_WB_LedsStripBaseEngine.h"

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
    uint16_t _cycle_mHz;
    uint8_t _minBrightness;
    uint8_t _maxBrightness;
    uint16_t _ini_mHz;  //, _incr_mHz;
    uint8_t _ini_minBr; //, _incr_minBr;
    uint8_t _ini_maxBr; //, _incr_maxBr;
    uint16_t _tg_cycle_mHz;
    uint8_t _tg_minBrightness;
    uint8_t _tg_maxBrightness;


    void _CycSetColor(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue);
    void _rotation(uint16_t cycle_mHz, uint8_t minBrightness, uint8_t maxBrightness);
};

#endif
