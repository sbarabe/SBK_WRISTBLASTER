/**********************************************************************************************
 *    SBK_WRISTBLASTER_CORE is a code for lights and sound effects of a Wrist Blaster
 *    replica or other props.
 *    Copyright (c) 2025 Samuel Barabé
 *    Special thanks to David Miyakawa for animations, sounds effects and work flow researches and design.
 *
 *    This program is free software: you can redistribute it and/or modify it under the terms
 *    of the GNU General Public License as published by the Free Software Foundation, either
 *    version 3 of the License, or any later version.
 *
 *    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *    See the GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along with this program.
 *    If not, see <https://www.gnu.org/licenses/>.
 ***********************************************************************************************/

/**********************************************************************************************
 *    GENERAL INFO :
 *
 *    SBK_WRISTBLASTER_CORE
 *    <https://github.com/sbarabe/SBK_WRISTBLASTER_CORE>
 *    Version 0
 *
 *    This code was first intended to be use with the SBK Kid Wrist Blaster
 *    <https://github.com/sbarabe/SBK-KidSizeProtonPackArduino> but the code has been extended to
 *    be a configurable code for any wrist plaster.
 *
 *    Lot of time and resources have been invests providing this open-source code, please support
 *    the author if you use or like this code by visiting the GitHub page and donating,
 *    it may help keeping this project alive and up to date. Requests for correcting/adding features
 *    can be made by creating a New Issue on the GitHub page :
 *    <https://github.com/sbarabe/SBK_WRISTBLASTER_CORE>
 *
 *    Now, only one type of player module and two bar meter drivers are supported, but others could
 *    be added (if requested) without modification to the main code.
 *
 *    Other animations could also be added upon request, but main sketch should also be updated to
 *    use these new animations.
 *
 *    Features included:
 *    - DFPlayer mini sound board.
 *    - BAR METER : up to 28 segments (configurable) controlled by MAX72xx or HT16K33 drivers
 *    - Vent, indicators and cylcotron are WS2812 led type: there is one defined chain
 *      for the wrist blaster (blasterLeds).
 *    - WS2812 LEDs positions can be set in their header section.
 *    - LEDs animations are defined in object functions. Class objects and functions are in separated
 *      files to keep the main sketch as short as possible.
 *    - There is an option for volume potentiometer that uses software volume control with the audio player :
 *      If disable in the config file, the volume will be the one declare in the AUDIO PLAYER configuration section.
 *    - There is the following regular switches and buttons :
 *         MAIN BOOT switch :        Power on/off the wrist blaster.
 *         CYCLOTRON switch :        Power on/Off the cyclotron, reuiqred for shooting
 *         ACTIVATE switch :         Toggle between cyclotron normal (capture fire) to full power (burst fire).
 *         INTENSIFY button/switch : Toggle party mode for music
 *         FIRE button :             It does what it says... It also switch previous/next themes in party mode, or enbale/disable smoke in POWER OFF state.
 *         Fire2 button :            Same as fire button but play previous themes in themes playing mode.
 *    - Smoke module : can be enable/disable from POWER OFF state with the Fire Button, always disable when powering on the MCU.
 *    - Firing ROD hue potentiometer : you can change the firing rod hue with a potentiometer, can be enable/disable from the config file.
 *
 * *************************************************************************************************
 *    HOW TO USE THE CODE :
 *
 *    This code is based on objects programming, objects are defined in their *.h and *.cpp files located in the SBK_WristBlaster_lib folder. 
 *    Objects instances are created and used in the core file SBK_WRISTBLASTER_CORE.ino. There is also a config file,
 *    SBK_WRISTBLASTER_CONFIG.h, where all basic parameters can be changed according to hardware used. This fully
 *    compartmented code helps keeping code cleaner and easier to update/maintain. This also prevent having
 *    an endless code hard to follow.
 *
 *    The sketch folder should be named 'SBK_WRISTBLASTER_CONFIG', same as the SBK_WRISTBLASTER_CONFIG.ino file. 
 *    It should contain both SBK_WRISTBLASTER_CONFIG.ino and SBK_WRISTBLASTER_CONFIG.h files.
 *    When you opne the .ino file in the IDE, you will see both CORE and CONFIG files in the IDE : 
 *    the config file should be the one you make some setting,
 *    the core file should be left as it is.
 *
 *    In your Arduino IDE libraries folder, you should place SBK_WristBlaster_lib folder : it contains all the specific libraries needed
 *    except the Adafruit_NeoPixel.h and the DFPlayerMine_Fast.h.
 *    They should be find with the IDE libraries search tool.
 *    
 *    The SBK_WRISTBLASTER_CONFIG.h file contains all the definitions and options : pins, player module, LEDs index,
 *    audio tracks, etc. This is the file you want to customize to adjust your pins setting, LEDs chain and index,
 *    audio tracks info and other options. If you like the animations and the wrist blaster workflow, you should not have
 *    to change anything else then the config file.
 *
 *    Be careful to adjust animations times/speed regarding your audio tracks/pact states durations. They are all
 *    gathered in the getLEDsSchemeForThisState() function after the Main Loop in the core file. CAUTION, playing
 *    with times and speeds can really mess around the animations flow, it is highly suggested taking notes of the
 *    original values and changing a few of them at the time and see the effects.
 *
 *    Other libraries are required to be installed in your IDE too, according to your devices
 *    and modules :
 *
 *    - For WS2812 LEDs :      Adafruit_NeoPixel.h (https://github.com/adafruit/Adafruit_NeoPixel)
 *    - For DFPlayer Mini :    DFPlayerMine_Fast.h (https://github.com/PowerBroker2/DFPlayerMini_Fast)
 *
 *    *** ADVANCED USERS ONLY ***
 *    If you want to change animations styles and colors, you need to go in the engines files and modified the
 *    associated functions or create new ones. Then you will have to implement them in the getLEDsSchemeForThisState()
 *    function in the core file.
 *
 *    If you want to change the wrist blaster state switch/cases workflow, you will have to modify wrist blaster states list and audio
 *    tracks list/length/looping in the CONFIG file, and the Main Loop pact states switch/cases contents.
 *
 *    Sketch mechanic works with different wrist blaster states and transitions in the Wrist Blaster state switches/cases defined
 *    in Main Loop. Each wrist blaster state has is initialization stage (stageFlag 0) and looping stages (stageFlag 1, 2, ...). The
 *    looping stages includes exit(s) to other states : switches actions, buttons actions and audio track ending.
 *
 ***********************************************************************************************/

#include <Arduino.h>
#include <Wire.h> // Include the I2C library (required)
#include "SBK_WRISTBLASTER_CONFIG.h"

/*********************************************/
/*                                           */
/*      GENERAL definitions and helpers      */
/*                                           */
/*********************************************/
// Helpers variables declarations and initial states :
uint32_t currentTime = 0;               // To keep track in a loop
BlasterState WBstate = STATE_POWER_OFF; // Initial wrist blaster state in the main loop
BlasterState prevState = STATE_ZERO;    // State tracking for some sequences
uint8_t stageFlag = 0;                  // stage flag to implement different state stages in main loop
uint8_t prevStageFlag = 100;            // stage flag tracking
uint32_t stateStartTime = 0;            // general time tracker for functions timers and delays
int8_t playingTrack = -1;               // Record the actual track playing
uint8_t heatLevel = 0;                  // Tracker for overheat
uint32_t heatLevelPrevUpdate = 0;       // Tracker for overheat
bool fireType = CAPTURE;                // 0 = Capture, 1 = Burst : help managing reboots and firing tails
// Helpers functions declarations, functions are defined after the main loop :
uint8_t stateInitialization();                                      // Standard initialisation sequence for most state
void clearAllLights();                                              // SHUTOFF all leds for wrist blaster and resets some trackers
bool checkIfTrackDoneExit(BlasterState next_state);                 // check if a state track is done playing and go to next stage
bool checkIfSwitchExit(bool switch_state, BlasterState next_state); // check if a switch action and go to next stage
bool checkIfTimerExit(uint16_t time, BlasterState next_state);      // check if a timer is done and go to next stage
void getLEDsSchemeForThisState();                                   // This function contains animations settings and calling for all states
void playThisStateTrack();                                          // play state track
void playThisTrack(uint8_t track);                                  // Play specific track other then state strack
bool checkPlayModeForThisState();                                   // check if play mode is correct for this state (looping / not looping)
uint16_t getDuration();                                             // Get actual state duration
uint16_t getSpecificDuration(BlasterState state);                   // Get duration of a specific state
void checkNextPreviousButton();                                     // While in praty mode, switch to previous/next song with fire button
void heatLevelCooling();                                            // Manage heat level
void heatLevelRisingBurst();                                        // Manage heat level
void heatLevelRisingCapture();                                      // Manage heat level
void checkSmokerEnabling();                                         // Check fire button operation to enable/disable smoke effect
uint8_t getCaptureScaledDuration();                                 // Scale Capture state duration to be compatible with heat level management
bool getPartyModeState();                                           // Helper to manage PBIntensify output as a switch or a push button

