/**
 * @file        SBK_WRISTBLASTER_CORE.ino
 * @brief       Lights and sound control code for the SBK Wrist Blaster replica or other props.
 *
 * @author      Samuel Barabé
 * @copyright   Copyright (c) 2025-2026 Samuel Barabé
 * @license     MIT License
 * @version     1.1.0
 * @link        https://github.com/sbarabe/SBK_WRISTBLASTER
 *
 * @details
 * This software provides core functionality for animating lights and playing sound effects on
 * a wrist-mounted prop weapon. Designed with customization and extensibility in mind.
 *
 * Special thanks to @David Miyakawa for animations, sound effects, and workflow design.
 *
 * @see         https://opensource.org/licenses/MIT
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ---
 * CORE FILE NOTICE:
 * This file contains the main Wrist Blaster logic. Most users should configure the project through
 * SBK_WRISTBLASTER_CONFIG.h rather than modifying this file directly, unless you are an advanced
 * user intending to customize the workflow or animation effects.
 *
 * For general setup, use the SBK_WRISTBLASTER_CONFIG.h file to configure:
 * - Pin assignments
 * - LED indexes
 * - Track lengths
 * - Feature toggles
 *
 * Modify only the config file to match your specific hardware and desired behavior.
 * This keeps the core logic stable, clean, and easier to maintain.
 *
 * For setup instructions and documentation, visit:
 * https://github.com/sbarabe/SBK_WRISTBLASTER
 *
 * The core is modular and object-oriented, with components organized in the
 * `SBK_WristBlaster_lib` folder.
 *
 * Supported features:
 * - DFPlayer Mini sound board
 * - LED bar meter (28 segments) via MAX72xx or HT16K33 drivers
 * - WS2812 LED animations (vent, cyclotron, indicators)
 * - Configurable buttons/switches for boot, fire, mode, and smoke
 * - Firing rod hue potentiometer input
 * - Party mode, burst fire mode, and smoke toggle
 *
 * Important Notes:
 * - Audio timing is based on predefined track durations (not BUSY pin).
 * - Incorrect durations may cause animation/audio desync.
 * - Advanced users may customize animations or state transitions in the core engine.
 *
 * If you find this useful, please consider supporting development at:
 * https://github.com/sbarabe/SBK_WRISTBLASTER
 *
 * @see https://opensource.org/licenses/MIT
 * @see https://github.com/PowerBroker2/DFPlayerMini_Fast
 * @see https://github.com/adafruit/Adafruit_NeoPixel
 **/

#include <Arduino.h>
#include <Wire.h> // Include the I2C library (required)
#include "SBK_WRISTBLASTER_CONFIG.h"

/*********************************************/
/*      GENERAL definitions and helpers      */
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
uint8_t battLevel = 100;                // Battery level variable for battery power monitoring option
uint32_t prevBattReading = 0;
// Helpers functions declarations, functions are defined after the main loop :
uint8_t stateInitialization();                                      // Standard initialisation sequence for most state
void clearAllLights();                                              // SHUTOFF all leds for wrist blaster and resets some trackers
bool checkIfTrackDoneExit(BlasterState next_state);                 // check if a state track is done playing and go to next stage
bool checkIfSwitchExit(bool switch_state, BlasterState next_state); // check if a switch action and go to next stage
bool checkIfTimerExit(uint16_t time, BlasterState next_state);      // check if a timer is done and go to next stage
void getLEDsSchemeForThisState();                                   // This function contains animations settings and calling for all states
void getPowercellLEDsSchemeForThisState();                          // This function contains optional animation for POWERCELL if enabled
void playThisStateTrack();                                          // play state track
void playThisTrack(uint8_t track);
;                                                 // Play specific track other then state track
bool checkPlayModeForThisState();                 // check if play mode is correct for this state (looping / not looping)
uint16_t getDuration();                           // Get actual state duration
uint16_t getSpecificDuration(BlasterState state); // Get duration of a specific state
void checkNextPreviousButton();                   // While in party mode, switch to previous/next song with fire button
void heatLevelCooling();                          // Manage heat level
void heatLevelRisingBurst();                      // Manage heat level
void heatLevelRisingCapture();                    // Manage heat level
#ifdef SMOKE_FEATURES_ENABLED
void checkSmokerEnabling();
#endif                              // Check fire button operation to enable/disable smoke effect
uint8_t getCaptureScaledDuration(); // Scale Capture state duration to be compatible with heat level management
bool getPartyModeState();           // Helper to manage PBIntensify output as a switch or a push button

/*********************************************/
/*           BAR METER & DRIVER(s)           */
/*********************************************/
//  BAR METER with shift registers
//  Bar meter helper variables for 28 segments bar meter:
//  DRIVER type, animations DIRECTION and segments MAPPING should be defined in SBK_WRISTBLASTER_CONFIG.h file
#ifdef PBM_MAX72XX
#include "SBK_WB_MAX72xx_V1_1_0.h"

