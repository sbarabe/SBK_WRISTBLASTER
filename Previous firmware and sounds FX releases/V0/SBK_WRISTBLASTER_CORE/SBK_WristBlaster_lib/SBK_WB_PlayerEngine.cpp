/*
 *  This code is part of SBK_WRISTBLASTER_CORE (VERSION 0), a codebase for animations and effects
 *  of a Wrist Blaster prop inspired by the movie Ghostbusters: Frozen Empire.
 *  Copyright (c) 2025 Samuel Barabé
 *
 *  For more information, visit the project page: <https://github.com/sbarabe/SBK_WRISTBLASTER_CORE>.
 *
 *  This work is licensed under the Creative Commons Attribution 4.0 International License (CC BY 4.0).
 *  You are free to share, copy, and modify this code as long as appropriate credit is given to the author.
 *  See the full license at: <https://creativecommons.org/licenses/by/4.0/>.
 *
 *  This code is provided "as-is" without any warranty of any kind, either expressed or implied,
 *  including but not limited to the warranties of merchantability or fitness for a particular purpose.
 *  See the full license text for more details.
 */

#include "SBK_WB_PlayerEngine.h"

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
/*       (with DFPlayerMini_Fast.h library)       */
/////////////////////////////////////////////////////

Player::Player(const uint8_t MAX, uint8_t volume,
               const uint8_t RX_pin, const uint8_t TX_pin,
               const uint8_t BUSY_pin, const uint8_t amp_MUTE_pin,
               const uint8_t pot_pin, const bool volPotEnable,
               const uint8_t commandDelay,
               const uint8_t audioAdvance,
               const bool busyPinEnable)
    : _VOLUME_MAX(constrain(MAX, 0, 30)),
      _volume(volume),
      _RX_PIN(RX_pin),
      _TX_PIN(TX_pin),
      _BUSY_PIN(BUSY_pin),
      _AMP_MUTE_PIN(amp_MUTE_pin),
      _POT_PIN(pot_pin),
      _VOL_POT_ENABLE(volPotEnable),
      _COMMAND_DELAY(commandDelay),
      _AUDIO_ADVANCE(audioAdvance),
      _BUSY_PIN_ENABLE(busyPinEnable),
      _currentTime(0),
      _startTime(0),
      _startTimePrev(0),
      _prevVolume(0), _prevVolumePotTime(0),
      _trackDuration(0),
      _playing(false),
      _prevAnalogRead(0),
      _lastCommand(0),
      _mute(true)
{
}

bool Player::begin(Stream &s)
{
  pinMode(_BUSY_PIN, INPUT);
  muteAmp(true);

  if (_VOL_POT_ENABLE)
    pinMode(_POT_PIN, INPUT);

  if (_player.begin(s, false, 50))
  {
    delay(_COMMAND_DELAY);
    setVol(_volume);
    delay(_COMMAND_DELAY);
    _player.playbackSource(2);
    delay(_COMMAND_DELAY);
    _player.EQSelect(1);
    delay(_COMMAND_DELAY);
    _player.stop();
    delay(_COMMAND_DELAY);
    _player.startDAC();
    delay(_COMMAND_DELAY);
    _player.stopRepeat();
    delay(_COMMAND_DELAY);
    _lastCommand = _currentTime; // Note when player's command is passed for delay check
    return true;
  }
  else
  {
    return false;
  }
}

void Player::update() { update(millis()); }

void Player::update(uint32_t syncCurrentTime)
{
  _currentTime = syncCurrentTime;
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
      _player.volume(newVolume);

      _lastCommand = _currentTime; // Note when player's command is passed for delay check

      _volumeAmpMute(); // Mute amp if volume is zero
    }
  }

  return _volume;
}

uint8_t Player::setVolWithPot()
{
  if (_VOL_POT_ENABLE)
  {

    if (_currentTime - _prevVolumePotTime >= 200) // Time Hysteresis
    {
      _prevVolumePotTime = _currentTime;

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
          _player.volume(newVolume); // Apply volume change

          _lastCommand = _currentTime; // Note when player's command is passed for delay check

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
  _player.volume(_volume);
  _lastCommand = _currentTime; // Note when player's command is passed for delay check

  _volumeAmpMute(); // Temporary mute amp if volume is zero
}

bool Player::isPlaying()
{
  bool playingNow = _playing;

  if (_BUSY_PIN_ENABLE)
  {
    // Use BUSY pin to determine play state
    if (_currentTime - _startTime < 200)
      playingNow = true; // Recently started, give it time to stabilize
    else
      playingNow = !digitalRead(_BUSY_PIN); // Low means playing, High means stopped
  }
  else
    // Fallback: Check track duration if no BUSY pin
    playingNow = (_currentTime - _startTime) < _trackDuration;

  // Update and log only if there's a change in playing state
  if (playingNow != _playing)
  {
    _playing = playingNow;

    DEBUG_PRINTLN(_playing ? "Player START" : "Player STOP");
  }

  return _playing;
}

bool Player::checkCommandDelay() { return (_currentTime - _lastCommand >= _COMMAND_DELAY); }

void Player::setThemesPlaymode()
{
  _player.repeatFolder(1);
  _lastCommand = _currentTime; // Note when player's command is passed for delay check
}

void Player::setSinglePlaymode()
{
  //  no function available for this in the library

  // _lastCommand = _currentTime; // Note when player's command is passed for delay check
}

void Player::setCyclingTrackPlaymode()
{
 //  no function available for this in the library

  // _lastCommand = _currentTime; // Note when player's command is passed for delay check
}

void Player::loopFileNum(int16_t track_num)
{
  muteAmp(false);

  _player.loop(track_num);
  _lastCommand = _currentTime; // Note when player's command is passed for delay check

  _startTime = _currentTime; // ???? NOT REQUIRED ????
  _trackDuration = 0;        // No need for track duration since looping
}

void Player::playFileNum(int16_t track_num, uint16_t track_length)
{
  muteAmp(false);

  _player.play(track_num);
  _lastCommand = _currentTime; // Note when player's command is passed for delay check

  _startTime = _currentTime; // To track file end playing with time...

  uint32_t durationAfterAdvance = track_length - _AUDIO_ADVANCE;
  uint32_t validDuration = max(0, durationAfterAdvance); // Ensures non-negative value
  _trackDuration = max(_COMMAND_DELAY, validDuration);   // Ensures it's at least _COMMAND_DELAY
}

void Player::stop()
{
  muteAmp(true);

  _player.stop();
  _lastCommand = _currentTime; // Note when player's command is passed for delay check
}

void Player::pause()
{
  _player.pause();
_lastCommand = _currentTime; // Note when player's command is passed for delay check

  muteAmp(true);
  
}

void Player::next()
{
  muteAmp(false);
 
  _player.playNext();
  _lastCommand = _currentTime; // Note when player's command is passed for delay check
}

void Player::previous()
{
  muteAmp(false);

  _player.playPrevious();
  _lastCommand = _currentTime; // Note when player's command is passed for delay check
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