/*********************************************/
/*           BAR METER & DRIVER(s)           */
/*********************************************/
//  BAR METER with shift registers
//  Bar meter helper variables for 28 segements bar meter:
//  DRIVER type, animations DIRECTION and segments MAPPING should be defined in SBK_WRISTBLASTER_CONFIG.h file
#ifdef BM_MAX72xx
MAX72xxDriver barmeter(&SEG_NUMBER, &BM_DIRECTION, BM_DIN_PIN, BM_CLK_PIN, BM_LOAD_PIN, BM_SEG_MAP);
#elif defined(BM_HT16K33)
HT16K33Driver barmeter(&SEG_NUMBER, &BM_DIRECTION, BM_DIN_PIN, BM_CLK_PIN, BM_ADDRESS, BM_MAPPING);
#endif

/***********************************************/
/*               WS2812 LEDs strip             */
/***********************************************/
//  LEDs index, positions and animations directions should be defined in SBK_WRISTBLASTER_CONFIG.h file
Adafruit_NeoPixel blasterLeds = Adafruit_NeoPixel(TOTAL_LEDS_NUMBER, LEDS_PIN, NEO_GRB + NEO_KHZ800);

FiringRod firingRod(&blasterLeds,
                    FIRE_ROD_POT_PIN, HUE_POT_READY,
                    &ROD_NUMLEDS, &LED_INDEX_TIP_1ST, &LED_INDEX_TIP_LAST);
Vent vent(&blasterLeds, &LED_INDEX_VENT);
Indicator slowBlowIndicator(&blasterLeds, &LED_INDEX_SLOWBLOW, "IND_SlBlw");
Indicator topYellowIndicator(&blasterLeds, &LED_INDEX_TOP_YELLOW, "IND_topYw");
Indicator topWhiteIndicator(&blasterLeds, &LED_INDEX_TOP_WHITE, "IND_topWh");
Indicator frontOrangeIndicator(&blasterLeds, &LED_INDEX_FRONT_ORANGE, "IND_frOr");
Cyclotron cyclotron(&blasterLeds,
                    &CYC_NUMLEDS, &LED_INDEX_CYC_START, &LED_INDEX_CYC_END,
                    &CYC_RING_1ST, &CYC_RING_LAST, &CYC_CENTER,
                    &CYCLOTRON_DIRECTION);

/***********************************************/
/*      Fire button Single Led Indicator       */
/***********************************************/
// This not a an adressable LED, it's a single led drive directly from the pin and a serie resistor
// See CONFIG.H to enable/disable this indicator
SingleColorIndicator fireButtonSingleLed(FIRE_BUTTON_LED_PIN, FIRE_BUTTON_LED_READY, "IND_fireBtn");

/*********************************************/
/*    AUDIO PLAYER definition and helpers    */
/*********************************************/
bool playing = false; // variable for playin status
bool cycling = false; // cylcing single track mode tracker
/****************************/
/*    PLAYER definitions    */
/****************************/
Player player(VOLUME_MAX, VOLUME_START,
              SW_RX_PIN, SW_TX_PIN, BUSY_PIN,
              AMP_MUTE_PIN,
              VOL_POT_PIN, VOL_POT_READY,
              PLAYER_COMMAND_DELAY,
              AUDIO_ADVANCE,
              BUSY_PIN_READY);
/************************************/
/* Audio board SERIAL COMMUNICATION */
/************************************/
SoftwareSerial SoftSerial(SW_RX_PIN, SW_TX_PIN);

/*********************************************/
/*                                           */
/* BUTTONS & SWITCHES definition and helpers */
/*                                           */
/*********************************************/
// For switches and buttons managing
Switch PBintensify(INTENSIFY_BUTTON_PIN, REVERSE_LOGIC, "SW Intensify");
Switch SWmain(MAIN_POWER_SWITCH_PIN, REVERSE_LOGIC, "SW Main");
Switch SWcyclotron(CYCLOTRON_POWER_SWITCH_PIN, REVERSE_LOGIC, "SW Cyclotron");
Switch SWactivate(ACTIVATE_SWITCH_PIN, REVERSE_LOGIC, "SW Activate");
Switch PBfire(FIRE_BUTTON_PIN, DIRECT_LOGIC, "Fire Button");

/*********************************************/
/*                SMOKER & FAN               */
/*********************************************/
Smoker smoker(SMOKE_RELAY_PIN, FAN_RELAY_PIN,
              &SMOKER_MIN_OFF_TIME, &SMOKER_MAX_ON_TIME);

//////////////////////////////////////////////////////////////////////////
//////////////////////  ***  SETUP LOOP  ***  ////////////////////////////
//////////////////////////////////////////////////////////////////////////
void setup(void)
{

// Setup Serial.com for troubleshotting OR audio board communication
#ifdef DEBUG_TO_SERIAL
  Serial.begin(DEBUG_BAUDRATE);
#endif

  // Audio player setup
  // Uses Software Serial, pins should be define in SBK_WRISTBLASTER_CONFIG.h
  // Baudrate should be set according to your audio player native baudrate.
  SoftSerial.begin(PLAYER_BAUDRATE);
  if (!player.begin(SoftSerial))
    DEBUG_PRINTLN("Init failed, please check the wire connection!");

  // Enable/disable software volume control with potentiometer
  player.setVolWithPotAtStart();

  // setup LEDs strip :
  blasterLeds.begin();
  blasterLeds.setBrightness(255);
  blasterLeds.clear();
  blasterLeds.show();
  // Setup LEDs strip animations :
  cyclotron.begin();
  vent.begin();
  slowBlowIndicator.begin();
  topWhiteIndicator.begin();
  topYellowIndicator.begin();
  frontOrangeIndicator.begin();

  // Setup Single Led Indicator
  fireButtonSingleLed.begin();

  // setup bar meter
  barmeter.begin();
  barmeter.clear();
  barmeter.update();

  // setup for the switches/buttons
  SWactivate.begin();
  PBintensify.begin();
  SWmain.begin();
  SWcyclotron.begin();
  PBfire.begin();

  // Smoker setup
  smoker.begin(DISABLE);
}
/******************** END_SEQ SETUP LOOP ********************/

//////////////////////////////////////////////////////////////////////////
//////////////////////  ***  MAIN LOOP  ***  /////////////////////////////
//////////////////////////////////////////////////////////////////////////

