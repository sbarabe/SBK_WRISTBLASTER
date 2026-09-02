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

#include <Arduino.h>
#include "SBK_WB_LedsStripBaseEngine_V2_0_0.h"

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
    Cyclotron(Adafruit_NeoPixel *strip, const uint8_t numLed, const uint8_t start, const uint8_t end,
              const uint8_t ring1st, const uint8_t ringLast, const uint8_t center,
              const bool direction);
    void begin();
    void clear();
    void rampInit(const CycParams &tg_params, uint16_t rampTime);
    void ramp();

private:
    const uint8_t _NUM_LEDS, _START, _END;
    const uint8_t _RING_FIRST, _RING_LAST, _CENTER;
    const bool _DIRECTION;
    uint16_t _cycle_mHz;
    uint8_t _minBrightness;
    uint8_t _maxBrightness;
    uint16_t _ini_mHz;  //, _incr_mHz;
    uint8_t _ini_minBr; //, _incr_minBr;
    uint8_t _ini_maxBr; //, _incr_maxBr;
    uint16_t _tg_cycle_mHz;
    uint8_t _tg_minBrightness;
    uint8_t _tg_maxBrightness;


    void _CycSetColor(uint8_t pixel, uint8_t red, uint8_t green, uint8_t blue);
    void _rotation(uint16_t cycle_mHz, uint8_t minBrightness, uint8_t maxBrightness);
};


