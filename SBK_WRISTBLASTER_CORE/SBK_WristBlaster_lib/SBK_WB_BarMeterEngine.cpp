/*
 *  SBK_WB_BarMeterEngine.cpp is a part of SBK_WRISTBLASTER_CORE (VERSION 0) code for bar meter animations of a Wrist Blaster replica
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

#include "SBK_WB_BarMeterEngine.h"

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



BarMeterAnimation::BarMeterAnimation(const uint8_t *segNumber)
    : P_SEG_NUMBER(segNumber),
      _currentTime(0),
      _ledsStatesLow(0),
      _ledsStatesHigh(0),
      _brightness(15),
      _brightnessPrev(25),
      _direction(false),
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
      _speed(10), _corrSpeed(10),
      _update(true),
      _fadeIn(DISABLE),
      _fadeOut(DISABLE)
{
}

uint8_t BarMeterAnimation::getLedState(uint8_t index)
{
    // If out of bound return 0
    if (index >= *P_SEG_NUMBER || index < 0)
    {
        return 0;
    }
    // For the first 16 bits
    if (index < 16)
    {
        return (_ledsStatesLow & (1 << index)) != 0;
    }
    // For the last 16 bits
    else
    {
        return (_ledsStatesHigh & (1 << (index - 16))) != 0;
    }
}

void BarMeterAnimation::clear()
{
    // reset the sequence
    //_reverseSeqTracker = false;
    _tracker = 0;
    _brightness = 15;
    // shut all led's off
    _setLow();
}

void BarMeterAnimation::fillDownEmptyDownOnceInit(uint16_t duration, bool fadeIn) // fill from top to bottom and empty down to bottom
{
    _speed = max(5, duration) / (*P_SEG_NUMBER * 2);
    _speed = constrain(_speed, 10, 255);

    _fadeIn = fadeIn;
    _brightness = (_fadeIn == ENABLE) ? 0 : 15; // For the Fade In effect

    _fillDownInit = true;
    _fillDownComplete = false;
    _emptyDownInit = false;
    _emptyDownComplete = false;
}

void BarMeterAnimation::fillDownEmptyDownOnce() // fill from top to bottom and empty down to bottom
{
    if (_emptyDownComplete)
        return;

    if (_fillDownInit)                                    // Initialise sequence
        _fillDownInit = _fillDown(_speed, true, _fadeIn); // Initialize fill up

    else if (!_fillDownComplete)
    {
        _fillDownComplete = _fillDown(_speed, false, _fadeIn); // Fill the bar meter upward
        _emptyDownInit = _fillDownComplete;
    }

    else if (_emptyDownInit)
        _emptyDownInit = _emptyDown(_speed, true, _fadeIn); // Initialize fill down

    else
        _emptyDownComplete = _emptyDown(_speed, false, _fadeIn); // Empty the bar meter downward
}

void BarMeterAnimation::cyclotronIdleInit(uint8_t heatLevel)
{
    uint8_t scaledHeatLevel = constrain(map(heatLevel, 0, 100, 0, *P_SEG_NUMBER), 0, *P_SEG_NUMBER);

    _speed = 25;

    _direction = false;
    _tracker = scaledHeatLevel;
    _brightness = 15;
    _setLow();
}

void BarMeterAnimation::cyclotronIdle(uint8_t heatLevel)
{
    // Convert 0-100 scale to 0-*P_SEG_NUMBER (28)
    uint8_t scaledHeatLevel = constrain(map(heatLevel, 0, 100, 0, *P_SEG_NUMBER), 0, *P_SEG_NUMBER);

    if (_currentTime - _prevUpdate >= _speed)
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

void BarMeterAnimation::cyclotronIdleFullInit(uint8_t heatLevel)
{
    // // Convert 0-100 scale to 0-*P_SEG_NUMBER (28)
    // uint8_t scaledHeatLevel = constrain(map(heatLevel, 0, 100, 0, *P_SEG_NUMBER - 5), 0, *P_SEG_NUMBER - 5);

    _speed = 10;
    _prevHeatLevel = 0;

    _direction = false; // Set initial direction to "up"
    _tracker = 0;
    _brightness = 15;
    _setLow(); // Fill the bar meter upwards initially
}

void BarMeterAnimation::cyclotronIdleFull(uint8_t heatLevel)
{
    if (_currentTime - _prevUpdate >= _speed)
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

void BarMeterAnimation::fillUpEmptyDownOnceInit(uint16_t duration) // fill up from bottom to top and empty bottom to top
{
    _speed = max(5, duration) / (*P_SEG_NUMBER * 2);
    _speed = constrain(_speed, 10, 255);

    _fillUpInit = true; // Set initial direction to "up"
    _fillUpComplete = false;
    _emptyDownInit = true; // Reset completed flag
    _emptyDownComplete = false;
    _brightness = 15;
}

void BarMeterAnimation::fillUpEmptyDownOnce() // fill up from bottom to top and empty bottom to top
{
    if (_emptyDownComplete)
        return;

    if (_fillUpInit)
        _fillUpInit = _fillUp(_speed, true); // Initialise fill up

    // Depending on the direction, either fill up or empty down
    else if (!_fillUpComplete)
        _fillUpComplete = _fillUp(_speed, false); // Fill up the bar meter

    else if (_emptyDownInit)
        _emptyDownInit = _emptyDown(_speed, true, DISABLE); // Empty down the bar meter

    else
        _emptyDownComplete = _emptyDown(_speed, false, DISABLE); // Empty down the bar meter
}

void BarMeterAnimation::fillUpFastEmptyDownSlowOnceInit(uint16_t duration, bool fadeout) // full bar fast and slow emptying from top to bottom
{
    _fadeOut = fadeout;
    _speed = round(max(5, duration - 10.0 * *P_SEG_NUMBER) / (*P_SEG_NUMBER * 1.1));
    _speed = constrain(_speed, 10, 255);

    _fillUpInit = true; // Set initial direction to "up"
    _fillUpComplete = false;
    _emptyDownInit = true; // Reset completed flag
    _emptyDownComplete = false;
    _brightness = 15;
}

void BarMeterAnimation::fillUpFastEmptyDownSlowOnce() // full bar fast and slow emptying from top to bottom
{
    if (_emptyDownComplete)
        return;

    if (_fillUpInit)
        _fillUpInit = _fillUp(10, true);

    else if (!_fillUpComplete)
        _fillUpComplete = _fillUp(10, false); // Fill up the bar meter

    else if (_emptyDownInit)
        _emptyDownInit = _emptyDown(_speed, true, _fadeOut);

    else
        _emptyDownComplete = _emptyDown(_speed, false, _fadeOut);
}

void BarMeterAnimation::partyModeInit() // bouncing from bottom (maybe like a volume meter with the music)
{
    _lastBeatTime = _currentTime;
    _lastRandomUpdate = _currentTime;
    _isPeak = false;
    _brightness = 15;
    _setLow();
}

void BarMeterAnimation::partyMode() // bouncing from bottom (maybe like a volume meter with the music)
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
        _peakLevel = finalLevel;           // New peak
        _prevPeakUpdate = _currentTime; // Reset decay timer
    }
    else if (_currentTime - _prevPeakUpdate >= _PEAK_HOLD_TIME && _peakLevel > finalLevel)
    {
        _peakLevel--;                      // Slowly drop peak
        _prevPeakUpdate = _currentTime; // Reset decay timer
        _update = true;                    // update required
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

void BarMeterAnimation::fireInit(bool direction)
{
    fireInit(direction, REPEAT_SEQ);
}

void BarMeterAnimation::fireInit(bool direction, bool repeat)
{
    _direction = direction;
    _repeat = repeat;

    // If the previous tate sequence must just be finished in this state
    if (_repeat == END_SEQ)
        return;

    // Initialize if it's not just the ending sequence
    _speed = 25;
    _tracker = _direction ? 0 : 22; // Capture starts from the last frame, Burst starts from the first frame
    _corrSpeed = _speed;
    _brightness = 15;
    _setLow();
}

void BarMeterAnimation::fire(uint8_t heatLevel)
{
    // Get the corrected speed from heat level
    _corrSpeed = map(heatLevel, 0, 100, _speed, 10);

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

/*void BarMeterAnimation::bouncingFromBottom(bool initialize)
{ // In Idle mode safety ON
    // normal sync animation on the bar meter while safety ON
    static bool direction = false;
    uint8_t speed = 25;

    if (initialize)
    {
        _brightness = 15;
        _fillUp(speed, false); // Fill the bar meter upwards initially
        direction = false;     // Set initial direction to "up"
    }

    if (!direction)
    {
        direction = _fillUp(speed, true); // If direction is true, empty down and toggle the direction
    }
    else
    {
        direction = !_emptyDown(speed, true); // If direction is false, fill up and toggle the direction
    }
}*/