void loop()
{

// Troubleshooting info on wrist blaster states and stagesthis engine controlled components
#ifdef DEBUG_TO_SERIAL
  {
    if (WBstate != prevState || stageFlag != prevStageFlag)
    {
      DEBUG_PRINT("Wrist Blaster State = ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT("  Stage = ");
      DEBUG_PRINTLN(stageFlag);
      if (WBstate != prevState)
      {
        prevState = WBstate;
      }
      if (stageFlag != prevStageFlag)
      {
        prevStageFlag = stageFlag;
      }
    }
  }
#endif

  // Get time for this loop
  currentTime = millis();

  // LEDS UPDATE
  getLEDsSchemeForThisState(); // Get new leds schemes for this loop
  // Update simple LEDs states to last animations schemes.
  barmeter.update(currentTime);
  fireButtonSingleLed.update(currentTime);
  // Update addressable LEDs chain with last color schemes.
  bool update_leds_chain = false;
  update_leds_chain |= slowBlowIndicator.update(currentTime);
  update_leds_chain |= topWhiteIndicator.update(currentTime);
  update_leds_chain |= topYellowIndicator.update(currentTime);
  update_leds_chain |= frontOrangeIndicator.update(currentTime);
  update_leds_chain |= cyclotron.update(currentTime);
  update_leds_chain |= vent.update(currentTime);
  update_leds_chain |= firingRod.update(currentTime);
  if (update_leds_chain) // Update only if required
    blasterLeds.show();

  // Check buttons and switches readings and states
  PBintensify.update(currentTime);
  SWmain.update(currentTime);
  SWcyclotron.update(currentTime);
  SWactivate.update(currentTime);
  PBfire.update(currentTime);

  // Update smoker and rumbler
  smoker.update(currentTime);

  // DFPlayer Mini Management
  player.update(currentTime);
  player.setVolWithPot(); // Set audio volume with potentiometer
  // Ensure command Delay is done before any other action
  if (!player.checkCommandDelay())
    return;

  ///////////////////////////////////////////////////////////////
  // Actions for different blaster states
  switch (WBstate)
  {
  case STATE_ZERO:
    break;

  //////////////////////////////////////////////
  case STATE_PARTY_MODE:
  {
    switch (stageFlag)
    {
    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_PARTY_MODE");

      // Specifics state initializers :
      player.setThemesPlaymode(); // Play files in folder 01 on SD Card
      playingTrack = -2;          // No state strack is being played
      DEBUG_PRINTLN("Play FOLDER");

      // Standard initializers
      stateStartTime = currentTime;
      stageFlag = 1; // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      // Check Fire button to play next/previous themes track, needs a press and release
      checkNextPreviousButton();

      // Wrist blaster state exits by priority : check if wrist blaster goes into PARTY_MODE_OUT
      if (checkIfSwitchExit(!getPartyModeState(), STATE_PARTY_MODE_OUT))
        break;

      break;
    }
    }
    break;
  }

  //////////////////////////////////////////////
  case STATE_PARTY_MODE_IN:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_PARTY_MODE_IN");

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      // Wrist blaster state exits by priority :
      if (checkIfTrackDoneExit(STATE_PARTY_MODE)) // Check if track is ended before going into STATE_PARTY_MODE
        break;

      break;
    }
    }
    break;
  }

  //////////////////////////////////////////////
  case STATE_PARTY_MODE_OUT:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_PARTY_MODE_OUT");

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      // Wrist blaster state exits by priority :

      // Determine next state :
      BlasterState next = WBstate;
      if (SWmain.isOFF())
        next = STATE_POWER_OFF;
      else if (SWcyclotron.isOFF())
        next = STATE_POWER_OFF_TO_ON;
      else
        next = SWactivate.isON() ? STATE_CYCLOTRON_ON_TO_FULL : STATE_CYCLOTRON_OFF_TO_ON;

      // If track's done, go to next state
      if (checkIfTrackDoneExit(next))
        break;

      break;
    }
    }
    break;
  }

  //////////////////////////////////////////////
  case STATE_POWER_OFF:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_POWER_OFF");

      // Specifics state initializers :
      heatLevel = 0;     // Reset heat level to 0
      player.stop();     // Stop player, no track for this state
      playingTrack = -1; // No state strack is being played
      DEBUG_PRINTLN("Player STOP");

      // Standard initializers
      stateStartTime = currentTime;
      stageFlag = 1; // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      checkSmokerEnabling(); // Enable/disable smoker with fire button while in POWER OFF state

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(getPartyModeState(), STATE_PARTY_MODE_IN)) // Check if the Intensify Switch is ON, goes into STATE_PARTY_MODE_IN
        break;

      if (checkIfSwitchExit(SWmain.isON(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is ON, goes into main booting state STATE_POWER_OFF_TO_ON
        break;

      break;
    }
    }

    break;
  }

  //////////////////////////////////////////////
  case STATE_POWER_OFF_TO_ON:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_POWER_OFF_TO_ON");

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      heatLevelCooling();

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      if (checkIfTrackDoneExit(STATE_POWER_ON)) // Check if track is ended before going into STATE_POWER_ON
        break;

      break;
    }
    }

    break;
  }

    //////////////////////////////////////////////
  case STATE_POWER_ON_TO_OFF:
  {
    switch (stageFlag)
    {
    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_POWER_ON_TO_OFF");

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      heatLevelCooling();

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isON(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is ON, goes into main booting state STATE_POWER_OFF_TO_ON
        break;

      if (checkIfTrackDoneExit(STATE_POWER_OFF)) // Check if track is ended before going into STATE_POWER_OFF
        break;

      break;
    }
    }
    break;
  }

  //////////////////////////////////////////////
  case STATE_POWER_ON:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_POWER_ON");

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      heatLevelCooling();

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(getPartyModeState(), STATE_PARTY_MODE_IN)) // Check if the Intensify Switch is ON, goes into STATE_PARTY_MODE_IN
        break;

      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isON(), SWactivate.isON() ? STATE_CYCLOTRON_ON_TO_FULL : STATE_CYCLOTRON_OFF_TO_ON)) // Check if Cyclotron Switch is ON, goes into cyclotron booting state STATE_CYCLOTRON_OFF_TO_ON
        break;

      break;
    }
    }

    break;
  }

    //////////////////////////////////////////////
  case STATE_CYCLOTRON_OFF_TO_ON:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CYCLOTRON_OFF_TO_ON");

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      heatLevelCooling();

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWactivate.isON(), STATE_CYCLOTRON_ON_TO_FULL)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
        break;

      if (checkIfTrackDoneExit(STATE_CYCLOTRON_ON)) // Check if track is ended before going into STATE_CYCLOTRON_ON
        break;

      break;
    }
    }

    break;
  }

  //////////////////////////////////////////////
  case STATE_CYCLOTRON_ON_TO_OFF:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CYCLOTRON_ON_TO_OFF");

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      heatLevelCooling();

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isON(), STATE_CYCLOTRON_OFF_TO_ON)) // Check if Cyclotron Switch is ON, goes into cyclotron booting state STATE_CYCLOTRON_OFF_TO_ON
        break;

      if (checkIfTrackDoneExit(STATE_POWER_ON)) // Check if track is ended before going into STATE_POWER_ON witch cyclotron OFF
        break;

      break;
    }
    }

    break;
  }

  //////////////////////////////////////////////
  case STATE_CYCLOTRON_ON:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CYCLOTRON_ON");

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Wrist blaster state LEDs animations
      getLEDsSchemeForThisState();

      // Ensure command Delay is done before any other action
      if (!player.checkCommandDelay())
        break;

      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      heatLevelCooling();

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(getPartyModeState(), STATE_PARTY_MODE_IN)) // Check if the Intensify Switch is ON, goes into STATE_PARTY_MODE_IN
        break;

      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
        break;

      if (checkIfSwitchExit(SWactivate.isON(), STATE_CYCLOTRON_ON_TO_FULL)) // Check if Cyclotron Activate Switch is ON, goes into cyclotron booting FULL state STATE_CYCLOTRON_ON_TO_FULL
        break;

      // if (checkIfSwitchExit(PBfire.isON(), // Check if Firing Button pushed, goes into capture firing ramp state STATE_CAPTURE
      //                       (heatLevel < getCaptureScaledDuration())
      //                           ? STATE_CAPTURE
      //                           : STATE_CAPTURE_WARNING_OVERHEAT))
      //   break;

      if (checkIfSwitchExit(PBfire.isON(), STATE_CAPTURE))
        break;

      break;
    }
    }
    break;
  }

  //////////////////////////////////////////////
  case STATE_CYCLOTRON_ON_TO_FULL:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CYCLOTRON_ON_TO_FULL");

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :

    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      heatLevelCooling();

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWactivate.isOFF(), STATE_CYCLOTRON_FULL_TO_ON)) // Check if Cyclotron Activate Switch is OFF, goes into cyclotron returning to normal STATE_CYCLOTRON_FULL_TO_ON
        break;

      if (checkIfTrackDoneExit(STATE_CYCLOTRON_FULL_POWER)) // Check if track is ended before going into STATE_CYCLOTRON_FULL_POWER
        break;

      break;
    }
    }

    break;
  }

  //////////////////////////////////////////////
  case STATE_CYCLOTRON_FULL_TO_ON:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CYCLOTRON_FULL_TO_ON");

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :

    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      heatLevelCooling();

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWactivate.isON(), STATE_CYCLOTRON_ON_TO_FULL)) /// Check if Cyclotron Activate Switch is ON, goes into cyclotron booting FULL state STATE_CYCLOTRON_ON_TO_FULL
        break;

      if (checkIfTrackDoneExit(STATE_CYCLOTRON_ON)) // Check if track is ended before going into STATE_CYCLOTRON_ON
        break;

      break;
    }
    }
    break;
  }

  //////////////////////////////////////////////
  case STATE_CYCLOTRON_FULL_POWER:

  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CYCLOTRON_FULL_POWER");

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      heatLevelCooling();

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(getPartyModeState(), STATE_PARTY_MODE_IN)) // Check if the Intensify Switch is ON, goes into STATE_PARTY_MODE_IN
        break;

      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
        break;

      if (checkIfSwitchExit(SWactivate.isOFF(), STATE_CYCLOTRON_FULL_TO_ON)) // Check if Cyclotron Activate Switch is OFF, goes into cyclotron returning to normal STATE_CYCLOTRON_FULL_TO_ON
        break;

      if (checkIfSwitchExit(PBfire.isON(),
                            (heatLevel < (MAX_BURST_SHOTS - 1) * 100.0 / MAX_BURST_SHOTS)
                                ? STATE_BURST
                                : STATE_BURST_OVERHEAT))
        break;

      break;
    }
    }

    break;
  }

  //////////////////////////////////////////////
  case STATE_CAPTURE:

  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CAPTURE");

      //  Specifics initializations :
      fireType = CAPTURE; // tail and reboot to STATE_CYCLOTRON_ON

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      heatLevelRisingCapture();

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
        break;

      if (checkIfSwitchExit(SWactivate.isON(), STATE_CYCLOTRON_ON_TO_FULL)) // Check if Cyclotron Activate Switch is OFF, goes into cyclotron returning to normal STATE_CYCLOTRON_FULL_TO_ON
        break;

      // Wrist blaster going into tail before overheat warning:
      if (checkIfSwitchExit(PBfire.isOFF(), STATE_CAPTURE_TAIL))
        break;

      // Handle capture warning transition when heat level is getting high
      if (currentTime - stateStartTime >= DURATION_CAPTURE_TAKEOFF_RAMP &&
          heatLevel > getCaptureScaledDuration())
      {
        WBstate = STATE_CAPTURE_WARNING_OVERHEAT;
        stageFlag = 0;
        break;
      }

      break;
    }
    }
    break;
  }

    //////////////////////////////////////////////
  case STATE_CAPTURE_WARNING_OVERHEAT:

  {
    switch (stageFlag)
    {

    case 0: // WARNING initialisation :
    {
      DEBUG_PRINTLN("STATE_CAPTURE_WARNING_OVERHEAT");

      //  Specifics initializations :
      fireType = CAPTURE;   // tail and reboot to STATE_CYCLOTRON_ON
      smoker.smoke(ENABLE); // Put the smoke and pump on, but not the fan

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // WARNING :

    { // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      heatLevelRisingCapture();

      // Wrist blaster state exits by priority :

      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
        break;

      if (checkIfSwitchExit(SWactivate.isON(), STATE_CYCLOTRON_ON_TO_FULL)) // Check if Cyclotron Activate Switch is OFF, goes into cyclotron returning to normal STATE_CYCLOTRON_FULL_TO_ON
        break;

      // Wrist blaster going into tail before overheat warning:
      if (checkIfSwitchExit(PBfire.isOFF(), STATE_CAPTURE_TAIL))
      {
        smoker.smoke(DISABLE, DISABLE_FAN);
        break;
      }

      if (currentTime - stateStartTime >= getDuration() - DURATION_CAPTURE_OVERHEAT)
      {
        stageFlag = 2;
        break;
      }

      break;
    }

    case 2: // CAPTURE OVERHEAT initialisation
    {
      DEBUG_PRINTLN("STATE_CAPTURE_OVERHEAT");

      //  Specifics initializations :
      smoker.startBurst(DURATION_CAPTURE_OVERHEAT, WITH_FAN); // Put the smoke, pump and fan ON for the state duration

      // Standard initializers
      stateStartTime = currentTime;
      stageFlag = 3;

      DEBUG_PRINTLN();
      break;
    }

    case 3: // CAPTURE OVERHEAT
    {
      //  Specifics state mechanics :
      heatLevel = 0; // Venting and coooling done...

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
      {
        smoker.smoke(DISABLE, DISABLE_FAN);
        break;
      }

      // Determine next state
      BlasterState next = SWcyclotron.isOFF()
                              ? STATE_POWER_ON // Cyclotron switch is off, goes into POWER ON
                          : SWactivate.isON() ? STATE_CYCLOTRON_ON_TO_FULL
                                              : STATE_CYCLOTRON_OFF_TO_ON; // venting is done, reboot to cyclotron ON or full power

      // Check if overheating tail track is done
      if (checkIfTrackDoneExit(next))
      {
        smoker.smoke(DISABLE, DISABLE_FAN);
        break;
      }
      break;
    }
    }

    break;
  }

    //////////////////////////////////////////////
  case STATE_CAPTURE_TAIL:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CAPTURE_TAIL");

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      //  Specifics state mechanics :
      heatLevelCooling();

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      // if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
      //   break;

      // Check if tail track is done
      if (!player.isPlaying()) // || (currentTime - stateStartTime) >= TRACK_LENGTH[WBstate])
      {
        // If tailing is done goes into POWER OFF, or into cyclotron ON or FULL depending of the fireing type
        WBstate = fireType ? STATE_CYCLOTRON_FULL_POWER : STATE_CYCLOTRON_ON;
        stageFlag = 0;
        break;
      }

      // Check if Firing Button pushed, goes into capture firing ramp state STATE_CAPTURE
      if (checkIfSwitchExit(PBfire.isON(),
                            (heatLevel < getCaptureScaledDuration())
                                ? STATE_CAPTURE
                                : STATE_CAPTURE_WARNING_OVERHEAT))
        break;

      break;
    }
    }
    break;
  }

    //////////////////////////////////////////////
  case STATE_BURST:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_BURST");

      //  Specifics initializations :
      fireType = BURST; // tail and reboot to STATE_CYCLOTRON_FULL_POWER

      //  Specifics state mechanics :
      heatLevelRisingBurst(); // increment heatLevel for this shot

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {       // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
        break;

      if (checkIfSwitchExit(SWactivate.isOFF(), STATE_CYCLOTRON_FULL_TO_ON)) // Check if Cyclotron Activate Switch is OFF, goes into cyclotron returning to normal STATE_CYCLOTRON_FULL_TO_ON
        break;

      if (currentTime - stateStartTime >= getDuration() - DURATION_BURST_TAIL)
      {
        stageFlag = 2;
        break;
      }

      break;
    }

    case 2: // Initiate TAIL :
    {
      DEBUG_PRINTLN("STATE_BURST_TAIL");

      // Standard initializers
      // None continue the burst track but with tail lights effects

      stageFlag = 3;

      DEBUG_PRINTLN();
      break;
    }

    case 3: // TAIL :
    {
      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      // Check if tail track is done
      if (!player.isPlaying())
      {
        // If tailing is done goes into cyclotron ON or FULL depending of the fireing type
        WBstate = fireType ? STATE_CYCLOTRON_FULL_POWER : STATE_CYCLOTRON_ON;
        stageFlag = 0;
        break;
      }

      // Check if Firing Button pushed, goes into capture firing ramp state STATE_BURST
      if (PBfire.isON())
      {
        WBstate = (heatLevel < (MAX_BURST_SHOTS - 1) * 100.0 / MAX_BURST_SHOTS) ? STATE_BURST : STATE_BURST_OVERHEAT;
        stageFlag = 0;
        break;
      }

      break;
    }
    }
    break;
  }

    //////////////////////////////////////////////
  case STATE_BURST_OVERHEAT:
  {
    switch (stageFlag)
    {

    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_BURST_WARNING");

      //  Specifics initializations :
      fireType = BURST; // tail and reboot to STATE_CYCLOTRON_FULL_POWER

      //  Specifics state mechanics :
      heatLevelRisingBurst(); // increment heatLevel for this shot
      smoker.smoke(ENABLE);

      // Standard initializers
      stageFlag = stateInitialization(); // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      // Enable/disable the track looping play mode if required, if updated break the loop to check command delay again
      // Must be called after the play command in the initialization stage 0...
      if (checkPlayModeForThisState())
        break;

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
        break;

      if (checkIfSwitchExit(SWactivate.isOFF(), STATE_CYCLOTRON_FULL_TO_ON)) // Check if Cyclotron Activate Switch is OFF, goes into cyclotron returning to normal STATE_CYCLOTRON_FULL_TO_ON
        break;

      if (currentTime - stateStartTime >= getDuration() - DURATION_BURST_OVERHEAT)
      {
        stageFlag = 2;
        break;
      }

      break;
    }

    case 2: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_BURST_OVERHEAT");

      smoker.smoke(ENABLE, ENABLE_FAN);

      // Standard initializers
      stageFlag = 3;

      DEBUG_PRINTLN();
      break;
    }

    case 3: // This wrist blaster state loop :
    {
      //  Specifics state mechanics :
      heatLevel = 0; // Venting and coooling done...

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
      {
        smoker.smoke(DISABLE, DISABLE_FAN);
        break;
      }

      // Determine next state
      BlasterState next = SWcyclotron.isOFF()
                              ? STATE_POWER_ON // Cyclotron switch is off, goes into POWER ON
                          : SWactivate.isON() ? STATE_CYCLOTRON_ON_TO_FULL
                                              : STATE_CYCLOTRON_OFF_TO_ON; // venting is done, reboot to cyclotron ON or full power

      // Check if overheating tail track is done
      if (checkIfTrackDoneExit(next))
      {
        smoker.smoke(DISABLE, DISABLE_FAN);
        break;
      }

      break;
    }
    }
    break;
  }
  }

  // END_SEQ Actions for different blaster states
  ///////////////////////////////////////////////////////////////
}
/********************** END_SEQ MAIN LOOP *******************/

