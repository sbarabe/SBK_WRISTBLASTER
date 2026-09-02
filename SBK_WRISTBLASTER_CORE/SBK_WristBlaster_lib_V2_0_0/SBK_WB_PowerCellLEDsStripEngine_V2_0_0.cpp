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

#include "SBK_WB_PowerCellLEDsStripEngine_V2_0_0.h"

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

// Blue intensity for animation
const uint8_t POWERCELL_BLUE = 175;

PowerCell::PowerCell(Adafruit_NeoPixel *strip, const uint8_t numLeds, const uint8_t start, const uint8_t end, const bool direction)
    : LedsStrip(strip),
      _NUM_LEDS(numLeds), _START(start), _END(end),
      _DIRECTION(direction),
      _ini_speed(0), _tg_speed(PC_POWER_ON_UPDATE_INT),
      _tracker(0), _levelTracker(0),
      _bootState(false),
      _pulse(false),
      _flashInterval(5000),
      _lowBatt(false),
      _battRed(0), _battGreen(0), _battBlue(POWERCELL_BLUE),
      _battLevel(100), _battType(0)
{
    _updateSpeed = PC_POWER_ON_UPDATE_INT;
}

void PowerCell::begin(uint8_t batt_level, uint8_t battType)
{
    _battLevel = batt_level;
    _battType = battType;
    _getBattLevelColors();
    clear();
}

void PowerCell::clear()
{
    _clearSomePixels(_START, _END);
}

bool PowerCell::update(uint8_t batt_level) { return update(millis(), batt_level); }

bool PowerCell::update(uint32_t now, uint8_t batt_level)
{
    _now = now;

    _battLevel = batt_level;

    if (!_updateRequired)
        return false;

    // The core transmits this strip once after the engine reports it dirty.
    _updateRequired = false;
    return true;
}

void PowerCell::bootInit(uint16_t duration)
{

    if (_bootState)
        return;

    _rampTime = duration;
    _fallingPixelStackUp(_rampTime, true);
}

bool PowerCell::boot()
{

    if (!_bootState) // Pixels drop down PowerCell and pile up!
    {
        if (_fallingPixelStackUp(_rampTime, false))
        {
            _bootState = true;
            _updateSpeed = PC_POWER_ON_UPDATE_INT;
            initSpeedRamp(_updateSpeed, 0);
        }
    }
    else
    {
        // boot sequence done, go to regular animation,
        _updateSpeed = PC_POWER_ON_UPDATE_INT;
        updateSpeedRamp();
    }
    return _bootState;
}

void PowerCell::initSpeedRamp(uint16_t targetSpeed, uint16_t rampTime)
{
    _iniTime = _now;
    _rampTime = rampTime;

    _tg_speed = constrain(targetSpeed, 5, PC_POWER_ON_UPDATE_INT);
    _ini_speed = _updateSpeed;

    _bootState = true;

    updateSpeedRamp();
}

void PowerCell::updateSpeedRamp()
{
    // Once the requested speed has been reached there is no ramp left to
    // calculate. The animation itself must continue updating below.
    if (_updateSpeed != _tg_speed)
        _updateSpeed = _rampParameter(_iniTime, _rampTime, _ini_speed, _tg_speed, PC_FIRING_MAX_UPDATE_INT);

    // If fill up sequence done, reset sequence
    if (_fillUp(false))
        _fillUp(true);
}

void PowerCell::shutDownInit(int16_t duration)
{
    _rampTime = duration;
    _risingPixelStackDown(_rampTime, true);
}

bool PowerCell::shutDown()
{

    if (_risingPixelStackDown(_rampTime, false))
        _bootState = false;

    return _bootState;
}

void PowerCell::poweredDownInit(uint16_t flashInterval)
// All bar graph pixels are OFF execpt pixels one blinking
{
    _flashInterval = flashInterval;

    clear();
    _prevUpdate = _now;
    _pulse = false;
}

void PowerCell::poweredDown()
// All bar graph pixels are OFF execpt pixels one blinking
{
    if (_now - _prevUpdate >= _flashInterval)
    {
        _prevUpdate = _now;

        _pulse = false;
    }

    // Turn off LED after short flash
    if (_now - _prevUpdate >= _flashInterval - 50)
    {
        // clear();
        _pulse = true;
    }

    // A flash of green color on first pixel
    _PwrCellSetColor(0, _pulse * 250, _pulse * 0, _pulse * 0);
}

void PowerCell::lowBattInit(uint16_t blinkInterval)
{

    clear();
    _prevUpdate = _now;
    _pulse = false;
    _flashInterval = blinkInterval;
}

void PowerCell::lowBatt()
{
    if (_now - _prevUpdate >= _flashInterval)
    {
        // Boot sequence not done
        _prevUpdate = _now;

        _pulse = !_pulse;
    }

    _PwrCellSetColor(_START, _pulse * 125, 0, 0);
}

bool PowerCell::_fallingPixelStackUp(uint16_t duration, bool initialize)
{
    if (initialize)
    {
        _tracker = _NUM_LEDS;
        _levelTracker = 0;
        _rampTime = duration;
        uint16_t totalSteps = 1 + (_NUM_LEDS + 1) * (_NUM_LEDS + 2) / 2;
        _updateSpeed = max(5, duration / totalSteps);

        _setColorAll(_START, _END, 0, 0, 0);
        _getBattLevelColors();
        return false;
    }

    if (_levelTracker > _NUM_LEDS)
    {
        return true;
    }

    if (_now - _prevUpdate >= _updateSpeed)
    {
        // Boot sequence not done
        _prevUpdate = _now;

        for (int8_t i = 0; i < _NUM_LEDS; i++)
        {
            if (i < _levelTracker || i == _tracker)
            {
                _PwrCellSetColor(i, _battRed, _battGreen, _battBlue);
            }
            else
            {
                _PwrCellSetColor(i, 0, 0, 0);
            }
        }
        _tracker--;
        if (_tracker == _levelTracker)
        {
            _levelTracker++;
            _tracker = _NUM_LEDS;
        }
        return false;
    }

    return false;
}

