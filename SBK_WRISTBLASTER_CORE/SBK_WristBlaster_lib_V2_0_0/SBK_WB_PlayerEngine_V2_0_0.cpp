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
 *  @brief Native UART driver for the DFRobot DFPlayer Mini (DFR0299).
 *
 *  The command frames implemented below follow the DFPlayer Mini serial
 *  protocol and are intended to remain compatible with genuine DFR0299 units
 *  and modules implementing the same command set.
 */

#include "SBK_WB_PlayerEngine_V2_0_0.h"

namespace
{
constexpr uint8_t DFPLAYER_NEXT = 0x01;
constexpr uint8_t DFPLAYER_PREVIOUS = 0x02;
constexpr uint8_t DFPLAYER_PLAY_TRACK = 0x03;
constexpr uint8_t DFPLAYER_SET_VOLUME = 0x06;
constexpr uint8_t DFPLAYER_SET_EQ = 0x07;
constexpr uint8_t DFPLAYER_LOOP_TRACK = 0x08;
constexpr uint8_t DFPLAYER_SET_SOURCE = 0x09;
constexpr uint8_t DFPLAYER_PAUSE = 0x0E;
constexpr uint8_t DFPLAYER_STOP = 0x16;
constexpr uint8_t DFPLAYER_REPEAT_FOLDER = 0x17;
constexpr uint8_t DFPLAYER_REPEAT_CURRENT = 0x19;
constexpr uint8_t DFPLAYER_SET_DAC = 0x1A;
}

// #define DEBUG_TO_SERIAL
#ifdef DEBUG_TO_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

/////////////////////////////////////////////////////
/*                                                 */
/************* DFPlayer Mini section ***************/
/*   DFR0299-compatible native UART protocol       */
/////////////////////////////////////////////////////

Player::Player(const uint8_t MAX, uint8_t volume,
               const uint8_t RX_pin, const uint8_t TX_pin,
               const uint8_t BUSY_pin, const uint8_t amp_MUTE_pin,
               const uint8_t pot_pin, const bool volPotEnable,
               const uint8_t commandDelay
#ifndef PLAYER_USE_BUSY_PIN
               , const uint8_t audioAdvance
#endif
               )
    : _serial(nullptr),
      _VOLUME_MAX(constrain(MAX, 0, 30)),
      _volume(volume),
#ifdef PLAYER_USE_BUSY_PIN
      _BUSY_PIN(BUSY_pin),
#endif
      _AMP_MUTE_PIN(amp_MUTE_pin),
      _POT_PIN(pot_pin),
      _VOL_POT_ENABLE(volPotEnable),
      _COMMAND_DELAY(commandDelay),
#ifndef PLAYER_USE_BUSY_PIN
      _AUDIO_ADVANCE(audioAdvance),
#endif
      _now(0),
      _startTime(0),
      _prevVolumePotTime(0),
#ifndef PLAYER_USE_BUSY_PIN
      _trackDuration(0),
#endif
      _playing(false),
      _prevAnalogRead(0),
      _lastCommand(0),
      _mute(true)
{
}

void Player::begin(Stream &s)
{
  _serial = &s;
#ifdef PLAYER_USE_BUSY_PIN
  pinMode(_BUSY_PIN, INPUT);
#endif
  muteAmp(true);

  if (_VOL_POT_ENABLE)
    pinMode(_POT_PIN, INPUT);

  delay(_COMMAND_DELAY);
  setVol(_volume);
  delay(_COMMAND_DELAY);
  _sendCommand(DFPLAYER_SET_SOURCE, 2); // TF/microSD card
  delay(_COMMAND_DELAY);
  _sendCommand(DFPLAYER_SET_EQ, 0); // Normal EQ
  delay(_COMMAND_DELAY);
  _sendCommand(DFPLAYER_STOP);
  delay(_COMMAND_DELAY);
  _sendCommand(DFPLAYER_SET_DAC, 0); // Enable DAC
  delay(_COMMAND_DELAY);
  _sendCommand(DFPLAYER_REPEAT_CURRENT, 1); // Disable repeat-current mode
  delay(_COMMAND_DELAY);
  _lastCommand = _now;
}

void Player::update() { update(millis()); }

void Player::update(uint32_t now)
{
  _now = now;
}

uint8_t Player::setVolWithPotAtStart()
{
  if (_VOL_POT_ENABLE)
  {
    uint16_t potValue;
    uint8_t newVolume = _volume;
    if (_VOL_POT_ENABLE)
    {
      potValue = analogRead(_POT_PIN);
      newVolume = (uint8_t)map(potValue, 10, 1000, 0, _VOLUME_MAX);
    }
    if (newVolume != _volume)
    {
      _volume = newVolume;
      _sendCommand(DFPLAYER_SET_VOLUME, newVolume);

      _lastCommand = _now; // Note when player's command is passed for delay check

      _volumeAmpMute(); // Mute amp if volume is zero
    }
  }

  return _volume;
}

uint8_t Player::setVolWithPot()
{
  if (_VOL_POT_ENABLE)
  {

    if (_now - _prevVolumePotTime >= 200) // Time Hysteresis
    {
      _prevVolumePotTime = _now;

      uint16_t potValue = analogRead(_POT_PIN); // Read potentiometer
      const uint8_t ANALOG_HYSTERESIS = 10;     // Define a small buffer (adjustable)

      // Only update if the change is significant
      if (abs(potValue - _prevAnalogRead) > ANALOG_HYSTERESIS)
      {
        _prevAnalogRead = potValue; // Store the new analog read

        // Map to volume range (0 - _VOLUME_MAX)
        uint8_t newVolume = map(potValue, 0, 1023, 0, _VOLUME_MAX);

        // Apply volume only if it's different
        if (newVolume != _volume)
        {
          _volume = newVolume;
          _sendCommand(DFPLAYER_SET_VOLUME, newVolume); // Apply volume change

          _lastCommand = _now; // Note when player's command is passed for delay check

          _volumeAmpMute(); // Mute amp if volume is zero
        }
      }
    }
  }
  return _volume;
}