#if defined(PCBM_MAX72XX_ON_PBM_CHAIN)
#define PBM_MAX72XX_DEVICE_COUNT 2
#else
#define PBM_MAX72XX_DEVICE_COUNT 1
#endif

MAX72xx PBM_driver(PBM_DIN_PIN, PBM_CLK_PIN, PBM_LOAD_PIN, PBM_MAX72XX_DEVICE_COUNT);
PanelBarMeter panelBarMeter(&PBM_SEG_NUMBER, &PBM_DIRECTION, &PBM_driver, PBM_DRIVER_ADDRESS, PBM_SEG_MAP);
#elif defined(PBM_HT16K33)
#include "SBK_WB_HT16K33_V1_1_0.h"
HT16K33 PBM_driver;
PanelBarMeter panelBarMeter(&PBM_SEG_NUMBER, &PBM_DIRECTION, &PBM_driver, PBM_DRIVER_ADDRESS, PBM_SEG_MAP);
#endif

/***********************************************/
/*               WS2812 LEDs strip             */
/***********************************************/
// LEDs index, positions and animations directions should be defined in SBK_WRISTBLASTER_CONFIG.h file
// Define the main WS2812 LEDs strip for the wrist blaster. If the Power Cell LEDs are on the same strip,
// the total number of LEDs includes both the blaster and the Power Cell LEDs.
#if defined(POWERCELL_STRIP) && POWERCELL_ON_SAME_STRIP
Adafruit_NeoPixel blasterLeds = Adafruit_NeoPixel(TOTAL_LEDS_NUMBER + POWERCELL_NUMLEDS, LEDS_STRIP1_PIN, NEO_GRB + NEO_KHZ800);
#else
Adafruit_NeoPixel blasterLeds = Adafruit_NeoPixel(TOTAL_LEDS_NUMBER, LEDS_STRIP1_PIN, NEO_GRB + NEO_KHZ800);
#endif
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

/***********************************************************/
/*     POWER CELL LEDS STRIP OR BARMETER (OPTIONAL)        */
/***********************************************************/
#if defined(POWERCELL_STRIP) || defined(POWERCELL_BARMETER)
#define POWERCELL_EXIST
#endif
#if defined(POWERCELL_STRIP) && POWERCELL_ON_SAME_STRIP
PowerCell powerCell(&blasterLeds, &POWERCELL_NUMLEDS, &POWERCELL_FIRST, &POWERCELL_LAST, &POWERCELL_DIRECTION);
#elif defined(POWERCELL_STRIP) && defined(LEDS_STRIP2_PIN)
Adafruit_NeoPixel powercellLeds = Adafruit_NeoPixel(POWERCELL_NUMLEDS, LEDS_STRIP2_PIN, NEO_GRB + NEO_KHZ800);
PowerCell powerCell(&powercellLeds, &POWERCELL_NUMLEDS, &POWERCELL_FIRST, &POWERCELL_LAST, &POWERCELL_DIRECTION);
#elif defined(POWERCELL_STRIP) && !defined(LEDS_STRIP2_PIN)
#error "Power Cell LEDs STRIP : Optional NEW Power Cell LEDs strip enabled but strip pin is not defined, wrong board or wrong pins definition."
#endif
#if defined(POWERCELL_BARMETER) && defined(PCBM_MAX72XX_ON_PBM_CHAIN)

// No new driver instance: use device 1 of PBM_driver.
PowerCellBarMeter powerCell(
    &POWERCELL_SEG_NUMBER,
    &POWERCELL_DIRECTION,
    &PBM_driver,
    PCBM_DRIVER_ADDRESS,
    PCBM_SEG_MAP);

#elif defined(POWERCELL_BARMETER) && defined(PCBM_MAX72XX_SEPARATE)

#if (defined(PCBM_DIN_PIN) + defined(PCBM_CLK_PIN) + defined(PCBM_LOAD_PIN)) != 3
#error "POWER CELL BAR METER : bar meter with MAX72xx deriver is defined but the pins are undefined (PCBM_DIN_PIN, PCBM_CLK_PIN, PCBM_LOAD_PIN). Wrong board or wrong pins definition."
#endif
#include "SBK_WB_MAX72xx_V1_1_0.h"
MAX72xx PCBM_driver(PCBM_DIN_PIN, PCBM_CLK_PIN, PCBM_LOAD_PIN, 1);
PowerCellBarMeter powerCell(&POWERCELL_SEG_NUMBER, &POWERCELL_DIRECTION, &PCBM_driver, PCBM_DRIVER_ADDRESS, PCBM_SEG_MAP);
#elif defined(POWERCELL_BARMETER) && defined(PCBM_HT16K33)
#include "SBK_WB_HT16K33_V1_1_0.h"
HT16K33 PCBM_driver;
PowerCellBarMeter powerCell(&POWERCELL_SEG_NUMBER, &POWERCELL_DIRECTION, &PCBM_driver, PCBM_DRIVER_ADDRESS, PCBM_SEG_MAP);
#endif

