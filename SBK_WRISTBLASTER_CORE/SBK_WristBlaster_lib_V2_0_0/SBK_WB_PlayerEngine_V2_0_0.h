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
 *
 *  @brief DFRobot DFPlayer Mini (DFR0299)-compatible audio player engine.
 *
 *  This engine controls a DFPlayer Mini-compatible module through its native
 *  9600-baud UART command protocol. It supports numbered-track playback,
 *  single-track looping, volume control (0-30), microSD/TF-card playback,
 *  optional BUSY-pin monitoring, and external amplifier muting.
 *
 *  This is host-side Arduino firmware support; it does not replace or modify
 *  the firmware installed on the DFPlayer Mini module.
 */

#pragma once

#include <Arduino.h>

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

#ifndef DISABLE_AUDIO_PLAYER
/**
 * @brief UART controller for a DFRobot DFPlayer Mini DFR0299-compatible module.
 *
 * Commands are sent directly as native 10-byte DFPlayer serial frames, so no
 * third-party DFPlayer library is required. The caller supplies an initialized
 * serial Stream (normally a HardwareSerial or SoftwareSerial instance) to
 * begin().
 */
class Player
{
public:
    Player(const uint8_t MAX, uint8_t volume,
           const uint8_t RX_pin, const uint8_t TX_pin,
           const uint8_t BUSY_pin, const uint8_t amp_MUTE_pin,
           const uint8_t pot_pin, const bool volPotEnable,
           const uint8_t commandDelay
#ifndef PLAYER_USE_BUSY_PIN
           , const uint8_t audioAdvance
#endif
           );
    void begin(Stream &s);
    void update();
    void update(uint32_t now);
    bool isPlaying();
    bool checkCommandDelay();
    void setThemesPlaymode();
    // void setSinglePlaymode();
    // void setCyclingTrackPlaymode();
    void loopFileNum(int16_t track_num);
#ifdef PLAYER_USE_BUSY_PIN
    void playFileNum(int16_t track_num);
#else
    void playFileNum(int16_t track_num, uint16_t track_length);
#endif
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
    Stream *_serial;
    const uint8_t _VOLUME_MAX;
    uint8_t _volume;
#ifdef PLAYER_USE_BUSY_PIN
    const uint8_t _BUSY_PIN;
#endif
    const uint8_t _AMP_MUTE_PIN;
    const uint8_t _POT_PIN;
    const bool _VOL_POT_ENABLE;
    const uint8_t _COMMAND_DELAY;
#ifndef PLAYER_USE_BUSY_PIN
    const uint8_t _AUDIO_ADVANCE;
#endif
    uint32_t _now;
    uint32_t _startTime;
    uint32_t _prevVolumePotTime;
    uint32_t _trackDuration;
    bool _playing;
    uint16_t _prevAnalogRead;
    uint32_t _lastCommand;
    bool _mute;
    void _sendCommand(uint8_t command, uint16_t parameter = 0);
    void _muteAmp(bool enable);
    void _volumeAmpMute();
};
#else
// Timing-only player used for animation testing on memory-constrained boards.
// It keeps non-looping states active for their configured track duration.
class Player
{
public:
    Player(const uint8_t, uint8_t, const uint8_t, const uint8_t,
           const uint8_t, const uint8_t, const uint8_t, const bool,
           const uint8_t
#ifndef PLAYER_USE_BUSY_PIN
           , const uint8_t
#endif
           )
        : _startTime(0), _trackDuration(0), _playing(false), _looping(false) {}

    void begin(Stream &) {}
    void update() { update(millis()); }
    void update(uint32_t now)
    {
        if (_playing && !_looping && now - _startTime >= _trackDuration)
            _playing = false;
    }
    bool isPlaying() { update(millis()); return _playing; }
    bool checkCommandDelay() { return true; }
    void setThemesPlaymode() {}
    // void setSinglePlaymode() {}
    // void setCyclingTrackPlaymode() {}
    void loopFileNum(int16_t) { _startTime = millis(); _playing = true; _looping = true; }
#ifdef PLAYER_USE_BUSY_PIN
    void playFileNum(int16_t) { _startTime = millis(); _playing = true; _looping = false; }
#else
    void playFileNum(int16_t, uint16_t trackLength)
    {
        _startTime = millis();
        _trackDuration = trackLength;
        _playing = true;
        _looping = false;
    }
#endif
    void stop() { _playing = false; _looping = false; }
    void pause() { _playing = false; }
    void next() {}
    void previous() {}
    void setVol(uint8_t) {}
    void defineVolumePot() {}
    uint8_t setVolWithPotAtStart() { return 0; }
    uint8_t setVolWithPot() { return 0; }
    void muteAmp(bool) {}

private:
    uint32_t _startTime;
#ifndef PLAYER_USE_BUSY_PIN
    uint32_t _trackDuration;
#endif
    bool _playing;
    bool _looping;
};
#endif