/*void BarMeterAnimation::bouncingFromCenter(bool initialize, uint8_t level)
{
    static int8_t tracker;
    static bool direction;
    uint8_t speed = 25;

    if (initialize)
    {
        tracker = level;
        direction = false;
        _brightness = 15;
        _setLow();
        _update = true; // update required
    }

    if (_currentTime - _prevUpdate >= speed)
    {
        _prevUpdate = _currentTime;
        _update = true; // update required

        for (uint8_t i = 0; i < *P_SEG_NUMBER; i++)
        {
            if ((i < tracker) || (i >= *P_SEG_NUMBER - tracker))
            {
                _setLed(i, false);
            }
            else
            {
                _setLed(i, true);
            }
        }

        if (!direction)
        {
            tracker = min(tracker + 1, 14);
            if (tracker >= 14)
            {
                direction = true;
            }
        }
        else
        {
            tracker = max(tracker - 1, level);
            if (tracker <= level)
            {
                direction = false;
            }
        }
    }
}*/

/*void BarMeterAnimation::shootingFromCenter(bool initialize, uint8_t level)
{
    static int8_t tracker;
    uint8_t speed = 25;

    if (initialize)
    {
        tracker = level;
        _brightness = 15;
        _setLow();
    }

    if (_currentTime - _prevUpdate >= speed)
    {
        _prevUpdate = _currentTime;
        _update = true; // update required

        for (uint8_t i = 0; i < *P_SEG_NUMBER; i++)
        {
            if (i > 14 - level && i < 13 + level)
            {
                _setLed(i, true);
            }
            else
            {
                _setLed(i, false);
            }
        }

        _setLed(tracker, true);
        _setLed(*P_SEG_NUMBER - 1 - tracker, true);
        tracker--;
        if (tracker < 0)
        {
            tracker = 14 - level;
        }
    }
}*/

