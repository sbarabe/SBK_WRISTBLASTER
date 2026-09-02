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

#include "SBK_WB_PowerCellBarMeterEngine_V1_1_0.h"

/* DEBUG MESSAGES TO SERIAL */
// comment/uncomment #define DEBUG_TO_SERIAL to receive serial message

#define DEBUG_TO_SERIAL

#ifdef DEBUG_TO_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

PowerCellBarMeter::PowerCellBarMeter(const uint8_t *segNumber, const bool *direction,
                                     BarMeterDriver *driver,
                                     const uint8_t address,
                                     const uint8_t (*mapping)[2])
    : BarMeter(driver,
               address,
               segNumber,
               direction,
               mapping),
      _fillDownInit(false), _fillDownComplete(false),
      _emptyDownInit(false), _emptyDownComplete(false),
      _fillUpInit(false), _fillUpComplete(false),
      _tracker(0), _levelTracker(0),
      _repeat(true),
      _updateInterval(10), _corrSpeed(10),
      _update(true),
      _fadeIn(DISABLE),
      _fadeOut(DISABLE),
      // Power Cell related :
      _bootState(false),
      _pulse(false),
      _flashInterval(5000),
      _battLevel(100),
      _battType(0)
{
}

PowerCellBarMeter::~PowerCellBarMeter()
{
}

void PowerCellBarMeter::begin(uint8_t batt_level, uint8_t battType)
{
    _battLevel = batt_level;
    _battType = battType;
    _tg_speed = PC_POWER_ON_UPDATE_INT/2;

    _driver->setBrightness(0, _brightness); // Set maxBri level (0 is min, 15 is max)
    _driver->clear(0);
    _driver->show(0);
}

void PowerCellBarMeter::clear()
{
    // reset the sequence
    //_reverseSeqTracker = false;
    _tracker = 0;
    _brightness = 15;
    // shut all led's off
    _setLow();
}

void PowerCellBarMeter::update(uint8_t battLevel)
{
    update(millis(), battLevel);
}

void PowerCellBarMeter::update(uint32_t syncCurrentTime, uint8_t battLevel)
{
    _currentTime = syncCurrentTime;
    _battLevel = battLevel;

    // Update only if required
    if (_update)
    {

        // Reset update required tracker
        _update = false;

        // Update brightness id needed
        if (_brightness != _brightnessPrev)
        {
            _brightnessPrev = _brightness;

            _driver->setBrightness(0, _brightness);
        }

        // To be configure for in relation with bar meter total leds number and connections matrix to the MAX72xx
        // Leds mapping might be different for your setup, check rows and columns orders : _driver.setLed(0, ROW, COL, _ledsStates[i]))
        for (uint8_t i = 0; i < *P_SEG_NUMBER; i++)
        {
            // Check if animation is REVERSED
            uint8_t j = *P_DIRECTION ? (*P_SEG_NUMBER - 1 - i) : i; // DIRECTION : false = forward, true = reverse

            // set segments according to mapping define in setting
            _driver->setLed(0, _SEG_MAP[j][0], _SEG_MAP[j][1], _getLedState(i));
        }
        _driver->show(0);
    }
}

void PowerCellBarMeter::bootInit(uint16_t duration)
{

    if (_bootState)
        return;

    _duration = duration;
    _fallingPixelStackUp(_duration, true);
}

bool PowerCellBarMeter::boot()
{
    if (!_bootState) // Pixels drop down PowerCell and pile up!
    {
        if (_fallingPixelStackUp(_duration, false))
        {
            _bootState = true;
            _updateInterval = _tg_speed = _ini_speed = PC_POWER_ON_UPDATE_INT/2;
            rampToIdleInit(PC_POWER_ON_UPDATE_INT, 0);
        }
    }
    else
    {

        rampToIdle();
    }
    return _bootState;
}

void PowerCellBarMeter::shutDownInit(int16_t duration)
{
    _duration = duration;
    _risingPixelStackDown(_duration, true);
}

bool PowerCellBarMeter::shutDown()
{

    if (_risingPixelStackDown(_duration, false))
        _bootState = false;

    return _bootState;
}

// All bar graph pixels are OFF execpt pixels one blinking
void PowerCellBarMeter::poweredDownInit(uint16_t flashInterval)
{
    _flashInterval = flashInterval;

    clear();
    _prevUpdate = _currentTime;
    _pulse = false;
}

// All bar graph pixels are OFF execpt pixels one blinking
void PowerCellBarMeter::poweredDown()
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
    _setLed(0, _pulse);
}

void PowerCellBarMeter::rampToIdleInit(uint16_t tg_speed, uint16_t ramp_time)
{
    _iniTime = _currentTime;
    _duration = ramp_time;

    _tg_speed = constrain(tg_speed/2, 5, 255);
    _ini_speed = _updateInterval;

    _bootState = true;

    rampToIdle();
}