bool PowerCell::_risingPixelStackDown(uint16_t duration, bool initialize)
{
    if (initialize)
    {
        _tracker = _NUM_LEDS;
        _levelTracker = _NUM_LEDS;
        _rampTime = duration;
        uint16_t totalSteps = 1 + (_NUM_LEDS + 1) * (_NUM_LEDS + 2) / 2;
        _updateSpeed = max(5, duration / totalSteps);

        _getBattLevelColors();
        _setColorAll(_START, _END, _battRed, _battGreen, _battBlue);

        return false;
    }

    if (_levelTracker < 0)
    {
        return true;
    }

    if (_now - _prevUpdate >= _updateSpeed)
    {
        // Boot sequence not done
        _prevUpdate = _now;

        for (int8_t i = 0; i < _NUM_LEDS; i++)
        {
            if (i < _levelTracker || i == _tracker)
            {
                _PwrCellSetColor(i, _battRed, _battGreen, _battBlue);
            }
            else
            {
                _PwrCellSetColor(i, 0, 0, 0);
            }
        }
        _tracker++;
        if (_tracker > _NUM_LEDS)
        {
            _levelTracker--;
            _tracker = _levelTracker;
        }
        return false;
    }

    return false;
}

bool PowerCell::_fillUp(bool initialize)
{
    if (initialize)
    {
        _getBattLevelColors();
        _levelTracker = 0;
        _setColorAll(_START, _END, 0, 0, 0);
        return false;
    }

    if (_levelTracker >= _NUM_LEDS)
    {
        _getBattLevelColors();
        return true;
    }

    // Check if the time for the next update has arrived
    if (_now - _prevUpdate >= _updateSpeed)
    {
        _prevUpdate = _now;

        // Update only the next pixel in line
        if (_levelTracker < _NUM_LEDS)
        {
            // Turn on the next pixel (falling down)
            _PwrCellSetColor(_levelTracker, _battRed, _battGreen, _battBlue);
        }
        else if (_levelTracker == _NUM_LEDS)
        {
            // Reset the last pixel after it reaches the bottom
            _PwrCellSetColor(_NUM_LEDS - 1, 0, 0, 0); // Turn off the pixel
        }

        // Increment or reset the levelTracker
        _levelTracker++;
    }
    return false;
}

void PowerCell::_PwrCellSetColor(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue)
{

    // Adjust the pixel index based on the direction
    uint8_t adjustedPixel = (!_DIRECTION) ? (_START + pixel) : (_END - pixel);

    // If the color is different, set the new color
    _setColor(adjustedPixel, red, green, blue);
}

void PowerCell::_getBattLevelColors()
{ /*
     const uint8_t alwaysBluePercent = 75;
     const uint8_t alwaysRedPercent = 25;

     // If power monitoring is disabled or battery is above the blue threshold
     if (!_POWER_MONITORING || percent >= alwaysBluePercent)
     {
         _PwrCellSetColor(pixel, 0, 0, POWERCELL_BLUE); // Solid Blue
         return;
     }

     // Calculate color shift from blue to red
     int red = _mapFloat(percent, alwaysBluePercent, alwaysRedPercent, 0, 255);
     int blue = _mapFloat(percent, alwaysBluePercent, alwaysRedPercent, POWERCELL_BLUE, 0);

     // Ensure values are within valid color bounds
     red = constrain(red, 0, 255);
     blue = constrain(blue, 0, POWERCELL_BLUE);

     _PwrCellSetColor(pixel, red, 0, blue);*/
    uint8_t offset = 0;
    // If battery is an NiMh 5 cells, offset the minimum to take into account the 85% duty cycle
    // limit of the Arduino nano LTO voltage regulator (5882mV) that is about 20% state of discharge
    if (_battType == 3)
        offset = 15;

    const uint8_t alwaysBluePercent = 15 + offset;
    const uint8_t startYellowPercent = 10 + offset;
    const uint8_t alwaysRedPercent = 5 + offset;

    // Above 75% - Solid Blue
    if (_battLevel >= alwaysBluePercent)
    {
        _battRed = 0;
        _battGreen = 0;
        _battBlue = POWERCELL_BLUE; // Default Blue
        return;
    }

    // Between 75% and 50% - Blue to Yellow (Reduced White)
    if (_battLevel >= startYellowPercent)
    {
        const uint8_t step = alwaysBluePercent - _battLevel;
        const uint8_t yellowRed = POWERCELL_BLUE * 4U / 5U;
        const uint16_t risingRed = (uint16_t)step * 42U;

        _battRed = min(risingRed, (uint16_t)yellowRed);
        _battGreen = step * 7U;
        _battBlue = (step < 4U)
                        ? ((uint16_t)(10U - 3U * step) * POWERCELL_BLUE) / 10U
                        : 0U;
        return;
    }

    // Between 50% and 25% - Yellow to Red (Smooth Transition)
    if (_battLevel >= alwaysRedPercent)
    {
        const uint8_t step = startYellowPercent - _battLevel;
        const uint8_t yellowRed = POWERCELL_BLUE * 4U / 5U;
        const uint8_t yellowGreen = POWERCELL_BLUE / 5U;

        _battRed = yellowRed + step * 7U;
        _battGreen = yellowGreen - step * 7U;
        _battBlue = 0;
        return;
    }

    // Below 25% - Solid Red
    _battRed = 255;
    _battGreen = 0;
    _battBlue = 0;
}
