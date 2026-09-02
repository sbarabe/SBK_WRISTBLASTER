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


#include "SBK_WB_PanelBarMeterEngine_V1_1_0.h"

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

PanelBarMeter::PanelBarMeter(const uint8_t *segNumber,
                             const bool *direction,
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
      _tracker(0),
      _prevHeatLevel(0),
      _repeat(true),
      _lastBeatTime(0),
      _lastRandomUpdate(0), _isPeak(false),
      _MIN_BASE_LEVEL(10), _MIN_PEAK_LEVEL(18), _randomOffset(0), _currentLevel(_MIN_BASE_LEVEL),
      _peakLevel(5), _prevPeakUpdate(0), _PEAK_HOLD_TIME(150),
      _fadeIn(DISABLE),
      _fadeOut(DISABLE)
{
}

PanelBarMeter::~PanelBarMeter()
{
}

void PanelBarMeter::begin()
{
    clear();
}

void PanelBarMeter::clear()
{
    _tracker = 0;
    _brightness = 15;
    _setLow();
}

void PanelBarMeter::fillDownEmptyDownOnceInit(uint16_t duration, bool fadeIn) // fill from top to bottom and empty down to bottom
{
    _updateInterval = max(5, duration) / (*P_SEG_NUMBER * 2);
    _updateInterval = constrain(_updateInterval, 10, 255);

    _fadeIn = fadeIn;
    _brightness = (_fadeIn == ENABLE) ? 0 : 15; // For the Fade In effect

    _fillDownInit = true;
    _fillDownComplete = false;
    _emptyDownInit = false;
    _emptyDownComplete = false;
}

void PanelBarMeter::fillDownEmptyDownOnce() // fill from top to bottom and empty down to bottom
{
    if (_emptyDownComplete)
        return;

    if (_fillDownInit)                                             // Initialise sequence
        _fillDownInit = _fillDown(_updateInterval, true, _fadeIn); // Initialize fill up

    else if (!_fillDownComplete)
    {
        _fillDownComplete = _fillDown(_updateInterval, false, _fadeIn); // Fill the bar meter upward
        _emptyDownInit = _fillDownComplete;
    }

    else if (_emptyDownInit)
        _emptyDownInit = _emptyDown(_updateInterval, true, _fadeIn); // Initialize fill down

    else
        _emptyDownComplete = _emptyDown(_updateInterval, false, _fadeIn); // Empty the bar meter downward
}

void PanelBarMeter::cyclotronIdleInit(uint8_t heatLevel)
{
    uint8_t scaledHeatLevel = constrain(map(heatLevel, 0, 100, 0, *P_SEG_NUMBER), 0, *P_SEG_NUMBER);

    _updateInterval = 25;

    _direction = false;
    _tracker = scaledHeatLevel;
    _brightness = 15;
    _setLow();
}

void PanelBarMeter::cyclotronIdle(uint8_t heatLevel)
{
    // Convert 0-100 scale to 0-*P_SEG_NUMBER (28)
    uint8_t scaledHeatLevel = constrain(map(heatLevel, 0, 100, 0, *P_SEG_NUMBER), 0, *P_SEG_NUMBER);

    if (_currentTime - _prevUpdate >= _updateInterval)
    {
        _prevUpdate = _currentTime;
        _update = true; // update required

        for (uint8_t i = 0; i < *P_SEG_NUMBER; i++)
        {
            // Fill from bottom and top toward center up to tracker
            if ((i < _tracker) || (i >= *P_SEG_NUMBER - _tracker))
            {
                _setLed(i, true);
            }
            else
            {
                _setLed(i, false);
            }
        }

        // Increase tracker and reverse direction if center is reached
        if (!_direction)
        {
            _tracker = min(_tracker + 1, 14);
            if (_tracker >= 14)
            {
                _direction = true;
            }
        }

        // Decrease tracker and reverse direction if heat Level is reached
        else
        {
            _tracker = max(_tracker - 1, scaledHeatLevel / 2);
            if (_tracker <= scaledHeatLevel / 2)
            {
                _direction = false;
            }
        }
    }
}

void PanelBarMeter::cyclotronIdleFullInit(uint8_t heatLevel)
{
    // // Convert 0-100 scale to 0-*P_SEG_NUMBER (28)
    // uint8_t scaledHeatLevel = constrain(map(heatLevel, 0, 100, 0, *P_SEG_NUMBER - 5), 0, *P_SEG_NUMBER - 5);

    _updateInterval = 10;
    _prevHeatLevel = 0;

    _direction = false; // Set initial direction to "up"
    _tracker = 0;
    _brightness = 15;
    _setLow(); // Fill the bar meter upwards initially
}

void PanelBarMeter::cyclotronIdleFull(uint8_t heatLevel)
{
    if (_currentTime - _prevUpdate >= _updateInterval)
    {
        // Convert 0-100 scale to 0-23, not 28, to leave a buffer for bouncing even at max heat level
        uint8_t scaledHeatLevel = constrain(map(heatLevel, 0, 100, 0, *P_SEG_NUMBER - 5), 0, *P_SEG_NUMBER - 5);

        // DEBUG_PRINTLN("Barmeter Idle Full : scaled HL = " + String(scaledHeatLevel) + "  tracker = " + String(_tracker));

        // Fill the bottom relative to to heat level
        if (_prevHeatLevel != heatLevel)
        {
            _prevHeatLevel = heatLevel;
            _update = true; // update required
            // for (uint8_t i = 0; i < scaledHeatLevel; i++)
            // {
            //     _setLed(i, true);
            // }
        }

        // Bounce from heat level to top
        if (!_direction)
        {
            if (_tracker >= *P_SEG_NUMBER)
            {
                _tracker = *P_SEG_NUMBER - 1;
                _direction = true;
            }
            else
            {
                _prevUpdate = _currentTime;
                _update = true; // update required

                // if (_tracker < *P_SEG_NUMBER && _tracker >= 0) // Bounds check
                _setLed(_tracker, true);

                _tracker++; // Move to the next LED
            }
        }
        else
        {
            if (_tracker <= scaledHeatLevel)
            {
                _tracker = scaledHeatLevel;
                _direction = false;
            }
            else
            {
                _prevUpdate = _currentTime;
                _update = true; // update required

                // if (_tracker < *P_SEG_NUMBER && _tracker >= 0) // Bounds check
                // {
                _setLed(_tracker, false);
                // }

                _tracker--; // Move to the next LED
            }
        }
    }
}

void PanelBarMeter::fillUpEmptyDownOnceInit(uint16_t duration) // fill up from bottom to top and empty bottom to top
{
    _updateInterval = max(5, duration) / (*P_SEG_NUMBER * 2);
    _updateInterval = constrain(_updateInterval, 10, 255);

    _fillUpInit = true; // Set initial direction to "up"
    _fillUpComplete = false;
    _emptyDownInit = true; // Reset completed flag
    _emptyDownComplete = false;
    _brightness = 15;
}

void PanelBarMeter::fillUpEmptyDownOnce() // fill up from bottom to top and empty bottom to top
{
    if (_emptyDownComplete)
        return;

    if (_fillUpInit)
        _fillUpInit = _fillUp(_updateInterval, true); // Initialise fill up

    // Depending on the direction, either fill up or empty down
    else if (!_fillUpComplete)
        _fillUpComplete = _fillUp(_updateInterval, false); // Fill up the bar meter

    else if (_emptyDownInit)
        _emptyDownInit = _emptyDown(_updateInterval, true, DISABLE); // Empty down the bar meter

    else
        _emptyDownComplete = _emptyDown(_updateInterval, false, DISABLE); // Empty down the bar meter
}

void PanelBarMeter::fillUpFastEmptyDownSlowOnceInit(uint16_t duration, bool fadeout) // full bar fast and slow emptying from top to bottom
{
    _fadeOut = fadeout;
    _updateInterval = round(max(5, duration - 10.0 * *P_SEG_NUMBER) / (*P_SEG_NUMBER * 1.1));
    _updateInterval = constrain(_updateInterval, 10, 255);

    _fillUpInit = true; // Set initial direction to "up"
    _fillUpComplete = false;
    _emptyDownInit = true; // Reset completed flag
    _emptyDownComplete = false;
    _brightness = 15;
}

void PanelBarMeter::fillUpFastEmptyDownSlowOnce() // full bar fast and slow emptying from top to bottom
{
    if (_emptyDownComplete)
        return;

    if (_fillUpInit)
        _fillUpInit = _fillUp(10, true);

    else if (!_fillUpComplete)
        _fillUpComplete = _fillUp(10, false); // Fill up the bar meter

    else if (_emptyDownInit)
        _emptyDownInit = _emptyDown(_updateInterval, true, _fadeOut);

    else
        _emptyDownComplete = _emptyDown(_updateInterval, false, _fadeOut);
}

void PanelBarMeter::partyModeInit() // bouncing from bottom (maybe like a volume meter with the music)
{
    _lastBeatTime = _currentTime;
    _lastRandomUpdate = _currentTime;
    _isPeak = false;
    _brightness = 15;
    _setLow();
}

void PanelBarMeter::partyMode() // bouncing from bottom (maybe like a volume meter with the music)
{
    // 116 BPM means roughly every 517ms there should be a peak (Ghostbusters song bpm)
    if (_currentTime - _lastBeatTime >= 517)
    {
        _lastBeatTime = _currentTime;
        _update = true;     // update required
        _isPeak = !_isPeak; // Toggle between peak and base level
    }

    // Smooth transition between base and peak levels
    if (_isPeak && _currentLevel <= _MIN_PEAK_LEVEL)
    {
        _currentLevel += random(3, 5); // Gradual increase
    }
    else if (!_isPeak && _currentLevel >= _MIN_BASE_LEVEL)
    {
        _currentLevel -= random(0, 4); // Gradual decrease
    }

    // Randomized additional LEDs on top at a random interval
    if (_currentTime - _lastRandomUpdate >= uint16_t(random(50, 300)))
    {
        _lastRandomUpdate = _currentTime;
        _update = true;                // update required
        _randomOffset = random(-4, 4); // 0 to 5 extra LEDs
    }

    uint8_t finalLevel = _currentLevel + _randomOffset;
    if (finalLevel > *P_SEG_NUMBER)
        finalLevel = *P_SEG_NUMBER;
    if (finalLevel < 0)
        finalLevel = 0;

    // Update peak level
    if (finalLevel > _peakLevel)
    {
        _peakLevel = finalLevel;        // New peak
        _prevPeakUpdate = _currentTime; // Reset decay timer
    }
    else if (_currentTime - _prevPeakUpdate >= _PEAK_HOLD_TIME && _peakLevel > finalLevel)
    {
        _peakLevel--;                   // Slowly drop peak
        _prevPeakUpdate = _currentTime; // Reset decay timer
        _update = true;                 // update required
    }

    // Update LED states
    for (uint8_t i = 0; i < *P_SEG_NUMBER; i++)
    {
        _setLed(i, (i < finalLevel));
    }

    // Ensure peak LED stays on
    if (_peakLevel < *P_SEG_NUMBER)
    {
        _setLed(_peakLevel, true);
    }
}

bool fire1[23][28] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

void PanelBarMeter::fireInit(bool direction)
{
    fireInit(direction, REPEAT_SEQ);
}

void PanelBarMeter::fireInit(bool direction, bool repeat)
{
    _direction = direction;
    _repeat = repeat;

    // If the previous tate sequence must just be finished in this state
    if (_repeat == END_SEQ)
        return;

    // Initialize if it's not just the ending sequence
    _updateInterval = 25;
    _tracker = _direction ? 0 : 22; // Capture starts from the last frame, Burst starts from the first frame
    _corrSpeed = _updateInterval;
    _brightness = 15;
    _setLow();
}

void PanelBarMeter::fire(uint8_t heatLevel)
{
    // Get the corrected speed from heat level
    _corrSpeed = map(heatLevel, 0, 100, _updateInterval, 10);

    // Check if enough time has passed to update the animation
    if (_currentTime - _prevUpdate >= _corrSpeed)
    {
        // DEBUG_PRINTLN("_direction:" + String(_direction) + " _repeat:" + String(_repeat) + " _tracker:" + String(_tracker));

        _prevUpdate = _currentTime;
        _update = true; // update required

        // Update LED states based on the current frame in fire1
        for (int8_t i = 0; i < *P_SEG_NUMBER; i++)
        {
            _setLed(i, fire1[_tracker][i]);
        }

        // Move to the previous frame
        if (_direction == CAPTURE)
        {
            // Move to the next frame in the selected direction
            _tracker--;
            // Repeat or end sequence
            if (_tracker < 0)
                _tracker = (_repeat == REPEAT_SEQ) ? 22 : 0;
        }
        else
        {
            // Move to the next frame in the selected direction
            _tracker++;
            // Repeat or end sequence
            if (_tracker > 22)
                _tracker = (_repeat == REPEAT_SEQ) ? 0 : 22;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*     Private short sequences use in Animations         */ /////////////////////////////////////////////////
/*        (this functions call for an update)            */ /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool PanelBarMeter::_fillUp(uint8_t speed, bool initialize)
{
    if (initialize)
    {
        _tracker = 0;
        _setLow();
        _update = true; // update required
    }

    if (_tracker > *P_SEG_NUMBER)
    {
        _tracker = *P_SEG_NUMBER;
        return true;
    }

    if (_currentTime - _prevUpdate >= speed)
    {
        _prevUpdate = _currentTime;
        _update = true; // update required

        if (_tracker < *P_SEG_NUMBER && _tracker >= 0) // Bounds check
            _setLed(_tracker, true);

        _tracker++; // Move to the next LED
    }

    return false;
}

bool PanelBarMeter::_emptyDown(uint8_t speed, bool initialize, bool fadeout)
{
    static bool intensityFlag = false;

    if (initialize)
    {
        _tracker = *P_SEG_NUMBER;
        _setHigh();
        _update = true; // update required
    }

    if (_tracker < 0)
    {
        _tracker = 0;
        return true;
    }

    if (_currentTime - _prevUpdate >= speed)
    {
        _prevUpdate = _currentTime;
        _update = true; // update required

        if (_tracker < *P_SEG_NUMBER && _tracker >= 0) // Bounds check
        {
            _setLed(_tracker, false);
        }

        _tracker--; // Move to the next LED

        if (fadeout)
        {
            if (intensityFlag)
            {
                _brightness = (max(0, _brightness - 1));
            }

            intensityFlag = !intensityFlag; // Decrease intensity only one out of two update
        }
    }

    return false;
}

bool PanelBarMeter::_fillDown(uint8_t speed, bool initialize, bool fadein)
{
    static bool intensityFlag = false;

    if (initialize)
    {
        _tracker = *P_SEG_NUMBER;
        _setLow();
        _update = true; // update required
    }

    if (_tracker < 0)
    {
        _tracker = 0;
        return true;
    }

    if (_currentTime - _prevUpdate >= speed)
    {
        _prevUpdate = _currentTime;
        _update = true; // update required

        if (_tracker < *P_SEG_NUMBER && _tracker >= 0) // Bounds check
        {
            _setLed(_tracker, true);
        }

        _tracker--; // Move to the next LED

        if (fadein)
        {
            if (intensityFlag)
            {
                _brightness = (min(_brightness + 1, 15));
            }

            intensityFlag = !intensityFlag; // Increase intensity only one out of two update
        }
    }

    return false;
}

bool PanelBarMeter::_emptyUp(uint8_t speed, bool initialize)
{
    if (initialize)
    {
        _tracker = 0;
        _setHigh();
        _update = true; // update required
    }

    if (_tracker > *P_SEG_NUMBER)
    {
        _tracker = *P_SEG_NUMBER;
        return true;
    }

    if (_currentTime - _prevUpdate >= speed)
    {
        _prevUpdate = _currentTime;
        _update = true; // update required

        if (_tracker < *P_SEG_NUMBER && _tracker >= 0) // Bounds check
        {
            _setLed(_tracker, false);
        }

        _tracker++; // Move to the next LED
    }

    return false;
}