/***********************************************************/
/*           BATT POWER MEASUREMENT (OPTIONAL)             */
/***********************************************************/
#ifndef BATT_PIN
#pragma message("SBK_WRISTBLASTER_CORE : BATT_PIN not defined, no battery monitoring available.")
BattMoniroting batt; // No parameters = no batt monitoring
#else
BattMoniroting batt(BATT_PIN, POWER_MONITORING, selectedBattery, BATT_LOW_CUTOFF, BATT_READING_SCALING_FACTOR);
#endif

/***********************************************/
/*      Fire button Single Led Indicator       */
/***********************************************/
// This is not an addressable LED, it's a single led drive directly from the pin and a series resistor
// See CONFIG.H to enable/disable this indicator
SingleColorIndicator fireButtonSingleLed(FIRE_BUTTON_LED_PIN, FIRE_BUTTON_LED_READY, "IND_fireBtn");

/*********************************************/
/*    AUDIO PLAYER definition and helpers    */
/*********************************************/
// bool playing = false; // variable for playin status
bool cycling = false; // Cycling/looping track mode tracker
/****************************/
/*    PLAYER definitions    */
/****************************/
Player player(VOLUME_MAX, VOLUME_START,
              RX_PIN, TX_PIN, BUSY_PIN,
              AMP_MUTE_PIN,
              VOL_POT_PIN, VOL_POT_READY,
              PLAYER_COMMAND_DELAY,
              AUDIO_ADVANCE,
              BUSY_PIN_READY);
/************************************/
/* Audio board SERIAL COMMUNICATION */
/************************************/
#if defined(PLAYER_SOFTSERIAL)
SoftwareSerial SoftSerial(RX_PIN, TX_PIN);
#pragma message("Compiling for DFPlayer with SoftSerial communication.")
#elif defined(PLAYER_SERIAL1)
#pragma message("Compiling for DFPlayer with Serial1 communication.")
#endif

/*********************************************/
/*                                           */
/* BUTTONS & SWITCHES definition and helpers */
/*                                           */
/*********************************************/
// For switches and buttons managing
Switch PBintensify(INTENSIFY_BUTTON_PIN, INTENSIFY_PB_LOGIC, "SW Intensify");
Switch SWmain(MAIN_POWER_SWITCH_PIN, MAIN_POWER_SW_LOGIC, "SW Main");
Switch SWcyclotron(CYCLOTRON_POWER_SWITCH_PIN, CYCLOTRON_POWER_SW_LOGIC, "SW Cyclotron");
Switch SWactivate(ACTIVATE_SWITCH_PIN, ACTIVATE_SW_LOGIC, "SW Activate");
Switch PBfire(FIRE_BUTTON_PIN, FIRE_PB_LOGIC, "Fire Button");

#ifdef SMOKE_FEATURES_ENABLED
/*********************************************/
/*                SMOKER & FAN               */
/*********************************************/
Smoker smoker(SMOKE_RELAY_PIN, FAN_RELAY_PIN,
              &SMOKER_MIN_OFF_TIME, &SMOKER_MAX_ON_TIME);
#endif

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
#if defined(PLAYER_SOFTSERIAL)
  SoftSerial.begin(PLAYER_BAUDRATE);
  if (!player.begin(SoftSerial))
    DEBUG_PRINTLN("Player SoftSerial Init failed, please check the wire connection or pins definition!");
#elif defined(PLAYER_SERIAL1)
  Serial1.begin(PLAYER_BAUDRATE);
  if (!player.begin(Serial1))
    DEBUG_PRINTLN("Player Serial1 init failed, please check the wire connection or pins definition!");
#else
#pragma message("Player communication is not defined, check board/pins definitions.")
#endif

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

  // Initial battery monitoring reading
  batt.begin();
  battLevel = batt.readBattPercentage();

// Setup Optional PowerCell animations
#ifdef POWERCELL_EXIST
#if (defined POWERCELL_STRIP) && !POWERCELL_ON_SAME_STRIP // Power Cell is on a strip alone
  powercellLeds.setBrightness(255);
  powercellLeds.clear();
  powercellLeds.show();
#endif
#ifdef POWERCELL_BARMETER
#if defined(PCBM_MAX72XX_ON_PBM_CHAIN)
  PBM_driver.begin(PCBM_DRIVER_ADDRESS); // device 1
#else
  PCBM_driver.begin(PCBM_DRIVER_ADDRESS);
#endif
#endif
  powerCell.begin(battLevel, selectedBattery);
  powerCell.clear();
  powerCell.update(battLevel);
#endif

  // Setup Single Led Indicator
  fireButtonSingleLed.begin();

  // setup bar meter
  PBM_driver.begin(PBM_DRIVER_ADDRESS);
  panelBarMeter.begin();
  panelBarMeter.clear();
  panelBarMeter.update();

  // setup for the switches/buttons
  SWactivate.begin();
  PBintensify.begin();
  SWmain.begin();
  SWcyclotron.begin();
  PBfire.begin();

#ifdef SMOKE_FEATURES_ENABLED
  // Smoker setup
  smoker.begin(DISABLE);
