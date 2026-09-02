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

#include "SBK_WB_CyclotronEngine_V2_0_0.h"

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

// Cyclotron object and functions

Cyclotron::Cyclotron(Adafruit_NeoPixel *strip,
                     const uint8_t numLed, const uint8_t start, const uint8_t end,
                     const uint8_t ring1st, const uint8_t ringLast, const uint8_t center,
                     const bool direction)
    : LedsStrip(strip),
      _NUM_LEDS(max((uint8_t)1,
                    (uint8_t)min((uint16_t)numLed,
                                 (uint16_t)max(start, end) - min(start, end) + 1U))),
      _START(min(start, end)), _END(max(start, end)),
      _RING_FIRST(min(min(ring1st, ringLast), (uint8_t)(_NUM_LEDS - 1))),
      _RING_LAST(min(max(ring1st, ringLast), (uint8_t)(_NUM_LEDS - 1))),
      _CENTER(min(center, (uint8_t)(_NUM_LEDS - 1))),
      _DIRECTION(direction),
      _cycle_mHz(100), _minBrightness(0), _maxBrightness(0),
      _ini_mHz(100), _ini_minBr(0), _ini_maxBr(255),
      _tg_cycle_mHz(100), _tg_minBrightness(0), _tg_maxBrightness(255)
{
}

void Cyclotron::begin()
{
    DEBUG_PRINTLN("Cyclotron DEBUG ON");

    Cyclotron::clear();
}

void Cyclotron::clear()
{
    _clearSomePixels(_START, _END);
}

void Cyclotron::rampInit(const CycParams &tg_params, uint16_t rampTime)
{
    _iniTime = _now;
    _rampTime = rampTime;

    _tg_cycle_mHz = min(8000, tg_params.cycle_mHz);
    _ini_mHz = _cycle_mHz;

    _tg_minBrightness = tg_params.minBrightness;
    _ini_minBr = _minBrightness;

    _tg_maxBrightness = tg_params.maxBrightness;
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
    if (_now - _prevUpdate >= _updateSpeed)
    {
        _prevUpdate = _now;

        if (_rampTime == 0)
        {
            _cycle_mHz = _tg_cycle_mHz;
            _minBrightness = _tg_minBrightness;
            _maxBrightness = _tg_maxBrightness;
        }
        else if ((_cycle_mHz != _tg_cycle_mHz) || (_minBrightness != _tg_minBrightness) || (_maxBrightness != _tg_maxBrightness))
        {
            // Calculate the current ramp values before drawing this frame.
            _cycle_mHz = _rampParameter(_iniTime, _rampTime, _ini_mHz, _tg_cycle_mHz, _updateSpeed);
            _minBrightness = _rampParameter(_iniTime, _rampTime, _ini_minBr, _tg_minBrightness, _updateSpeed);
            _maxBrightness = _rampParameter(_iniTime, _rampTime, _ini_maxBr, _tg_maxBrightness, _updateSpeed);
        }

        _rotation(_cycle_mHz, _minBrightness, _maxBrightness);
        // DEBUG_PRINTLN("_cycle_mHz " + String(_cycle_mHz) + "  _minBrightness " + String(_minBrightness) + "  _maxBrightness " + String(_maxBrightness));
    }
}