/*void BarMeterAnimation::idleTwo(bool initialize)
{
    uint8_t speed = 25;

    _brightness = 15;

    // normal sync animation on the bar meter while safety OFF
    if (_currentTime - _prevUpdate >= speed)
    {
        _prevUpdate = _currentTime;
        _update = true; // update required

        for (int8_t i = 0; i < *P_SEG_NUMBER; i++)
        {
            // Only segement equal to running led tracker will be ON
            if (i == _tracker - 1)
            {
                _setLed(i, true);
            }
            else
            {
                _setLed(i, false);
            }
        }
        if (_reverseSeqTracker == false)
        {
            _tracker++;
            if (_tracker >= *P_SEG_NUMBER)
            {
                _tracker = *P_SEG_NUMBER;
                _reverseSeqTracker = true;
            }
        }
        else
        {
            _tracker--;
            if (_tracker <= 1)
            {
                _tracker = 1;
                _reverseSeqTracker = false;
            }
        }
    }
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*     Private short sequences use in Animations         */ /////////////////////////////////////////////////
/*        (this functions call for an update)            */ /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BarMeterAnimation::_fillUp(uint8_t speed, bool initialize)
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

bool BarMeterAnimation::_emptyDown(uint8_t speed, bool initialize, bool fadeout)
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

bool BarMeterAnimation::_fillDown(uint8_t speed, bool initialize, bool fadein)
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

bool BarMeterAnimation::_emptyUp(uint8_t speed, bool initialize)
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

void BarMeterAnimation::_setHigh()
{
    _update = true; // update required

    // Set all 32 bits to 1, turning all LEDs on
    _ledsStatesLow = 0xFFFF;
    _ledsStatesHigh = 0xFFFF;
}

void BarMeterAnimation::_setLow()
{
    _update = true; // update required

    // Set all 32 bits to 0, turning all LEDs off
    _ledsStatesLow = 0;
    _ledsStatesHigh = 0;
}

void BarMeterAnimation::_setLed(uint8_t index, bool state)
{
    if (index < 16 && index >= 0)
    {
        if (state)
        {
            _ledsStatesLow |= (1 << index); // Set the LED in the lower part (0-15)
        }
        else
        {
            _ledsStatesLow &= ~(1 << index); // Clear the LED in the lower part (0-15)
        }
    }
    else if (index < 28)
    {
        uint8_t shiftedIndex = index - 16;
        if (state)
        {
            _ledsStatesHigh |= (1 << shiftedIndex); // Set the LED in the upper part (16-27)
        }
        else
        {
            _ledsStatesHigh &= ~(1 << shiftedIndex); // Clear the LED in the upper part (16-27)
        }
    }
}