#endif
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

  // Check battery state and goes into POWER_DOWN then LOW_BATT if too low...
  if (currentTime - prevBattReading > 2000)
  {
    prevBattReading = currentTime;

    batt.updateReading();
    battLevel = batt.readBattPercentage();
    if (POWER_MONITORING &&
        WBstate != STATE_POWER_ON_TO_OFF &&
        WBstate != STATE_LOW_BATT &&
        batt.isBattTooLow())
    {
      WBstate = STATE_POWER_ON_TO_OFF;
      stageFlag = 0;
    }
  }

  // LEDS UPDATE
  bool update_leds_chain = false;
  getLEDsSchemeForThisState(); // Get new leds schemes for this loop
  // Update simple LEDs states to last animations schemes.
  panelBarMeter.update(currentTime);
  fireButtonSingleLed.update(currentTime);
  // Optional Power Cell LEDs/BARMETER
// Update optional PowerCell addressable LEDs chain with last color schemes.
#ifdef POWERCELL_EXIST
  getPowercellLEDsSchemeForThisState();
#if defined(POWERCELL_STRIP) && !POWERCELL_ON_SAME_STRIP
  if (powerCell.update(currentTime, battLevel))
    powercellLeds.show();
#elif defined(POWERCELL_STRIP) && POWERCELL_ON_SAME_STRIP
  {
    update_leds_chain |= powerCell.update(currentTime, battLevel);
  }
#elif defined(POWERCELL_BARMETER)
  powerCell.update(currentTime, battLevel);
#endif
#endif

  // Update addressable LEDs chain with last color schemes.
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

#ifdef SMOKE_FEATURES_ENABLED
  // Update smoker
  smoker.update(currentTime);
#endif

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
  {
    DEBUG_PRINTLN("Invalid WBstate, STATE_ZERO...");
    WBstate = SWmain.isON() ? STATE_POWER_ON_TO_OFF : STATE_POWER_OFF;
    stageFlag = 0;
    break;
  }

  //////////////////////////////////////////////
  case STATE_LOW_BATT:
  {
    switch (stageFlag)
    {
    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_LOW_BATT");

      // Specifics state initializers :
      heatLevel = 0;     // Reset heat level to 0
      player.stop();     // Stop player, no track for this state
      playingTrack = -1; // No state track is being played
      DEBUG_PRINTLN("Player STOP");

      // Standard initializers
      stateStartTime = currentTime;
      stageFlag = 1; // End state initialization when stageFlag is 1

      DEBUG_PRINTLN();
      break;
    }

    case 1: // This wrist blaster state loop :
    {
      if (BATT_LOW_CUTOFF == ENABLE)
      {
        // Wrist blaster state exits by priority :
        if (!batt.isBattTooLow())
        {
          WBstate = STATE_POWER_OFF;
          stageFlag = 0;
          break;
        }
      }

      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
      break;
    }
    }
    break;
  }

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
      playingTrack = -2;          // No state track is being played
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
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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

#ifdef SMOKE_FEATURES_ENABLED
      smoker.smoke(DISABLE, DISABLE_FAN);
#endif

      // Specifics state initializers :
      heatLevel = 0;     // Reset heat level to 0
      player.stop();     // Stop player, no track for this state
      playingTrack = -1; // No state track is being played
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
#ifdef SMOKE_FEATURES_ENABLED
      checkSmokerEnabling(); // Enable/disable smoker with fire button while in POWER OFF state
