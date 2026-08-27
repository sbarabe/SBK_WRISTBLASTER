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

#include "SBK_WB_PowerCellLEDsStripEngine_V1_1_0.h"

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

PowerCell::PowerCell(Adafruit_NeoPixel *strip, const uint8_t *numLeds, const uint8_t *start, const uint8_t *end, const bool *direction)
    : LedsStrip(strip),
      P_NUMLEDS(numLeds), P_START(start), P_END(end),
      P_DIRECTION(direction),
      _ini_speed(0), _tg_speed(PC_POWER_ON_UPDATE_INT),
      _tracker(0), _levelTracker(0),
      _bootState(false),
      _pulse(false),
      _flashInterval(5000),
      _lowBatt(false),
      _battRed(0), _battGreen(0), _battBlue(POWERCELL_BLUE),
      _battLevel(100), _battType(0)
{
    _ini_r = new uint8_t[*P_NUMLEDS];
    _ini_g = new uint8_t[*P_NUMLEDS];
    _ini_b = new uint8_t[*P_NUMLEDS];
    _updateSpeed = PC_POWER_ON_UPDATE_INT;
}

PowerCell::~PowerCell()
{
    delete[] _ini_r;
    delete[] _ini_g;
    delete[] _ini_b;
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
    _clearSomePixels(*P_START, *P_END);
}

bool PowerCell::update(uint8_t batt_level) { return update(millis(), batt_level); }

bool PowerCell::update(uint32_t syncCurrentTime, uint8_t batt_level)
{
    _currentTime = syncCurrentTime;

    _battLevel = batt_level;

    if (!_updateRequired)
        return false;

    _strip->show();
    // Reset update tracker
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
            rampToIdleInit(_updateSpeed, 0);
        }
    }
    else
    {
        // boot sequence done, go to regular animation,
        _updateSpeed = PC_POWER_ON_UPDATE_INT;
        rampToIdle();
    }
    return _bootState;
}

void PowerCell::rampToIdleInit(uint16_t tg_speed, uint16_t rampTime)
{
    _iniTime = _currentTime;
    _rampTime = rampTime;

    _tg_speed = constrain(tg_speed, 5, PC_POWER_ON_UPDATE_INT);
    _ini_speed = _updateSpeed;

    _bootState = true;

    rampToIdle();
}

void PowerCell::rampToIdle()
{
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
    _prevUpdate = _currentTime;
    _pulse = false;
}

void PowerCell::poweredDown()
// All bar graph pixels are OFF execpt pixels one blinking
{
    if (_currentTime - _prevUpdate >= _flashInterval)
    {
        _prevUpdate = _currentTime;

        _pulse = false;
    }

    // Turn off LED after short flash
    if (_currentTime - _prevUpdate >= _flashInterval - 50)
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
    _prevUpdate = _currentTime;
    _pulse = false;
    _flashInterval = blinkInterval;
}

void PowerCell::lowBatt()
{
    if (_currentTime - _prevUpdate >= _flashInterval)
    {
        // Boot sequence not done
        _prevUpdate = _currentTime;

        _pulse = !_pulse;
    }

    _PwrCellSetColor(*P_START, _pulse * 125, 0, 0);
}

bool PowerCell::_fallingPixelStackUp(uint16_t duration, bool initialize)
{
    if (initialize)
    {
        _tracker = *P_NUMLEDS;
        _levelTracker = 0;
        _rampTime = duration;
        uint16_t totalSteps = 1 + (*P_NUMLEDS + 1) * (*P_NUMLEDS + 2) / 2;
        _updateSpeed = max(5, duration / totalSteps);

        _setColorAll(*P_START, *P_END, 0, 0, 0);
        _getBattLevelColors();
        return false;
    }

    if (_levelTracker > *P_NUMLEDS)
    {
        return true;
    }

    if (_currentTime - _prevUpdate >= _updateSpeed)
    {
        // Boot sequence not done
        _prevUpdate = _currentTime;

        for (int8_t i = 0; i < *P_NUMLEDS; i++)
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
            _tracker = *P_NUMLEDS;
        }
        return false;
    }
}