//////////////////////////////////////////////////////////////////////////
///////////////////// *** HELPER FUNCTIONS *** ///////////////////////////
//////////////////////////////////////////////////////////////////////////

/*******************************************/
/*   ANIMATIONS functions in wrist blaster states   */
/*******************************************/
void getLEDsSchemeForThisState()
{
  // Some sequences need to be initialized with wrist blaster state, stageFlag is used to know if wrist blaster state is in initialization

  switch (WBstate)
  {
  case STATE_ZERO:
    break;

  case STATE_PARTY_MODE:

  {
    switch (stageFlag)
    {
    case 0:
    {
      /* Initialize animations/schemes : */
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.clear();
      /*fireButtonSingleLed */ fireButtonSingleLed.on();
      /*vent                */ vent.clear();
      /*barmeter            */ barmeter.partyModeInit();
      /*cyclotron           */ cyclotron.rampInit(CYC_ON, 500); // Finish party mode in sequence if not done
      /*firingRod           */ firingRod.clear();
      break;
    }
    case 1:
    { /*slowBlowIndicator   */
      slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.flash(517); // 116 bpm : 60 000 / 116 = 517ms
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ // Cleared
      /*fireButtonSingleLed */ // ON
      /*vent                */ // Cleared
      /*barmeter            */ barmeter.partyMode();
      /*cyclotron           */ cyclotron.ramp(); // Finish party mode in sequence if not done
      /*firingRod           */                   // Cleared
      break;
    }
    }
    break;
  }

  case STATE_PARTY_MODE_IN:
  {
    switch (stageFlag)
    {
    case 0:
    { // Initialize animations/schemes :
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, FAST_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.clear();
      /*fireButtonSingleLed */ fireButtonSingleLed.blinkInit(MEDIUM_BLINK_SP);
      /*vent                */ vent.clear();
      /*barmeter            */ barmeter.fillUpEmptyDownOnceInit(getDuration());
      /*cyclotron           */ cyclotron.rampInit(CYC_ON, getDuration());
      /*firingRod           */ firingRod.clear();
      break;
    }
    case 1:
    { /*slowBlowIndicator   */
      slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ // Cleared
      /*fireButtonSingleLed */ fireButtonSingleLed.blink();
      /*vent                */ // Cleared
      /*barmeter            */ barmeter.fillUpEmptyDownOnce();
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ // Cleared
      break;
    }
    }
    break;
  }

  case STATE_PARTY_MODE_OUT:
  {
    switch (stageFlag)
    {
    case 0:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, FAST_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.clear();
      /*fireButtonSingleLed */ fireButtonSingleLed.blinkInit(MEDIUM_BLINK_SP);
      /*vent                */ vent.clear();
      /*barmeter            */ barmeter.fillUpFastEmptyDownSlowOnceInit(getDuration(), ENABLE);
      /*cyclotron           */ cyclotron.rampInit(CYC_OFF, getDuration());
      /*firingRod           */ firingRod.clear();
      break;
    }
    case 1:
    { /*slowBlowIndicator   */
      slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ // Cleared
      /*fireButtonSingleLed */ fireButtonSingleLed.blink();
      /*vent                */ // Cleared
      /*barmeter            */ barmeter.fillUpFastEmptyDownSlowOnce();
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ // Cleared
      break;
    }
    }
    break;
  }

  case STATE_POWER_OFF:
  {
    switch (stageFlag)
    {
    case 0:
    {
      clearAllLights(); // clear cyclotron, bar meter, vent, firing jewel and cyclotron LEDS
      topWhiteIndicator.initParam(smoker.enable() ? GREEN : RED, 50);
      break;
    }
    case 1:
    {
      if (PBfire.isOFF())
        topWhiteIndicator.flash(5000); // set LED_INDEX_TOP_WHITE led green flashing

      break;
    }
    }
    break;
  }

  case STATE_POWER_OFF_TO_ON:
  {
    switch (stageFlag)
    {
    case 0:
    {
      topWhiteIndicator.clear();
      slowBlowIndicator.initParam(RED, 100, FAST_BLINK_SP);
      break;
    }
    case 1:
    {
      slowBlowIndicator.blink(DISABLE);
      break;
    }
    }
    break;
  }

  case STATE_POWER_ON_TO_OFF:
  {
    switch (stageFlag)
    {
    case 0:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(BLACK, 100, SOLID, getDuration());
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 0, SOLID, getDuration());
      /*topYellowIndicator  */ topYellowIndicator.initParam(BLACK, 100, SOLID, getDuration());
      /*frontOrangeIndicator*/ frontOrangeIndicator.clear();
      /*fireButtonSingleLed */ fireButtonSingleLed.clear();
      /*vent                */ vent.initParam(WARM_WHITE, 0, 2000);
      /*barmeter            */ barmeter.clear();
      /*cyclotron           */ cyclotron.clear();
      /*firingRod           */ firingRod.clear();
      break;
    }
    case 1:
    { /*slowBlowIndicator   */
      slowBlowIndicator.ramp();
      /*topWhiteIndicator   */ topWhiteIndicator.ramp();
      /*topYellowIndicator  */ topYellowIndicator.ramp();
      /*frontOrangeIndicator*/ // Cleared
      /*fireButtonSingleLed */ // Cleared
      /*vent                */ vent.ramp();
      /*barmeter            */ // Cleared
      /*cyclotron           */ // Cleared
      /*firingRod           */ // Cleared
      break;
    }
    }
    break;
  }

  case STATE_POWER_ON:
  {
    switch (stageFlag)
    {
    case 0:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, SLOW_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 255);
      /*frontOrangeIndicator*/ frontOrangeIndicator.clear();
      /*fireButtonSingleLed */ fireButtonSingleLed.clear();
      /*vent                */ vent.clear();
      /*barmeter            */ barmeter.clear();
      /*cyclotron           */ cyclotron.clear();
      /*firingRod           */ firingRod.clear();
      break;
    }
    case 1:
    { /*slowBlowIndicator   */
      slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ // Cleared
      /*fireButtonSingleLed */ // Cleared
      /*vent                */ // Cleared
      /*barmeter            */ // Cleared
      /*cyclotron           */ // Cleared
      /*firingRod           */ // Cleared
      break;
    }
    }
    break;
  }

  case STATE_CYCLOTRON_OFF_TO_ON:
  {
    switch (stageFlag)
    {
    case 0:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, MEDIUM_BLINK_SP, getDuration());
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 100, MEDIUM_BLINK_SP, topWhiteIndicator.getPrevBlink(), topWhiteIndicator.getPulse());
      /*fireButtonSingleLed */ fireButtonSingleLed.blinkInit(FAST_BLINK_SP);
      /*vent                */ vent.initParam(WARM_WHITE, 50, getDuration());
      /*barmeter            */ barmeter.fillDownEmptyDownOnceInit(getDuration(), ENABLE);
      /*cyclotron           */ cyclotron.rampInit(CYC_ON, getDuration());
      /*firingRod           */ firingRod.clear(); // Just to finish the shootout fade if not finished
      break;
    }
    case 1:
    { /*slowBlowIndicator   */
      slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(ENABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ frontOrangeIndicator.blink(DISABLE_RAMP);
      /*fireButtonSingleLed */ fireButtonSingleLed.blink();
      /*vent                */ vent.ramp(); // White, not at full brightness
      /*barmeter            */ barmeter.fillDownEmptyDownOnce();
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ // cleared

      break;
    }
    }
    break;
  }

  case STATE_CYCLOTRON_ON_TO_OFF:
  {
    switch (stageFlag)
    {
    case 0:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, SLOW_BLINK_SP, getDuration());
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 0, SOLID, getDuration());
      /*fireButtonSingleLed */ fireButtonSingleLed.blinkInit(FAST_BLINK_SP);
      /*vent                */ vent.initParam(WARM_WHITE, 0, getDuration());
      /*barmeter            */ barmeter.fillUpFastEmptyDownSlowOnceInit(getDuration(), ENABLE);
      /*cyclotron           */ cyclotron.rampInit(CYC_OFF, getDuration());
      /*firingRod           */ firingRod.clear();
      break;
    }
    case 1:
    { /*slowBlowIndicator   */
      slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(ENABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ frontOrangeIndicator.ramp();
      /*fireButtonSingleLed */ fireButtonSingleLed.blink();
      /*vent                */ vent.ramp();
      /*barmeter            */ barmeter.fillUpFastEmptyDownSlowOnce();
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ // cleared
      break;
    }
    }
    break;
  }

  case STATE_CYCLOTRON_ON:
  {
    switch (stageFlag)
    {
    case 0:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, MEDIUM_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 100);
      /*fireButtonSingleLed */ fireButtonSingleLed.on();
      /*vent                */ vent.initParam(WARM_WHITE, 50); // finishing fade if not done
      /*barmeter            */ barmeter.cyclotronIdleInit(heatLevel);
      /*cyclotron           */ cyclotron.rampInit(CYC_ON, 500); // Finishing ramping if not done
      /*firingRod           */ firingRod.clear();
      break;
    }
    case 1:
    { /*slowBlowIndicator   */
      slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ frontOrangeIndicator.solid();
      /*fireButtonSingleLed */               // ON, no update required
      /*vent                */ vent.solid(); // finishing fade if not done
      /*barmeter            */ barmeter.cyclotronIdle(heatLevel);
      /*cyclotron           */ cyclotron.ramp(); // Finishing ramping if not done
      /*firingRod           */                   // cleared
      break;
    }
    }
    break;
  }

  case STATE_CYCLOTRON_ON_TO_FULL:
  {
    switch (stageFlag)
    {
    case 0:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, FAST_BLINK_SP, getDuration());
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 100);
      /*fireButtonSingleLed */ fireButtonSingleLed.blinkInit(FAST_BLINK_SP);
      /*vent                */ vent.initParam(WARM_WHITE, 75, getDuration());              // finishing fade if not done
      /*barmeter            */ barmeter.fillDownEmptyDownOnceInit(getDuration(), DISABLE); // full bar from top and empty it from top
      /*cyclotron           */ cyclotron.rampInit(CYC_FULL, getDuration());
      /*firingRod           */ firingRod.clear();
      break;
    }
    case 1:
    { /*slowBlowIndicator   */
      slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(ENABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ frontOrangeIndicator.solid();
      /*fireButtonSingleLed */ fireButtonSingleLed.blink();
      /*vent                */ vent.ramp();                      // finishing fade if not done
      /*barmeter            */ barmeter.fillDownEmptyDownOnce(); // full bar from top and empty it from top
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ // cleared
      break;
    }
    }
    break;
  }

  case STATE_CYCLOTRON_FULL_TO_ON:
  {
    switch (stageFlag)
    {
    case 0:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, MEDIUM_BLINK_SP, getDuration());
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 100);
      /*fireButtonSingleLed */ fireButtonSingleLed.blinkInit(FAST_BLINK_SP);
      /*vent                */ vent.initParam(WARM_WHITE, 50, getDuration());                    // finishing fade if not done
      /*barmeter            */ barmeter.fillUpFastEmptyDownSlowOnceInit(getDuration(), DISABLE); // full bar and slow emptying from top to bottom
      /*cyclotron           */ cyclotron.rampInit(CYC_ON, getDuration());
      /*firingRod           */ firingRod.clear();
      break;
    }
    case 1:
    { /*slowBlowIndicator   */
      slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(ENABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ frontOrangeIndicator.solid();
      /*fireButtonSingleLed */ fireButtonSingleLed.blink();
      /*vent                */ vent.ramp();                            // finishing fade if not done
      /*barmeter            */ barmeter.fillUpFastEmptyDownSlowOnce(); // full bar and slow emptying from top to bottom
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ // cleared
      break;
    }
    }
    break;
  }

  case STATE_CYCLOTRON_FULL_POWER:
  {
    switch (stageFlag)
    {
    case 0:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, FAST_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 100);
      /*fireButtonSingleLed */ fireButtonSingleLed.on();
      /*vent                */ vent.initParam(WARM_WHITE, 75); // finishing fade if not done
      /*barmeter            */ barmeter.cyclotronIdleFullInit(heatLevel);
      /*cyclotron           */ cyclotron.rampInit(CYC_FULL, 500); // Finishing ramping if not done
      /*firingRod           */ firingRod.clear();
      break;
    }
    case 1:
    { /*slowBlowIndicator   */
      slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ frontOrangeIndicator.solid();
      /*fireButtonSingleLed */               // ON, no update required
      /*vent                */ vent.solid(); // finishing fade if not done
      /*barmeter            */ barmeter.cyclotronIdleFull(heatLevel);
      /*cyclotron           */ cyclotron.ramp(); // Finishing ramping if not done
      /*firingRod           */                   // Cleared

      break;
    }
    }
    break;
  }

  case STATE_CAPTURE:
  {
    switch (stageFlag)
    {
    case 0:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, MEDIUM_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 100);
      /*fireButtonSingleLed */ fireButtonSingleLed.off();
      /*vent                */ vent.initParam(WARM_WHITE, 50);
      /*barmeter            */ barmeter.fireInit(CAPTURE);
      /*cyclotron           */ cyclotron.rampInit(CYC_CAPTURE_MAX, getDuration());
      /*firingRod           */ firingRod.strobeInit(SHUFFLE, 100, 300);
      break;
    }
    case 1:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ frontOrangeIndicator.solid();
      /*fireButtonSingleLed */ // OFF
      /*vent                */ vent.flicker(50, 25);
      /*barmeter            */ barmeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();
      break;
    }
    }
    break;
  }

  case STATE_CAPTURE_WARNING_OVERHEAT:
  {
    switch (stageFlag)
    {
    case 0:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, MEDIUM_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100, FAST_BLINK_SP, topWhiteIndicator.getPrevBlink(), topWhiteIndicator.getPulse());
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 100);
      /*fireButtonSingleLed */ fireButtonSingleLed.off();
      /*vent                */ vent.initParam(WARM_WHITE, 50);
      /*barmeter            */
      if (prevState == STATE_CYCLOTRON_ON) // Only init if it's direct warning without going trough STATE_CAPTURE
      {
        barmeter.fireInit(CAPTURE);
      }
      /*cyclotron           */ cyclotron.rampInit(CYC_CAPTURE_WARNING, getDuration() - DURATION_CAPTURE_OVERHEAT);
      /*firingRod           */ firingRod.strobeInit(SHUFFLE, 100, 300);
      break;
    }
    case 1:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.blink(DISABLE_RAMP);
      /*frontOrangeIndicator*/ frontOrangeIndicator.solid();
      /*fireButtonSingleLed */ // OFF
      /*vent                */ vent.flicker(50, 25);
      /*barmeter            */ barmeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();
      break;
    }
    case 2:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, MEDIUM_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100, FAST_BLINK_SP);
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 0, min(3000, DURATION_CAPTURE_OVERHEAT - 3000));
      /*fireButtonSingleLed */ fireButtonSingleLed.blinkInit(FAST_BLINK_SP);
      /*vent                */ vent.initParam(WARM_WHITE, 0, min(3000, DURATION_CAPTURE_OVERHEAT - 3000));
      /*barmeter            */ barmeter.fireInit(BURST, END_SEQ); // reverse end finish fire animation
      /*cyclotron           */ cyclotron.rampInit(CYC_OFF, min(3000, DURATION_CAPTURE_OVERHEAT - 3000));
      /*firingRod           */ firingRod.strobeInit(NO_SHUFFLE, 0, 2000);
      break;
    }
    case 3:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.blink(DISABLE_RAMP);
      /*frontOrangeIndicator*/ frontOrangeIndicator.ramp();
      /*fireButtonSingleLed */ fireButtonSingleLed.blink();
      /*vent                */ vent.ramp();
      /*barmeter            */ barmeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();
      break;
    }
    }
    break;
  }

  case STATE_CAPTURE_TAIL:
  {
    switch (stageFlag)
    {
    case 0:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, MEDIUM_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 100);
      /*fireButtonSingleLed */ fireButtonSingleLed.blinkInit(FAST_BLINK_SP);
      /*vent                */ vent.initParam(WARM_WHITE, 50, 300);
      /*barmeter            */ barmeter.fireInit(BURST, END_SEQ); // reverse end finish fire animation
      /*cyclotron           */ cyclotron.rampInit(CYC_FULL, getDuration());
      /*firingRod           */ firingRod.strobeInit(NO_SHUFFLE, 0, 1000);
      break;
    }
    case 1:
    {
      /*slowBlowIndicator   */ slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ frontOrangeIndicator.solid();
      /*fireButtonSingleLed */ fireButtonSingleLed.blink();
      /*vent                */ vent.ramp();
      /*barmeter            */ barmeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();
      break;
    }
    }
    break;
  }

  case STATE_BURST:
  {
    switch (stageFlag)
    {
    case 0: // Burst initialisation
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, FAST_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 100);
      /*fireButtonSingleLed */ fireButtonSingleLed.off();
      /*vent                */ vent.initParam(WARM_WHITE, 75);
      /*barmeter            */ barmeter.fireInit(BURST);
      /*cyclotron           */ cyclotron.rampInit(CYC_BURST_MAX, getDuration() - DURATION_BURST_TAIL); //
      /*firingRod           */ firingRod.strobeInit(SHUFFLE, 100, 300);
      break;
    }
    case 1: // BURST
    {
      /*slowBlowIndicator   */ slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ frontOrangeIndicator.solid();
      /*fireButtonSingleLed */ // OFF
      /*vent                */ vent.flicker(50, 25);
      /*barmeter            */ barmeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();

      break;
    }
    case 2: // TAIL initialisation
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, FAST_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100);
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 100);
      /*fireButtonSingleLed */ fireButtonSingleLed.blinkInit(FAST_BLINK_SP);
      /*vent                */ vent.initParam(WARM_WHITE, 75, 300);
      /*barmeter            */ barmeter.fireInit(CAPTURE, END_SEQ); // No init, reverse and finish the burst sequence...
      /*cyclotron           */ cyclotron.rampInit(CYC_CAPTURE_MAX, DURATION_BURST_TAIL);
      /*firingRod           */ firingRod.strobeInit(NO_SHUFFLE, 0, 1000);
      break;
    }
    case 3: // TAIL
    {
      /*slowBlowIndicator   */ slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/ frontOrangeIndicator.solid();
      /*fireButtonSingleLed */ fireButtonSingleLed.blink();
      /*vent                */ vent.ramp();
      /*barmeter            */ barmeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();

      break;
    }
    }
    break;
  }

  case STATE_BURST_OVERHEAT:
  {
    switch (stageFlag)
    {
    case 0: // Burst with warning initialisation
    {
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, FAST_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100, FAST_BLINK_SP, topWhiteIndicator.getPrevBlink(), topWhiteIndicator.getPulse());
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 100);
      /*fireButtonSingleLed */ fireButtonSingleLed.off();
      /*vent                */ vent.initParam(WARM_WHITE, 75);
      /*barmeter            */ barmeter.fireInit(BURST);
      /*cyclotron           */ cyclotron.rampInit(CYC_BURST_WARNING, getDuration() - DURATION_BURST_OVERHEAT);
      /*firingRod           */ firingRod.strobeInit(SHUFFLE, 100, 300);
      break;
    }
    case 1: // BURST with warning
    {
      /*slowBlowIndicator   */ slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.blink(DISABLE);
      /*frontOrangeIndicator*/ frontOrangeIndicator.solid();
      /*fireButtonSingleLed */ // OFF
      /*vent                */ vent.flicker(50, 25);
      /*barmeter            */ barmeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();
      break;
    }
    case 2: // OVERHEAT initialisation
    {
      uint16_t duration = min(3000, DURATION_BURST_OVERHEAT - 3000);
      /*slowBlowIndicator   */ slowBlowIndicator.initParam(RED, 100);
      /*topWhiteIndicator   */ topWhiteIndicator.initParam(WHITE, 75, FAST_BLINK_SP);
      /*topYellowIndicator  */ topYellowIndicator.initParam(YELLOW, 100, FAST_BLINK_SP);
      /*frontOrangeIndicator*/ frontOrangeIndicator.initParam(ORANGE, 0, duration);
      /*fireButtonSingleLed */ fireButtonSingleLed.blinkInit(FAST_BLINK_SP);
      /*vent                */ vent.initParam(WARM_WHITE, 0, duration);
      /*barmeter            */ barmeter.fireInit(CAPTURE, END_SEQ); // No init, reverse and finish the burst sequence...
      /*cyclotron           */ cyclotron.rampInit(CYC_OFF, duration);
      /*firingRod           */ firingRod.strobeInit(NO_SHUFFLE, 0, 2000);
      break;
    }
    case 3: // OVERHEAT
    {
      /*slowBlowIndicator   */ slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE_RAMP);
      /*topYellowIndicator  */ topYellowIndicator.blink(DISABLE_RAMP);
      /*frontOrangeIndicator*/ frontOrangeIndicator.ramp();
      /*fireButtonSingleLed */ fireButtonSingleLed.blink();
      /*vent                */ vent.ramp();
      /*barmeter            */ barmeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();
      break;
    }
    }
    break;
  }
  }
}

