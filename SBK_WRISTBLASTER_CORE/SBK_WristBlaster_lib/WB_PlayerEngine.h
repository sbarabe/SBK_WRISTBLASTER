/*
 *  WB_PlayerEngine.h is a part of SBK_WRISTBLASTER_CORE (Version 0) code for sound effects and animations of a Wrist Blaster replica
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

#ifndef WB_PLAYERENGINE_H
#define WB_PLAYERENGINE_H

#include <Arduino.h>
#include <DFPlayerMini_Fast.h>

/* GENERAL HELPERS */
#ifndef DISABLE
#define DISABLE 0
#endif
#ifndef ENABLE
#define ENABLE 1
#endif
// helper for audio track looping
#ifndef NOLOOP
#define NOLOOP 0
#endif
#ifndef LOOP
#define LOOP 1
#endif

class Player
{
public:
    Player(const uint8_t MAX, uint8_t volume,
           const uint8_t RX_pin, const uint8_t TX_pin,
           const uint8_t BUSY_pin, const uint8_t amp_MUTE_pin,
           const uint8_t pot_pin, const bool volPotEnable,
           const uint8_t commandDelay, 
           const uint8_t audioAdvance,
           const bool busyPinEneable);
    bool begin(Stream &s);
    void update();
    void update(uint32_t syncCurrentTime);
    bool isPlaying();
    bool checkCommandDelay();
    void setThemesPlaymode();
    void setSinglePlaymode();
    void setCyclingTrackPlaymode();
    void loopFileNum(int16_t track_num);
    void playFileNum(int16_t track_num);
    void playFileNum(int16_t track_num, uint16_t track_length);
    void stop();
    void pause();
    void next();
    void previous();
    void setVol(uint8_t volume);
    void defineVolumePot();
    uint8_t setVolWithPotAtStart();
    uint8_t setVolWithPot();
    void muteAmp(bool enable);

private:
    DFPlayerMini_Fast _player;
    const uint8_t _VOLUME_MAX;
    uint8_t _volume;
    const uint8_t _RX_PIN;
    const uint8_t _TX_PIN;
    const uint8_t _BUSY_PIN;
    const uint8_t _AMP_MUTE_PIN;
    const uint8_t _POT_PIN;
    const bool _VOL_POT_ENABLE;
    const uint8_t _COMMAND_DELAY;
    const uint8_t _AUDIO_ADVANCE;
    const bool _BUSY_PIN_ENABLE;
     uint32_t _currentTime;
    uint32_t _startTime;
    uint32_t _startTimePrev;
    uint8_t _prevVolume;
    uint32_t _prevVolumePotTime ;
    uint32_t _trackDuration;
    bool _playing;
    uint16_t _prevAnalogRead;
    uint32_t _lastCommand;
    bool _mute;
    void _muteAmp(bool enable);
    void _volumeAmpMute();
};

#endif