#endif

      // Wrist blaster state exits by priority :
      if (checkIfSwitchExit(getPartyModeState(), STATE_PARTY_MODE_IN)) // Check if the Intensify Switch is ON, goes into STATE_PARTY_MODE_IN
        break;

      if (checkIfSwitchExit(SWmain.isON(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is ON, goes into main booting state STATE_POWER_OFF_TO_ON
        break;
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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

#ifdef SMOKE_FEATURES_ENABLED
      smoker.smoke(DISABLE, DISABLE_FAN);
#endif

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
#ifdef POWERCELL_EXIST
      // With a PowerCell present, keep shutdown non-interruptible:
      // the PowerCell shutdown animation must complete to finish its internal state transition
      // before allowing any reboot path.
      if (checkIfTrackDoneExit(batt.isBattTooLow() ? STATE_LOW_BATT : STATE_POWER_OFF)) // Check if track is ended before going into STATE_POWER_OFF or STATE_LOW_BATT
        break;
#else
      // Without a PowerCell shutdown sequence to preserve, allow the Main switch
      // to cancel shutdown and restart immediately.
      if (checkIfSwitchExit(SWmain.isON(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is ON, goes into main booting state STATE_POWER_OFF_TO_ON
        break;

      // If there is no restart request, complete the regular shutdown path.
      if (checkIfTrackDoneExit(STATE_POWER_OFF)) // Check if track is ended before going into STATE_POWER_OFF
        break;
#endif

      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWactivate.isON(), STATE_CYCLOTRON_ON_TO_FULL)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
        break;

      if (checkIfTrackDoneExit(STATE_CYCLOTRON_ON)) // Check if track is ended before going into STATE_CYCLOTRON_ON
        break;
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isON(), STATE_CYCLOTRON_OFF_TO_ON)) // Check if Cyclotron Switch is ON, goes into cyclotron booting state STATE_CYCLOTRON_OFF_TO_ON
        break;

      if (checkIfTrackDoneExit(STATE_POWER_ON)) // Check if track is ended before going into STATE_POWER_ON witch cyclotron OFF
        break;
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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

      if (checkIfSwitchExit(SWmain.isOFF(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
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
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWactivate.isOFF(), STATE_CYCLOTRON_FULL_TO_ON)) // Check if Cyclotron Activate Switch is OFF, goes into cyclotron returning to normal STATE_CYCLOTRON_FULL_TO_ON
        break;

      if (checkIfTrackDoneExit(STATE_CYCLOTRON_FULL_POWER)) // Check if track is ended before going into STATE_CYCLOTRON_FULL_POWER
        break;
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWactivate.isON(), STATE_CYCLOTRON_ON_TO_FULL)) /// Check if Cyclotron Activate Switch is ON, goes into cyclotron booting FULL state STATE_CYCLOTRON_ON_TO_FULL
        break;

      if (checkIfTrackDoneExit(STATE_CYCLOTRON_ON)) // Check if track is ended before going into STATE_CYCLOTRON_ON
        break;
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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

      if (checkIfSwitchExit(SWmain.isOFF(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
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
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
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
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      fireType = CAPTURE; // tail and reboot to STATE_CYCLOTRON_ON
#ifdef SMOKE_FEATURES_ENABLED
      smoker.smoke(ENABLE); // Put the smoke and pump on, but not the fan
#endif

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

      if (checkIfSwitchExit(SWmain.isOFF(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
        break;

      if (checkIfSwitchExit(SWactivate.isON(), STATE_CYCLOTRON_ON_TO_FULL)) // Check if Cyclotron Activate Switch is OFF, goes into cyclotron returning to normal STATE_CYCLOTRON_FULL_TO_ON
        break;

      // Wrist blaster going into tail before overheat warning:
      if (checkIfSwitchExit(PBfire.isOFF(), STATE_CAPTURE_TAIL))
      {
#ifdef SMOKE_FEATURES_ENABLED
        smoker.smoke(DISABLE, DISABLE_FAN);
#endif
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
#ifdef SMOKE_FEATURES_ENABLED
      smoker.startBurst(DURATION_CAPTURE_OVERHEAT, WITH_FAN); // Put the smoke, pump and fan ON for the state duration
#endif

      // Standard initializers
      stateStartTime = currentTime;
      stageFlag = 3;

      DEBUG_PRINTLN();
      break;
    }

    case 3: // CAPTURE OVERHEAT
    {
      //  Specifics state mechanics :
      heatLevel = 0; // Venting and cooling done...

      // Determine next state
      BlasterState next = SWmain.isOFF()        ? STATE_POWER_ON_TO_OFF
                          : SWcyclotron.isOFF() ? STATE_POWER_ON // Cyclotron switch is off, goes into POWER ON
                          : SWactivate.isON()   ? STATE_CYCLOTRON_ON_TO_FULL
                                                : STATE_CYCLOTRON_OFF_TO_ON; // venting is done, reboot to cyclotron ON or full power

      // Check if overheating tail track is done
      if (checkIfTrackDoneExit(next))
      {
#ifdef SMOKE_FEATURES_ENABLED
        smoker.smoke(DISABLE, DISABLE_FAN);
#endif
        break;
      }
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
        break;

      // if (checkIfSwitchExit(SWcyclotron.isOFF(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
      //   break;

      // Check if tail track is done
      if (!player.isPlaying()) // || (currentTime - stateStartTime) >= TRACK_LENGTH[WBstate])
      {
        // If tailing is done goes into POWER OFF, or into cyclotron ON or FULL depending of the fireng type
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
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
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
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
        break;

      // Check if tail track is done
      if (!player.isPlaying())
      {
        // If tailing is done goes into cyclotron ON or FULL depending of the fireng type
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
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      DEBUG_PRINTLN("STATE_BURST_OVERHEAT");

      //  Specifics initializations :
      fireType = BURST; // tail and reboot to STATE_CYCLOTRON_FULL_POWER

      //  Specifics state mechanics :
      heatLevelRisingBurst(); // increment heatLevel for this shot

#ifdef SMOKE_FEATURES_ENABLED
      smoker.smoke(ENABLE);
#endif

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
      if (checkIfSwitchExit(SWmain.isOFF(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
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
#ifdef SMOKE_FEATURES_ENABLED
      smoker.smoke(ENABLE, ENABLE_FAN);
#endif

      // Standard initializers
      stageFlag = 3;

      DEBUG_PRINTLN();
      break;
    }

    case 3: // This wrist blaster state loop :
    {
      //  Specifics state mechanics :
      heatLevel = 0; // Venting and cooling done...

      // Determine next state
      BlasterState next = SWmain.isOFF()        ? STATE_POWER_ON_TO_OFF
                          : SWcyclotron.isOFF() ? STATE_POWER_ON // Cyclotron switch is off, goes into POWER ON
                          : SWactivate.isON()   ? STATE_CYCLOTRON_ON_TO_FULL
                                                : STATE_CYCLOTRON_OFF_TO_ON; // venting is done, reboot to cyclotron ON or full power

      // Check if overheating tail track is done
      if (checkIfTrackDoneExit(next))
      {
#ifdef SMOKE_FEATURES_ENABLED
        smoker.smoke(DISABLE, DISABLE_FAN);
#endif
        break;
      }
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
      break;
    }
    }
    break;
  }
    //////////////////////////////////////////////
  case STATE_ALL_ON_TO_OFF:
  {
    switch (stageFlag)
    {
    case 0: // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_ALL_ON_TO_OFF");

#ifdef SMOKE_FEATURES_ENABLED
      smoker.smoke(DISABLE, DISABLE_FAN);
#endif

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

#ifdef POWERCELL_EXIST
      // With a PowerCell present, keep shutdown non-interruptible:
      // the PowerCell shutdown animation must complete to finish its internal state transition
      // before allowing any reboot path.
      if (checkIfTrackDoneExit(batt.isBattTooLow() ? STATE_LOW_BATT : STATE_POWER_OFF)) // Check if track is ended before going into STATE_POWER_OFF or STATE_LOW_BATT
        break;
#else
      // Without a PowerCell shutdown sequence to preserve, allow the Main switch
      // to cancel shutdown and restart immediately.
      if (checkIfSwitchExit(SWmain.isON(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is ON, goes into main booting state STATE_POWER_OFF_TO_ON
        break;

      // If there is no restart request, complete the regular shutdown path.
      if (checkIfTrackDoneExit(STATE_POWER_OFF)) // Check if track is ended before going into STATE_POWER_OFF
        break;
#endif

      break;
    }

      //////////////////////////////////////////////
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in Main Loop State Machine, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
      break;
    }
    }
    break;
  }

  default:
  {
    DEBUG_PRINT("Invalid WBstate, recovering: ");
    DEBUG_PRINTLN(WBstate);
    WBstate = SWmain.isON() ? STATE_POWER_ON_TO_OFF : STATE_POWER_OFF;
    stageFlag = 0;
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

  case STATE_LOW_BATT:
  {
    switch (stageFlag)
    {
    case 0:
    {
      clearAllLights(); // clear cyclotron, bar meter, vent, firing jewel and cyclotron LEDS
      topWhiteIndicator.initParam(RED, 50, MEDIUM_BLINK_SP);
      break;
    }
    case 1:
    {
      topWhiteIndicator.blink(DISABLE);
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
      break;
    }
    }
    break;
  }

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
      /*panelBarMeter            */ panelBarMeter.partyModeInit();
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
      /*panelBarMeter            */ panelBarMeter.partyMode();
      /*cyclotron           */ cyclotron.ramp(); // Finish party mode in sequence if not done
      /*firingRod           */                   // Cleared
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */ panelBarMeter.fillUpEmptyDownOnceInit(getDuration());
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
      /*panelBarMeter            */ panelBarMeter.fillUpEmptyDownOnce();
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ // Cleared
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */ panelBarMeter.fillUpFastEmptyDownSlowOnceInit(getDuration(), ENABLE);
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
      /*panelBarMeter            */ panelBarMeter.fillUpFastEmptyDownSlowOnce();
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ // Cleared
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
#ifdef SMOKE_FEATURES_ENABLED
      topWhiteIndicator.initParam(smoker.enable() ? GREEN : RED, 50);
#endif
      break;
    }
    case 1:
    {
      if (PBfire.isOFF())
        topWhiteIndicator.flash(5000); // set LED_INDEX_TOP_WHITE led green flashing
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */ panelBarMeter.clear();
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
      /*panelBarMeter            */ // Cleared
      /*cyclotron           */      // Cleared
      /*firingRod           */      // Cleared
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */ panelBarMeter.clear();
      /*cyclotron           */ cyclotron.clear();
      /*firingRod           */ firingRod.clear();
      break;
    }
    case 1:
    { /*slowBlowIndicator   */
      slowBlowIndicator.solid();
      /*topWhiteIndicator   */ topWhiteIndicator.blink(DISABLE);
      /*topYellowIndicator  */ topYellowIndicator.solid();
      /*frontOrangeIndicator*/      // Cleared
      /*fireButtonSingleLed */      // Cleared
      /*vent                */      // Cleared
      /*panelBarMeter            */ // Cleared
      /*cyclotron           */      // Cleared
      /*firingRod           */      // Cleared
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */ panelBarMeter.fillDownEmptyDownOnceInit(getDuration(), ENABLE);
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
      /*panelBarMeter            */ panelBarMeter.fillDownEmptyDownOnce();
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ // cleared
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */ panelBarMeter.fillUpFastEmptyDownSlowOnceInit(getDuration(), ENABLE);
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
      /*panelBarMeter            */ panelBarMeter.fillUpFastEmptyDownSlowOnce();
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ // cleared
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */ panelBarMeter.cyclotronIdleInit(heatLevel);
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
      /*panelBarMeter            */ panelBarMeter.cyclotronIdle(heatLevel);
      /*cyclotron           */ cyclotron.ramp(); // Finishing ramping if not done
      /*firingRod           */                   // cleared
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*vent                */ vent.initParam(WARM_WHITE, 75, getDuration());                        // finishing fade if not done
      /*panelBarMeter            */ panelBarMeter.fillDownEmptyDownOnceInit(getDuration(), DISABLE); // full bar from top and empty it from top
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
      /*vent                */ vent.ramp();                                // finishing fade if not done
      /*panelBarMeter            */ panelBarMeter.fillDownEmptyDownOnce(); // full bar from top and empty it from top
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ // cleared
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*vent                */ vent.initParam(WARM_WHITE, 50, getDuration());                              // finishing fade if not done
      /*panelBarMeter            */ panelBarMeter.fillUpFastEmptyDownSlowOnceInit(getDuration(), DISABLE); // full bar and slow emptying from top to bottom
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
      /*vent                */ vent.ramp();                                      // finishing fade if not done
      /*panelBarMeter            */ panelBarMeter.fillUpFastEmptyDownSlowOnce(); // full bar and slow emptying from top to bottom
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ // cleared
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */ panelBarMeter.cyclotronIdleFullInit(heatLevel);
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
      /*panelBarMeter            */ panelBarMeter.cyclotronIdleFull(heatLevel);
      /*cyclotron           */ cyclotron.ramp(); // Finishing ramping if not done
      /*firingRod           */                   // Cleared
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */ panelBarMeter.fireInit(CAPTURE);
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
      /*panelBarMeter            */ panelBarMeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */
      if (prevState == STATE_CYCLOTRON_ON) // Only init if it's direct warning without going through STATE_CAPTURE
      {
        panelBarMeter.fireInit(CAPTURE);
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
      /*panelBarMeter            */ panelBarMeter.fire(heatLevel);
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
      /*panelBarMeter            */ panelBarMeter.fireInit(BURST, END_SEQ); // reverse end finish fire animation
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
      /*panelBarMeter            */ panelBarMeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */ panelBarMeter.fireInit(BURST, END_SEQ); // reverse end finish fire animation
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
      /*panelBarMeter            */ panelBarMeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */ panelBarMeter.fireInit(BURST);
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
      /*panelBarMeter            */ panelBarMeter.fire(heatLevel);
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
      /*panelBarMeter            */ panelBarMeter.fireInit(CAPTURE, END_SEQ); // No init, reverse and finish the burst sequence...
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
      /*panelBarMeter            */ panelBarMeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      /*panelBarMeter            */ panelBarMeter.fireInit(BURST);
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
      /*panelBarMeter            */ panelBarMeter.fire(heatLevel);
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
      /*panelBarMeter            */ panelBarMeter.fireInit(CAPTURE, END_SEQ); // No init, reverse and finish the burst sequence...
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
      /*panelBarMeter            */ panelBarMeter.fire(heatLevel);
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
      break;
    }
    }
    break;
  }

  case STATE_ALL_ON_TO_OFF:
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
      /*panelBarMeter            */ panelBarMeter.fillUpFastEmptyDownSlowOnceInit(getDuration(), ENABLE);
      /*cyclotron           */ cyclotron.rampInit(CYC_OFF, getDuration());
      /*firingRod           */ firingRod.strobeInit(NO_SHUFFLE, 0, 1000);
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
      /*panelBarMeter       */ panelBarMeter.fillUpFastEmptyDownSlowOnce();
      /*cyclotron           */ cyclotron.ramp();
      /*firingRod           */ firingRod.strobe();
      break;
    }

    default:
    {
      DEBUG_PRINT("Invalid stageFlag in LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
      break;
    }
    }
    break;
  }
  }
}

//  END_SEQ of animations functions in wrist blaster states
////////////////////////////////////////////////

/***********************************************************************/
/*   OPTIONAL POWERCELL ANIMATIONS functions in wrist blaster states   */
/***********************************************************************/
#ifdef POWERCELL_EXIST
void getPowercellLEDsSchemeForThisState()
{
  // Some sequences need to be initialized with wrist blaster state, stageFlag is used to know if wrist blaster state is in initialization

  switch (WBstate)
  {
  case STATE_ZERO:
    break;

  case STATE_LOW_BATT:
  {
    switch (stageFlag)
    {
    case 0:
    {
      powerCell.lowBattInit(MEDIUM_BLINK_SP);
      break;
    }
    case 1:
    {
      powerCell.lowBatt();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
      break;
    }
    }
    break;
  }

  case STATE_PARTY_MODE:

  {
    switch (stageFlag)
    {
    case 0:
    {
      powerCell.rampToIdleInit(PC_CYC_ON_UPDATE_INT, 0);
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
    {
      powerCell.rampToIdleInit(PC_CYC_ON_UPDATE_INT, getDuration());
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_CYC_ON_UPDATE_INT, 0);
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.poweredDownInit(5000);
      break;
    }
    case 1:
    {
      powerCell.poweredDown();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.bootInit(min(getDuration(), getSpecificDuration(STATE_POWER_ON_TO_OFF)));
      break;
    }
    case 1:
    {
      powerCell.boot();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.shutDownInit(getDuration());
      break;
    }
    case 1:
    {
      powerCell.shutDown();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_POWER_ON_UPDATE_INT, 0);
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_CYC_ON_UPDATE_INT, getDuration());
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_POWER_ON_UPDATE_INT, getDuration());
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_CYC_ON_UPDATE_INT, 0);
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_CYC_FULL_UPDATE_INT, getDuration());
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_CYC_ON_UPDATE_INT, getDuration());
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_CYC_FULL_UPDATE_INT, 0);
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_FIRING_MAX_UPDATE_INT, getDuration());
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_FIRING_MAX_UPDATE_INT, getDuration() - DURATION_CAPTURE_OVERHEAT);
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    case 2:
    {
      powerCell.rampToIdleInit(PC_CYC_ON_UPDATE_INT, min(3000, DURATION_CAPTURE_OVERHEAT - 3000));
      break;
    }
    case 3:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_CYC_ON_UPDATE_INT, getDuration());
      break;
    }
    case 1:
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_FIRING_MAX_UPDATE_INT, getDuration() - DURATION_BURST_TAIL);
      break;
    }
    case 1: // BURST
    {
      powerCell.rampToIdle();
      break;
    }
    case 2: // TAIL initialisation
    {
      powerCell.rampToIdleInit(PC_CYC_FULL_UPDATE_INT, DURATION_BURST_TAIL);
      break;
    }
    case 3: // TAIL
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
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
      powerCell.rampToIdleInit(PC_FIRING_MAX_UPDATE_INT, getDuration() - DURATION_BURST_OVERHEAT);
      break;
    }
    case 1: // BURST with warning
    {
      powerCell.rampToIdle();
      break;
    }
    case 2: // OVERHEAT initialisation
    {
      powerCell.rampToIdleInit(PC_CYC_FULL_UPDATE_INT, min(3000, DURATION_BURST_OVERHEAT - 3000));
      break;
    }
    case 3: // OVERHEAT
    {
      powerCell.rampToIdle();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
      break;
    }
    }
    break;
  }

  case STATE_ALL_ON_TO_OFF:
  {
    switch (stageFlag)
    {
    case 0:
    {
      powerCell.shutDownInit(getDuration());
      break;
    }
    case 1:
    {
      powerCell.shutDown();
      break;
    }
    default:
    {
      DEBUG_PRINT("Invalid stageFlag in PowerCell LEDs Scheme, state ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT(", stage ");
      DEBUG_PRINTLN(stageFlag);
      stageFlag = 0;
      break;
    }
    }
    break;
  }
  }
}
#endif