/*************************************************************************************************************/
/*                         HT16K33 Driver class definitions and functions                                    */
/*************************************************************************************************************/

HT16K33Driver::HT16K33Driver(const uint8_t *segNumber,
                             const bool *direction,
                             const uint8_t dataPin, const uint8_t clockPin,
                             const uint8_t address,
                             const uint8_t (*mapping)[2])
    : BarMeterAnimation(segNumber),
      P_DIRECTION(direction),
      _CLOCK_PIN(clockPin), _DATA_PIN(dataPin), _ADDRESS(address),
      BM_SEG_MAP(mapping)
{
}

void HT16K33Driver::begin()
{
    _driver.init(_ADDRESS);
    _driver.setBrightness(_brightness); // Set maxBri level (0 is min, 15 is max)
    _driver.clear();
    clear();
    DEBUG_PRINTLN("HT16K33 bar meter driver DEBUG ON");
}

void HT16K33Driver::update()
{
    // Update only if required
    if (_update)
    {
        // Reset update required tracker
        _update = false;

        // Update brightness id needed
        if (_brightness != _brightnessPrev)
        {
            _brightnessPrev = _brightness;
            _driver.setBrightness(_brightness); // Set maxBri level (0 is min, 15 is max)
        }
        // To be configure for in relation with bar meter total leds number and connections matrix to the MAX72xx
        // Leds mapping might be different for your setup, check rows and columns orders : _driver.setPixel(0, ROW, COL, _ledsStates[i]))
        for (uint8_t i = 0; i < *P_SEG_NUMBER; i++)
        {
            // Check if animation is REVERSED
            uint8_t j = *P_DIRECTION ? (*P_SEG_NUMBER - 1 - i) : i; // DIRECTION : false = forward, true = reverse

            // set segments according to mapping define in setting
            _driver.setPixel(BM_SEG_MAP[j][0], BM_SEG_MAP[j][1], getLedState(i));
        }
        _driver.write();
    }
}

/*************************************************************************************************************/
/*                         MAX72xx Driver class definitions and functions                                    */
/*************************************************************************************************************/

MAX72xxDriver::MAX72xxDriver(const uint8_t *segNumber,
                             const bool *direction,
                             const uint8_t dataPin, const uint8_t clockPin, const uint8_t loadPin,
                             const uint8_t (*mapping)[2])
    : BarMeterAnimation(segNumber),
      P_DIRECTION(direction),
      _DATA_PIN(dataPin), _CLOCK_PIN(clockPin), _LOAD_PIN(loadPin),
      _driver(MAX72xx(_DATA_PIN, _CLOCK_PIN, _LOAD_PIN, 1)),
      BM_SEG_MAP(mapping)
{
}

void MAX72xxDriver::begin()
{
    pinMode(_LOAD_PIN, OUTPUT);
    pinMode(_CLOCK_PIN, OUTPUT);
    pinMode(_DATA_PIN, OUTPUT);
    _driver.setScanLimit(0, 6);
    _driver.shutdown(0, false);
    _driver.setIntensity(0, _brightness); // Set maxBri level (0 is min, 15 is max)
    _driver.clearDisplay(0);
    clear();
    DEBUG_PRINTLN("MAX72xx bar meter driver DEBUG ON");
}

void MAX72xxDriver::update()
{
    update(millis());
}

void MAX72xxDriver::update(uint32_t syncCurrentTime)
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
            _driver.setIntensity(0, _brightness); // Set maxBri level (0 is min, 15 is max)
        }

        // To be configure for in relation with bar meter total leds number and connections matrix to the MAX72xx
        // Leds mapping might be different for your setup, check rows and columns orders : _driver.setLed(0, ROW, COL, _ledsStates[i]))
        for (uint8_t i = 0; i < *P_SEG_NUMBER; i++)
        {
            // Check if animation is REVERSED
            uint8_t j = *P_DIRECTION ? (*P_SEG_NUMBER - 1 - i) : i; // DIRECTION : false = forward, true = reverse

            // set segments according to mapping define in setting
            _driver.setLed(0, BM_SEG_MAP[j][0], BM_SEG_MAP[j][1], getLedState(i));
        }
    }
}