//  END_SEQ of animations functions in wrist blaster states
////////////////////////////////////////////////

void clearAllLights()
{
  // Clear leds
  cyclotron.clear();
  barmeter.clear();
  vent.clear();
  slowBlowIndicator.clear();
  topWhiteIndicator.clear();
  topYellowIndicator.clear();
  frontOrangeIndicator.clear();
  fireButtonSingleLed.clear();
  firingRod.clear();

  // Reset trackers
}

bool checkIfTrackDoneExit(BlasterState next_state)
{
  if (/*playingTrack >= 0 &&*/ !player.isPlaying())
  {
    // Track done or time elapsed, change state
    WBstate = next_state;
    stageFlag = 0;
    return true;
  }
  return false;
}

bool checkIfSwitchExit(bool switch_state, BlasterState next_state)
{
  if (switch_state)
  {
    // Switch is triggered and delay has passed, change state
    WBstate = next_state;
    stageFlag = 0;
    return true;
  }
  return false;
}

bool checkIfTimerExit(uint16_t time, BlasterState next_state)
{
  if (currentTime - stateStartTime < time)
    return false;

  // Timer expired → Change state
  WBstate = next_state;
  stageFlag = 0;
  return true;
}

uint8_t stateInitialization() // Standard initializers for most state
{
  playThisStateTrack();
  stateStartTime = currentTime;
  return 1; // End state initialization when stageFlag is 1
}

