/*
 *  SBK_WB_BarMeterEngine.h is a part of SBK_WRISTBLASTER_CORE (VERSION 0) code for bar meter animations of a Wrist Blaster replica
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

#ifndef SBK_WB_BARMETERENGINE_H
#define SBK_WB_BARMETERENGINE_H

#include <Arduino.h>
#include "SBK_WB_MAX72xx.h"
#include <SPI.h>
#include "SBK_WB_HT16K33.h"

/* GENERAL HELPERS */
#ifndef DISABLE
#define DISABLE 0
#endif
#ifndef ENABLE
#define ENABLE 1
#endif
// Sequence looping
#ifndef END_SEQ
#define END_SEQ 0
#endif
#ifndef REPEAT_SEQ
#define REPEAT_SEQ 1
#endif
// Fire type
#ifndef CAPTURE
#define CAPTURE 0
#endif
#ifndef BURST
#define BURST 1
#endif
// animation directions
#ifndef FORWARD
#define FORWARD 0
#endif
#ifndef REVERSE
#define REVERSE 1
#endif

class BarMeterAnimation
{
public:
    BarMeterAnimation(const uint8_t *segNumber);
    uint8_t getLedState(uint8_t index);
    void clear();
    void partyModeInit();
    void partyMode();
    void fillUpEmptyDownOnceInit(uint16_t duration);
    void fillUpEmptyDownOnce();
    void fillUpFastEmptyDownSlowOnceInit(uint16_t duration, bool fadeout);
    void fillUpFastEmptyDownSlowOnce();
    void fillDownEmptyDownOnceInit(uint16_t duration, bool fadeIn);
    void fillDownEmptyDownOnce();
    void cyclotronIdleInit(uint8_t heatLevel);
    void cyclotronIdle(uint8_t heatLevel);
    void cyclotronIdleFullInit(uint8_t heatLevel);
    void cyclotronIdleFull(uint8_t heatLevel);
    // void bouncingFromCenter(bool initialize, uint8_t level);
    // void shootingFromCenter(bool initialize, uint8_t level);
    // void bouncingFromBottom(bool initialize);
    // void idleTwo(bool initialize);
    void fireInit(bool direction);
    void fireInit(bool direction, bool repeat);
    void fire(uint8_t heatLevel);
    void fire(uint8_t heatLevel, bool repeat);

protected:
    const uint8_t *P_SEG_NUMBER;
    uint32_t _currentTime;
    uint32_t _prevUpdate;
    uint16_t _ledsStatesLow;  // Stores the state for LEDs 0-15
    uint16_t _ledsStatesHigh; // Stores the state for LEDs 16-27
    uint8_t _brightness;
    uint8_t _brightnessPrev;
    bool _direction;
    bool _fillDownInit, _fillDownComplete;
    bool _emptyDownInit, _emptyDownComplete;
    bool _fillUpInit, _fillUpComplete;
    int8_t _tracker;
    uint8_t _prevHeatLevel;
    bool _repeat;

    uint32_t _lastBeatTime, _lastRandomUpdate;
    bool _isPeak;
    const uint8_t _MIN_BASE_LEVEL; // Default lower base level
    const uint8_t _MIN_PEAK_LEVEL; // Peak should rise above level 20
    uint8_t _randomOffset;
    int8_t _currentLevel;
    uint8_t _peakLevel;            // Track the highest LED reached
    uint32_t _prevPeakUpdate;      // Timer for peak drop
    const uint8_t _PEAK_HOLD_TIME; // Peak LED decay interval

    uint8_t _speed;
    uint8_t _corrSpeed;
    bool _update;
    uint8_t _fadeIn;
    uint8_t _fadeOut;

    bool _fillUp(uint8_t speed, bool initialize);
    bool _fillDown(uint8_t speed, bool initialize, bool fadein);
    bool _emptyDown(uint8_t speed, bool initialize, bool fadeout);
    bool _emptyUp(uint8_t speed, bool initialize);
    void _setHigh();
    void _setLow();
    void _setLed(uint8_t index, bool state);
};

class HT16K33Driver : public BarMeterAnimation
{
public:
    HT16K33Driver(const uint8_t *segNumber, const bool *direction, const uint8_t dataPin, const uint8_t clockPin, const uint8_t address, const uint8_t (*mapping)[2]);
    void begin();
    void update();
    void update(uint32_t syncCurrentTime);

private:
    const bool *P_DIRECTION;
    const uint8_t _CLOCK_PIN;
    const uint8_t _DATA_PIN;
    const uint8_t _ADDRESS;
    HT16K33 _driver;
    const uint8_t (*BM_SEG_MAP)[2]; // Pointer to mapping array
};

class MAX72xxDriver : public BarMeterAnimation
{
public:
    MAX72xxDriver(const uint8_t *segNumber, const bool *direction, const uint8_t dataPin, const uint8_t clockPin, const uint8_t loadPin, const uint8_t (*mapping)[2]);
    void begin();
    void update();
    void update(uint32_t syncCurrentTime);

private:
    const bool *P_DIRECTION;
    const uint8_t _DATA_PIN;
    const uint8_t _CLOCK_PIN;
    const uint8_t _LOAD_PIN;
    MAX72xx _driver;
    const uint8_t (*BM_SEG_MAP)[2]; // Pointer to mapping array
};

#endif