bool PowerCell::_risingPixelStackDown(uint16_t duration, bool initialize)
{
    if (initialize)
    {
        _tracker = *P_NUMLEDS;
        _levelTracker = *P_NUMLEDS;
        _rampTime = duration;
        uint16_t totalSteps = 1 + (*P_NUMLEDS + 1) * (*P_NUMLEDS + 2) / 2;
        _updateSpeed = max(5, duration / totalSteps);

        _getBattLevelColors();
        _setColorAll(*P_START, *P_END, _battRed, _battGreen, _battBlue);

        return false;
    }

    if (_levelTracker < 0)
    {
        return true;
    }

    if (_currentTime - _prevUpdate >= _updateSpeed)
    {
        // Boot sequence not done
        _prevUpdate = _currentTime;

        for (int8_t i = 0; i < *P_NUMLEDS; i++)
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
        if (_tracker > *P_NUMLEDS)
        {
            _levelTracker--;
            _tracker = _levelTracker;
        }
        return false;
    }
}

bool PowerCell::_fillUp(bool initialize)
{
    if (initialize)
    {
        _getBattLevelColors();
        _levelTracker = 0;
        _setColorAll(*P_START, *P_END, 0, 0, 0);
        return false;
    }

    if (_levelTracker >= *P_NUMLEDS)
    {
        _getBattLevelColors();
        return true;
    }

    // Check if the time for the next update has arrived
    if (_currentTime - _prevUpdate >= _updateSpeed)
    {
        _prevUpdate = _currentTime;

        // Update only the next pixel in line
        if (_levelTracker < *P_NUMLEDS)
        {
            // Turn on the next pixel (falling down)
            _PwrCellSetColor(_levelTracker, _battRed, _battGreen, _battBlue);
        }
        else if (_levelTracker == *P_NUMLEDS)
        {
            // Reset the last pixel after it reaches the bottom
            _PwrCellSetColor(*P_NUMLEDS - 1, 0, 0, 0); // Turn off the pixel
        }

        // Increment or reset the levelTracker
        _levelTracker++;
    }
    return false;
}

void PowerCell::_PwrCellSetColor(uint16_t pixel, uint8_t red, uint8_t green, uint8_t blue)
{

    // Adjust the pixel index based on the direction
    uint8_t adjustedPixel = (!*P_DIRECTION) ? (*P_START + pixel) : (*P_END - pixel);

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
        float mix = _mapFloat(_battLevel, alwaysBluePercent, startYellowPercent, 0.0f, 1.0f);

        _battRed = (uint8_t)(min(1.5f * mix * POWERCELL_BLUE / 1.25f, POWERCELL_BLUE / 1.25f)); // Red increases to max 120
        _battGreen = (uint8_t)(mix * POWERCELL_BLUE / 5.0f);                                    // Green to max 220 for yellow
        _battBlue = (uint8_t)(max(0, (1.0f - 1.5f * mix)) * POWERCELL_BLUE);                    // Blue fades out, faster then other color
        return;
    }

    // Between 50% and 25% - Yellow to Red (Smooth Transition)
    if (_battLevel >= alwaysRedPercent)
    {
        float mix = _mapFloat(_battLevel, startYellowPercent, alwaysRedPercent, 0.0f, 1.0f);

        _battRed = (uint8_t)(_mapFloat(mix, 0.0f, 1.0f, POWERCELL_BLUE / 1.25f, POWERCELL_BLUE)); // Red smoothly rises to 255
        _battGreen = (uint8_t)(_mapFloat(mix, 0.0f, 1.0f, POWERCELL_BLUE / 5.0f, 0));             // Green smoothly falls to 0
        _battBlue = 0;
        return;
    }

    // Below 25% - Solid Red
    _battRed = 255;
    _battGreen = 0;
    _battBlue = 0;
}

float PowerCell::_mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