void playThisStateTrack()
{
  if (TRACK_LENGTH[WBstate] == 0)
    player.stop();
  else
    TRACK_LOOPING[WBstate] ? player.loopFileNum(WBstate)
                           : player.playFileNum(WBstate, TRACK_LENGTH[WBstate]);

  playingTrack = WBstate;

  DEBUG_PRINTLN("Track: " + String(WBstate) + "  length: " +
                String(TRACK_LENGTH[WBstate]) +
                " Loop required: " + String(TRACK_LOOPING[WBstate]));
}

void playThisTrack(uint8_t track)
{

  TRACK_LOOPING[track] ? player.loopFileNum(track)
                       : player.playFileNum(track, TRACK_LENGTH[track]);

  playingTrack = track;

  DEBUG_PRINTLN("Track: " + String(track) + "  length: " +
                String(TRACK_LENGTH[track]) +
                " Loop required: " + String(TRACK_LOOPING[track]));
}

bool checkPlayModeForThisState()
{
  bool shouldLoop = TRACK_LOOPING[WBstate];

  // Set the proper looping mode for this state
  if (shouldLoop != cycling)
  {
    shouldLoop ? player.setCyclingTrackPlaymode() : player.setSinglePlaymode();
    cycling = shouldLoop;
    DEBUG_PRINTLN(" Set looping to:" + String(cycling));

    return true;
  }
  return false;
}