void Player::setVol(uint8_t volume)
{
  _volume = constrain(volume, 0, _VOLUME_MAX);
  _sendCommand(DFPLAYER_SET_VOLUME, _volume);
  _lastCommand = _now; // Note when player's command is passed for delay check

  _volumeAmpMute(); // Temporary mute amp if volume is zero
}

bool Player::isPlaying()
{
  bool playingNow = _playing;

#ifdef PLAYER_USE_BUSY_PIN
    // Use BUSY pin to determine play state
    if (_now - _startTime < 200)
      playingNow = true; // Recently started, give it time to stabilize
    else
      playingNow = !digitalRead(_BUSY_PIN); // Low means playing, High means stopped
#else
    // Fallback: Check track duration if no BUSY pin
    playingNow = (_now - _startTime) < _trackDuration;
#endif

  // Update and log only if there's a change in playing state
  if (playingNow != _playing)
  {
    _playing = playingNow;

    DEBUG_PRINTLN(_playing ? "Player START" : "Player STOP");
  }

  return _playing;
}

bool Player::checkCommandDelay() { return (_now - _lastCommand >= _COMMAND_DELAY); }

void Player::setThemesPlaymode()
{
  _sendCommand(DFPLAYER_REPEAT_FOLDER, 1);
  _lastCommand = _now; // Note when player's command is passed for delay check
}

// Kept for reference: the DFR0299 protocol has no separate command needed by
// this firmware to select single-track or looping-track mode. Playback mode is
// chosen directly with playFileNum() or loopFileNum().
//
// void Player::setSinglePlaymode()
// {
//   // No separate function is available or required.
// }
//
// void Player::setCyclingTrackPlaymode()
// {
//   // No separate function is available or required.
// }

void Player::loopFileNum(int16_t track_num)
{
  muteAmp(false);

  _sendCommand(DFPLAYER_LOOP_TRACK, static_cast<uint16_t>(track_num));
  _lastCommand = _now; // Note when player's command is passed for delay check

  _startTime = _now; // ???? NOT REQUIRED ????
#ifndef PLAYER_USE_BUSY_PIN
  _trackDuration = 0;
#endif
}

#ifdef PLAYER_USE_BUSY_PIN
void Player::playFileNum(int16_t track_num)
#else
void Player::playFileNum(int16_t track_num, uint16_t track_length)
#endif
{
  muteAmp(false);

  _sendCommand(DFPLAYER_PLAY_TRACK, static_cast<uint16_t>(track_num));
  _lastCommand = _now; // Note when player's command is passed for delay check

  _startTime = _now; // To track file end playing with time...

#ifndef PLAYER_USE_BUSY_PIN
  uint32_t validDuration = track_length > _AUDIO_ADVANCE
                               ? track_length - _AUDIO_ADVANCE
                               : 0;
  _trackDuration = max(_COMMAND_DELAY, validDuration);   // Ensures it's at least _COMMAND_DELAY
#endif
}

void Player::stop()
{
  muteAmp(true);

  _sendCommand(DFPLAYER_STOP);
  _lastCommand = _now; // Note when player's command is passed for delay check
}

void Player::pause()
{
  _sendCommand(DFPLAYER_PAUSE);
_lastCommand = _now; // Note when player's command is passed for delay check

  muteAmp(true);
  
}

void Player::next()
{
  muteAmp(false);
 
  _sendCommand(DFPLAYER_NEXT);
  _lastCommand = _now; // Note when player's command is passed for delay check
}

void Player::previous()
{
  muteAmp(false);

  _sendCommand(DFPLAYER_PREVIOUS);
  _lastCommand = _now; // Note when player's command is passed for delay check
}

void Player::muteAmp(bool enable) // Cute possible background noise and save power
{
  if (_mute != enable)
  {
    _mute = enable;
    _muteAmp(_mute); // Amp is muted when pin is High
  }
}

void Player::_muteAmp(bool enable) { digitalWrite(_AMP_MUTE_PIN, enable); } // Amp is muted when pin is High

void Player::_volumeAmpMute()
{
  if (_volume <= 1 && !_mute)
  {
    _muteAmp(true);
  }
  else if (_volume > 1 && !_mute)
  {
    _muteAmp(false);
  }
}

void Player::_sendCommand(uint8_t command, uint16_t parameter)
{
  if (!_serial)
    return;

  constexpr uint8_t VERSION = 0xFF;
  constexpr uint8_t LENGTH = 0x06;
  constexpr uint8_t NO_FEEDBACK = 0x00;
  const uint8_t parameterMsb = highByte(parameter);
  const uint8_t parameterLsb = lowByte(parameter);
  const uint16_t checksum = 0U - static_cast<uint16_t>(
                                      VERSION + LENGTH + command + NO_FEEDBACK +
                                      parameterMsb + parameterLsb);
  const uint8_t frame[10] = {
      0x7E, VERSION, LENGTH, command, NO_FEEDBACK,
      parameterMsb, parameterLsb, highByte(checksum), lowByte(checksum), 0xEF};

  for (uint8_t byte : frame)
    _serial->write(byte);
}
