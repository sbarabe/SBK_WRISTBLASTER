/*
 *  WB_CyclotronEngine.cpp is a part of SBK_WRISTBLASTER_CORE (Version 0) code for animations of a Wrist Blaster replica
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

#include "WB_CyclotronEngine.h"

/* DEBUG MESSAGES TO SERIAL */
// comment/uncomment #define DEBUG_TO_SERIAL to receive serial message

// #define DEBUG_TO_SERIAL
#ifdef DEBUG_TO_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

// Some constants values for animations

// Cyclotron object and functions

Cyclotron::Cyclotron(Adafruit_NeoPixel *strip,
                     const uint8_t *numLed, const uint8_t *start, const uint8_t *end,
                     const uint8_t *ring1st, const uint8_t *ringLast, const uint8_t *center,
                     const bool *direction)
    : LedsStrip(strip),
      P_NUMLEDS(numLed), P_START(start), P_END(end),
      P_RING_1ST(ring1st), P_RING_LAST(ringLast), P_CENTER(center),
      P_DIRECTION(direction),
      _prevUpdate(0),
      _minBrightness(0), _maxBrightness(0), _cycle_mHz(100),
      _updateSpeed(10),
      _update(true),
      _ini_mHz(100),   //_incr_mHz(5),
      _ini_minBr(0),   //_incr_minBr(5),
      _ini_maxBr(255), //_incr_maxBr(5),
      _tg_cycle_mHz(100),
      _tg_minBrightness(0),
      _tg_maxBrightness(255),
      _iniTime(0), _rampTime(0)
{
}

Cyclotron::~Cyclotron()
{
}

void Cyclotron::begin()
{
    DEBUG_PRINTLN("Cyclotron DEBUG ON");

    Cyclotron::clear();
}

void Cyclotron::clear()
{
    _clearSomePixels(*P_START, *P_END);
}

void Cyclotron::rampInit(const CycParams &tg_params, uint16_t rampTime)
{
    _iniTime = _currentTime;
    _rampTime = rampTime;

    _tg_cycle_mHz = max(0, min(8000, tg_params.cycle_mHz));
    _ini_mHz = _cycle_mHz;

    _tg_minBrightness = constrain(tg_params.minBrightness, 0, 255);
    _ini_minBr = _minBrightness;

    _tg_maxBrightness = constrain(tg_params.maxBrightness, 0, 255);
    _ini_maxBr = _maxBrightness;

    /*
    DEBUG_PRINTLN("Cyclotron rampInit :");
    DEBUG_PRINTLN("_iniTime " + String(_iniTime) + "  _rampTime " + String(_rampTime));
    DEBUG_PRINTLN("_tg_cycle_mHz " + String(_tg_cycle_mHz) + "  _ini_mHz " + String(_ini_mHz));
    DEBUG_PRINTLN("_tg_minBrightness " + String(_tg_minBrightness) + "  _ini_minBr " + String(_ini_minBr));
    DEBUG_PRINTLN("_tg_maxBrightness " + String(_tg_maxBrightness) + "  _ini_maxBr " + String(_ini_maxBr));
    DEBUG_PRINTLN();
    */
}

void Cyclotron::ramp()
{

    // Check if it's time to update the animation
    if (_currentTime - _prevUpdate >= _updateSpeed)
    {
        _prevUpdate = _currentTime;

        //_rotation(tg_cycle_mHz, tg_minBrightness, tg_maxBrightness);
        _rotation((uint16_t)_cycle_mHz, (uint8_t)_minBrightness, (uint8_t)_maxBrightness);

        if ((_cycle_mHz == _tg_cycle_mHz) && (_minBrightness == _tg_minBrightness) && (_maxBrightness == _tg_maxBrightness))
            return;

        if (_rampTime == 0)
        {
            _cycle_mHz = _tg_cycle_mHz;
            _minBrightness = _tg_minBrightness;
            _maxBrightness = _tg_maxBrightness;
            return;
        }

        // Ramp cyclotron
        _cycle_mHz = _rampParameter(_iniTime, _rampTime, _ini_mHz, _tg_cycle_mHz, _updateSpeed);
        _minBrightness = _rampParameter(_iniTime, _rampTime, _ini_minBr, _tg_minBrightness, _updateSpeed);
        _maxBrightness = _rampParameter(_iniTime, _rampTime, _ini_maxBr, _tg_maxBrightness, _updateSpeed);
        // DEBUG_PRINTLN("_cycle_mHz " + String(_cycle_mHz) + "  _minBrightness " + String(_minBrightness) + "  _maxBrightness " + String(_maxBrightness));
    }
}

void Cyclotron::_rotation(uint16_t cycle_mHz, uint8_t minBrightness, uint8_t maxBrightness)
{
    // Clear all LEDs before updating
    _setColorAll(*P_START, *P_END, 0, 0, 0);

    // Number of LEDs in the ring
    uint8_t ringSize = *P_RING_LAST - *P_RING_1ST + 1;

    // Track phase as a continuously increasing value
    static float phase = 0.0;

    // Compute elapsed time since last update
    static uint32_t lastUpdateTime = 0;
    float deltaTime = (_currentTime - lastUpdateTime) / 1000.0; // Convert to seconds
    lastUpdateTime = _currentTime;

    // Increment phase smoothly, handling speed changes
    phase += deltaTime * (cycle_mHz / 1000.0); // Convert mHz to Hz

    // Ensure phase stays within [0,1] range
    phase = fmod(phase, 1.0);

    // Loop through all ring LEDs
    for (uint8_t i = 0; i < ringSize; i++)
    {
        // Calculate offset per LED
        float ledOffset = (float)i / (ringSize);
        float relativePhase = phase - ledOffset;

        // Ensure relativePhase stays within [0,1] range
        if (relativePhase < 0)
            relativePhase += 1.0;

        // Apply asymmetric sine wave for trailing fade-out effect
        float fadeSharpness = 4.0; // Minimum value = 1.0, recommended 2.5 to 5.0, maximum 7.0 to 8.0
        float fadeFactor;
        if (relativePhase < 0.2)
        {
            // Fade-in effect for the first two pixels
            fadeFactor = pow(relativePhase / 0.2, fadeSharpness);
        }
        else
        {
            // Trailing fade-out curve
            fadeFactor = pow(0.5 * (1 + cos(PI * (relativePhase - 0.2) / 0.8)), fadeSharpness);
        }

        // Scale the fadeFactor with a minimum brightness base
        float brightnessFactor = minBrightness + (maxBrightness - minBrightness) * fadeFactor;

        // Apply brightness scaling
        uint8_t brightness = (uint8_t)(brightnessFactor);
        uint8_t red = brightness;
        // uint8_t green = brightness / 40; // Slight orange tint

        // Set color
        _setColor_(*P_RING_1ST + i, red, 0 /*green*/, 0);
    }

    _setColor_(*P_CENTER, minBrightness, minBrightness / 40, 0);
}

void Cyclotron::_setColor_(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue)
{

    // Adjust the pixel index based on the direction
    uint8_t adjustedPixel = (!*P_DIRECTION) ? (*P_START + pixel) : (*P_END - pixel);

    // If the color is different, set the new color
    _setColor(adjustedPixel, red, green, blue);
}