uint16_t getDuration() // Get track duration for the actual state
{
  uint8_t buffer = 0;
  return (TRACK_LENGTH[WBstate] - AUDIO_ADVANCE - buffer);
}

uint16_t getSpecificDuration(BlasterState state)
{ // Get track duration for a specific state
  uint8_t buffer = 0;
  return (TRACK_LENGTH[state] - AUDIO_ADVANCE - buffer);
}

void checkNextPreviousButton()
{
  static uint32_t pbfirePrev = 0;
  if (PBfire.toggleON())
  {
    pbfirePrev = currentTime;
    return;
  }

  if ((PBfire.toggleOFF()))
  {
    uint32_t pressDuration = currentTime - pbfirePrev;

    if (pressDuration < 1000)
      player.next(); // Short press → Next track
    else
      player.previous(); // Long press → Previous track
  }
}

void heatLevelCooling()
{
  if (heatLevel == 0)
    return;

  if (currentTime - heatLevelPrevUpdate > 125)
  // Increase heat level every 250ms, about 25secondes before going into overheat.
  {
    heatLevelPrevUpdate = currentTime;

    if (heatLevel > 0) // Ensure that heatLevel doesn't overflow as unsigned...
      heatLevel -= 1;
  }
}

void heatLevelRisingCapture() // Increase heat level every 250ms, about 25secondes before going into overheat.
{                             // HeatLevel 0 -100 %, at 100%, wrist blaster goes into overheat...
  if (currentTime - heatLevelPrevUpdate < (DURATION_CAPTURE_MAX / 100))
    return;

  heatLevelPrevUpdate = currentTime;

  heatLevel += 1;
  heatLevel = min(100, heatLevel);
  return;
}