//  END_SEQ of Optional PowerCell animations functions in wrist blaster states
////////////////////////////////////////////////

void clearAllLights()
{
  // Clear leds
  cyclotron.clear();
  panelBarMeter.clear();
  vent.clear();
  slowBlowIndicator.clear();
  topWhiteIndicator.clear();
  topYellowIndicator.clear();
  frontOrangeIndicator.clear();
  fireButtonSingleLed.clear();
  firingRod.clear();
#ifdef POWERCELL_EXIST
  powerCell.clear();
#endif

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

  // Timer expired ??? Change state
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
      player.next(); // Short press ??? Next track
    else
      player.previous(); // Long press ??? Previous track
  }
}

void heatLevelCooling()
{
  if (heatLevel == 0)
    return;

  if (currentTime - heatLevelPrevUpdate > 125)
  // Decrease heat level every 125ms, about 12.5secondes before going into overheat.
  {
    heatLevelPrevUpdate = currentTime;

    if (heatLevel > 0) // Ensure that heatLevel doesn't overflow as unsigned...
      heatLevel -= 1;
  }
}

void heatLevelRisingCapture() // // Increase heat level from 0 to 100 over DURATION_CAPTURE_MAX.
{                             // HeatLevel 0 -100 %, at 100%, wrist blaster goes into overheat...
  if (currentTime - heatLevelPrevUpdate < (DURATION_CAPTURE_MAX / 100))
    return;

  heatLevelPrevUpdate = currentTime;

  heatLevel += 1;
  heatLevel = min(100, heatLevel);
  return;
}

void heatLevelRisingBurst() // Increase heat level by approximately 100 / MAX_BURST_SHOTS for each burst.
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

#ifdef SMOKE_FEATURES_ENABLED
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
#endif

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