void PowerCellBarMeter::rampToIdle()
{
    _updateInterval = _rampParameter(_iniTime, _duration, _ini_speed, _tg_speed, PC_FIRING_MAX_UPDATE_INT/2);

    // If fill up sequence done, reset sequence
    if (_fillUp(getMaxLevelfromBattLevel(), false))
        _fillUp(getMaxLevelfromBattLevel(), true);
}

void PowerCellBarMeter::lowBattInit(uint16_t blinkInterval)
{
    clear();
    _prevUpdate = _currentTime;
    _pulse = false;
    _flashInterval = blinkInterval;
}

void PowerCellBarMeter::lowBatt()
{
    if (_currentTime - _prevUpdate >= _flashInterval)
    {
        // Boot sequence not done
        _prevUpdate = _currentTime;

        _pulse = !_pulse;
    }

    _setLed(0, _pulse);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*     Private short sequences use in Animations         */  /////////////////////////////////////////////////
/*        (these functions call for an update)            */ /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool PowerCellBarMeter::_fallingPixelStackUp(uint16_t duration, bool initialize)
{

    if (initialize)
    {
        _tracker = *P_SEG_NUMBER;
        _levelTracker = 0;
        _duration = duration;
        uint16_t totalSteps = 1 + (*P_SEG_NUMBER / 2 + 1) * (*P_SEG_NUMBER / 2 + 2) / 2;
        _updateInterval = max(5, duration / totalSteps);
        _setLow();
        _update = true;
        return false;
    }

    if (_levelTracker > *P_SEG_NUMBER + 1)
    {
        return true;
    }

    if (_currentTime - _prevUpdate >= _updateInterval)
    {

        // Boot sequence not done
        _prevUpdate = _currentTime;
        _update = true; // update required

        for (int8_t i = 0; i < *P_SEG_NUMBER; i++)
        {
            if (i < _levelTracker || i == _tracker || i == _tracker + 1)
            {
                _setLed(i, true);
            }
            else
            {
                _setLed(i, false);
            }
        }
        _tracker -= 2;
        if (_tracker + 1 <= _levelTracker)
        {
            _levelTracker += 2;
            _tracker = *P_SEG_NUMBER + 1;
        }
        return false;
    }
}

bool PowerCellBarMeter::_risingPixelStackDown(uint16_t duration, bool initialize)
{
    if (initialize)
    {
        _tracker = *P_SEG_NUMBER;
        _levelTracker = *P_SEG_NUMBER;
        _duration = duration;
        uint16_t totalSteps = 1 + (*P_SEG_NUMBER / 2 + 1) * (*P_SEG_NUMBER / 2 + 2) / 2;
        _updateInterval = max(5, duration / totalSteps);
        return false;
    }

    if (_levelTracker < -1)
    {
        clear();
        _update = true; // update required
        return true;
    }

    if (_currentTime - _prevUpdate >= _updateInterval)
    {
        // Boot sequence not done

        _prevUpdate = _currentTime;
        _update = true; // update required

        for (int8_t i = 0; i < *P_SEG_NUMBER; i++)
        {
            if (i < _levelTracker || i == _tracker || i == _tracker - 1)
            {
                _setLed(i, true);
            }
            else
            {
                _setLed(i, false);
            }
        }
        _tracker += 2;
        if (_tracker - 1 > *P_SEG_NUMBER)
        {
            _levelTracker -= 2;
            _tracker = _levelTracker;
        }
        return false;
    }
}
bool PowerCellBarMeter::_fillUp(uint8_t max_level, bool initialize)
{
    if (initialize)
    {
        _tracker = 0;
        _setLow();
        _update = true; // update required
    }

    uint8_t max_seg = round((max_level * (*P_SEG_NUMBER)) / 100.0);
    if (_tracker > max_seg || _tracker > *P_SEG_NUMBER)
    {
        _tracker = min(max_seg, *P_SEG_NUMBER);
        return true;
    }

    if (_currentTime - _prevUpdate >= _updateInterval)
    {
        _prevUpdate = _currentTime;
        _update = true; // update required

        if (_tracker < *P_SEG_NUMBER && _tracker >= 0) // Bounds check
            _setLed(_tracker, true);

        _tracker++; // Move to the next LED
    }

    return false;
}

uint8_t PowerCellBarMeter::getMaxLevelfromBattLevel()
{
    const uint8_t fullRange = 70;
    const uint8_t minRange = 20;

    uint8_t maxLevel = map(_battLevel, minRange, fullRange, minRange, 100);
    return constrain(maxLevel, minRange, 100);
}