void heatLevelRisingBurst() // Increase heat level every 250ms, about 25secondes before going into overheat.
{                           // HeatLevel 0 -100 %, at 100%, wrist blaster goes into overheat...
  heatLevelPrevUpdate = currentTime;

  heatLevel += (uint16_t)round(100.0 / MAX_BURST_SHOTS);
  heatLevel = min(100, heatLevel);
  return;
}

uint8_t getCaptureScaledDuration()
{
  uint16_t maxDuration = constrain(DURATION_CAPTURE_MAX, 10000, TRACK_LENGTH[STATE_CAPTURE]);
  uint16_t warningDuration = TRACK_LENGTH[STATE_CAPTURE_WARNING_OVERHEAT] - DURATION_CAPTURE_OVERHEAT;
  return round(100.0 * (maxDuration - warningDuration) / maxDuration);
}

void checkSmokerEnabling()
{
  static uint32_t pushedDetected = 0;
  static bool flag = false;

  if (PBfire.toggleON())
  {
    pushedDetected = currentTime;
    topWhiteIndicator.solid();
  }

  if (PBfire.isON() &&
      currentTime - pushedDetected >= 3000 &&
      !flag)
  {
    // Get new state
    bool newState = !smoker.enable();

    // Set new state
    smoker.setEnable(newState);
    flag = true;

    // Show new state with indicator
    topWhiteIndicator.initParam(newState ? GREEN : RED, 50);
    // topWhiteIndicator.solid();
  }

  // Return to regular leds scheme for this state
  if (PBfire.toggleOFF())
  {
    flag = false;
    topWhiteIndicator.initParam(smoker.enable() ? GREEN : RED, 50);
    // topWhiteIndicator.blink(DISABLE);
  }
}

bool getPartyModeState()
{
  static bool partyModeState = false;

  // Output is like a switch
  if (INTENSIFY_IS_A_SWITCH)
    partyModeState = PBintensify.isON();

  // Toggle the state when the button is pushed and release
  else if (PBintensify.toggleOFF())
    partyModeState = partyModeState ? false : true;

  return partyModeState;
}