void Cyclotron::_rotation(uint16_t cycle_mHz, uint8_t minBrightness, uint8_t maxBrightness)
{
    // Continuous fixed-point implementation. One revolution is represented by
    // 1,000,000 phase units, preserving sub-pixel motion without float math.
    // This table samples the original pow/cos trail at 32 equal phase steps;
    // linear interpolation between samples prevents visible brightness steps.
    static const uint8_t fadeCurve[] PROGMEM = {
        0, 0, 2, 12, 39, 95, 197, 254, 245, 230, 209,
        185, 157, 130, 103, 79, 58, 41, 28, 18, 11, 6,
        3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    const uint8_t ringSize = _RING_LAST - _RING_FIRST + 1;
    static uint32_t phase = 0;
    static uint32_t lastUpdateTime = 0;

    const uint32_t deltaTime = _now - lastUpdateTime;
    lastUpdateTime = _now;

    // milliseconds * millihertz maps directly to the 1,000,000-unit phase.
    // Split long intervals so the multiplication remains within uint32_t.
    const uint32_t phaseIncrement =
        (deltaTime % 1000UL) * cycle_mHz +
        ((((deltaTime / 1000UL) % 1000UL) * cycle_mHz) % 1000UL) * 1000UL;
    phase = (phase + phaseIncrement) % 1000000UL;

    for (uint8_t i = 0; i < ringSize; i++)
    {
        const uint32_t ledOffset = ((uint32_t)i * 1000000UL) / ringSize;
        uint32_t relativePhase = phase + 1000000UL - ledOffset;
        if (relativePhase >= 1000000UL)
            relativePhase -= 1000000UL;

        const uint8_t curveIndex = relativePhase / 31250UL;
        const uint16_t curvePosition = relativePhase - (uint32_t)curveIndex * 31250UL;
        const uint8_t fadeStart = pgm_read_byte(&fadeCurve[curveIndex]);
        const uint8_t fadeEnd = pgm_read_byte(&fadeCurve[curveIndex + 1]);
        const int16_t fadeDelta = (int16_t)fadeEnd - fadeStart;
        const uint8_t fade = fadeStart +
                             ((int32_t)fadeDelta * curvePosition +
                              (fadeDelta >= 0 ? 15625L : -15625L)) /
                                 31250L;

        const uint8_t brightness = minBrightness +
                                   ((uint16_t)(maxBrightness - minBrightness) * fade + 127U) / 255U;
        _CycSetColor(_RING_FIRST + i, brightness, 0, 0);
    }

    _CycSetColor(_CENTER, minBrightness, minBrightness / 40U, 0);
#if 0
    // Reference only: original continuous floating-point animation.
    // Original continuous floating-point animation. This is intentionally
    // CPU-heavy, but its sub-pixel phase and curved trail remain smooth at
    // slow rotation speeds.
    const uint8_t ringSize = _RING_LAST - _RING_FIRST + 1;
    static float phase = 0.0f;
    static uint32_t lastUpdateTime = 0;

    const float deltaTime = (_now - lastUpdateTime) / 1000.0f;
    lastUpdateTime = _now;

    phase += deltaTime * (cycle_mHz / 1000.0f);
    phase = fmod(phase, 1.0f);

    for (uint8_t i = 0; i < ringSize; i++)
    {
        const float ledOffset = (float)i / ringSize;
        float relativePhase = phase - ledOffset;
        if (relativePhase < 0.0f)
            relativePhase += 1.0f;

        const float fadeSharpness = 4.0f;
        float fadeFactor;
        if (relativePhase < 0.2f)
        {
            fadeFactor = pow(relativePhase / 0.2f, fadeSharpness);
        }
        else
        {
            fadeFactor = pow(0.5f * (1.0f + cos(PI * (relativePhase - 0.2f) / 0.8f)),
                             fadeSharpness);
        }

        const float brightnessFactor = minBrightness +
                                       (maxBrightness - minBrightness) * fadeFactor;
        const uint8_t brightness = (uint8_t)brightnessFactor;
        _CycSetColor(_RING_FIRST + i, brightness, 0, 0);
    }

    _CycSetColor(_CENTER, minBrightness, minBrightness / 40U, 0);
#endif
}

void Cyclotron::_CycSetColor(uint8_t pixel, uint8_t red, uint8_t green, uint8_t blue)
{
    // Adjust the pixel index based on the direction
    const uint8_t adjustedPixel = _DIRECTION ? _END - pixel : _START + pixel;

    // If the color is different, set the new color
    _setColor(adjustedPixel, red, green, blue);
}
