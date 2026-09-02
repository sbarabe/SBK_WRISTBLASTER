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
 **/

#include "SBK_WB_BarMeterBaseEngine_V1_1_0.h"

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

BarMeter::BarMeter(BarMeterDriver *driver,
                   const uint8_t address,
                   const uint8_t *segNumber,
                   const bool *direction,
                   const uint8_t (*mapping)[2])
    : _driver(driver),
      _ADDRESS(address),
      P_SEG_NUMBER(segNumber),
      P_DIRECTION(direction),
      _SEG_MAP(mapping),
      _currentTime(0),
      _ledsStatesLow(0),
      _ledsStatesHigh(0),
      _brightness(3),
      _brightnessPrev(25),
      _direction(false),
      _updateInterval(10), _corrSpeed(10),
      _update(true),
      _duration(0),
      _iniTime(0), _ini_speed(0), _tg_speed(0)
{
    _driver->clear(_ADDRESS);
}

BarMeter::~BarMeter()
{
    _driver = nullptr; // Just nullify the pointer if needed
}

void BarMeter::update()
{
    update(millis());
}

void BarMeter::update(uint32_t syncCurrentTime)
{
    _currentTime = syncCurrentTime;

    // Update only if required
    if (_update)
    {
        // Reset update required tracker
        _update = false;

        // Update brightness id needed
        if (_brightness != _brightnessPrev)
        {
            _brightnessPrev = _brightness;

            _driver->setBrightness(_ADDRESS, _brightness);
        }

        // To be configure for in relation with bar meter total leds number and connections matrix to the MAX72xx
        // Leds mapping might be different for your setup, check rows and columns orders : _driver.setLed(0, ROW, COL, _ledsStates[i]))
        for (uint8_t i = 0; i < *P_SEG_NUMBER; i++)
        {
            // Check if animation is REVERSED
            uint8_t j = *P_DIRECTION ? (*P_SEG_NUMBER - 1 - i) : i; // DIRECTION : false = forward, true = reverse

            _driver->setLed(_ADDRESS, _SEG_MAP[j][0], _SEG_MAP[j][1], _getLedState(i));
        }
        _driver->show(_ADDRESS);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*     Private short sequences use in Animations         */  /////////////////////////////////////////////////
/*        (these functions call for an update)            */ /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t BarMeter::_rampParameter(uint32_t iniTime, uint16_t rampTime, uint16_t iniPara, uint16_t tgPara, uint16_t updateSpeed)
{
    // No ramping needed if the values are the same
    if (iniPara == tgPara)
        return iniPara;

    // Ensure rampDuration is at least 1 to avoid division by zero
    // And apply a small offset to make sure ramp is done before rampTime...
    uint16_t correctedRampTime = max(5, rampTime - (updateSpeed * 2));

    // Map the timeElapsed to the new value and constrain to prevent overshooting
    uint16_t mappedValue = constrain(map(_currentTime - iniTime, 0, correctedRampTime, iniPara, tgPara),
                                     min(iniPara, tgPara),
                                     max(iniPara, tgPara));

    return mappedValue;
}

uint8_t BarMeter::_getLedState(uint8_t index)
{
    // If out of bound return 0
    if (index >= *P_SEG_NUMBER)
    {
        return 0;
    }

    if (index < 16)
    {
        return (_ledsStatesLow & (1 << index)) != 0;
    }
    else if (index < 32)
    {
        return (_ledsStatesHigh & (1 << (index - 16))) != 0;
    }
    else if (index < 48)
    {
        return (_ledsStatesExtraLow & (1 << (index - 32))) != 0;
    }
    else
    {
        return (_ledsStatesExtraHigh & (1 << (index - 48))) != 0;
    }
}

void BarMeter::_setHigh()
{
    _update = true; // update required

    // Set all 64 bits to 1, turning all LEDs on
    _ledsStatesLow = 0xFFFF;
    _ledsStatesHigh = 0xFFFF;
    _ledsStatesExtraLow = 0xFFFF;
    _ledsStatesExtraHigh = 0xFFFF;
}

void BarMeter::_setLow()
{
    _update = true; // update required

    // Set all 64 bits to 0, turning all LEDs off
    _ledsStatesLow = 0;
    _ledsStatesHigh = 0;
    _ledsStatesExtraLow = 0;
    _ledsStatesExtraHigh = 0;
}

void BarMeter::_setLed(uint8_t index, bool state)
{
    if (index < 16)
    {
        if (state)
        {
            _ledsStatesLow |= (1 << index);
        }
        else
        {
            _ledsStatesLow &= ~(1 << index);
        }
    }
    else if (index < 32)
    {
        uint8_t shiftedIndex = index - 16;
        if (state)
        {
            _ledsStatesHigh |= (1 << shiftedIndex);
        }
        else
        {
            _ledsStatesHigh &= ~(1 << shiftedIndex);
        }
    }
    else if (index < 48)
    {
        uint8_t shiftedIndex = index - 32;
        if (state)
        {
            _ledsStatesExtraLow |= (1 << shiftedIndex);
        }
        else
        {
            _ledsStatesExtraLow &= ~(1 << shiftedIndex);
        }
    }
    else if (index < 64)
    {
        uint8_t shiftedIndex = index - 48;
        if (state)
        {
            _ledsStatesExtraHigh |= (1 << shiftedIndex);
        }
        else
        {
            _ledsStatesExtraHigh &= ~(1 << shiftedIndex);
        }
    }
}