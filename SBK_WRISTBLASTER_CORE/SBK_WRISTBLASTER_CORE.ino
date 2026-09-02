/**
 * @file        SBK_WRISTBLASTER_CORE.ino
 * @brief       Lights and sound control code for the SBK Wrist Blaster replica or other props.
 *
 * @author      Samuel Barabe
 * @copyright   Copyright (c) 2025-2026 Samuel Barabe
 * @license     MIT License
 * @version     2.0.0
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
 * Library dependencies:
 * - Adafruit NeoPixel
 * - SBK_Button
 * - SBK_BarDrive
 * - SBK_MAX72xx or SBK_HT16K33, depending on the selected bar-meter driver
 *
 * Audio control uses the built-in SBK_WB_PlayerEngine and requires no external
 * DFPlayer library.
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
 * - Predefined track durations provide the timeline for state transitions,
 *   animations, and effects, as well as audio playback timing when the BUSY pin is not used.
 * - Incorrect durations may desynchronize animations and effects from the audio track.
 * - Advanced users may customize animations or state transitions in the core engine.
 *
 * If you find this useful, please consider supporting development at:
 * https://github.com/sbarabe/SBK_WRISTBLASTER
 *
 * @see https://opensource.org/licenses/MIT
 * DFPlayer Mini commands are sent by the lightweight SBK_WB_PlayerEngine.
 * @see https://github.com/adafruit/Adafruit_NeoPixel
 **/

#include <Arduino.h>
#include "SBK_WRISTBLASTER_CONFIG.h"

/*********************************************/
/*      GENERAL definitions and helpers      */
/*********************************************/
// Helpers variables declarations and initial states :
uint32_t now = 0;                            // To keep track in a loop
BlasterState WBstate = STATE_POWER_OFF;      // Initial wrist blaster state in the main loop
BlasterState prevState = STATE_ZERO;         // State tracking for some sequences
bool stateInitialized = false;               // False until the current state is initialized
bool prevStateInitialized = false;           // Previous value used by debug tracking
uint32_t stateStartTime = 0;                 // general time tracker for functions timers and delays
uint8_t heatLevel = 0;                       // Tracker for overheat
uint32_t heatLevelPrevUpdate = 0;            // Tracker for overheat
OverheatPhase overheatPhase = PHASE_WARNING; // Initial overheat phase
bool phaseInitialized = false;               // False until the current phase is initialized
uint32_t phaseStartTime = 0;                 // Shared phase start time tracker
BurstPhase burstPhase = BURST_PHASE_FIRING;
#ifndef CAPTURE
constexpr bool CAPTURE = false;
#endif
#ifndef BURST
constexpr bool BURST = true;
#endif
bool fireType = CAPTURE; // 0 = Capture, 1 = Burst : help managing reboots and firing tails
uint8_t battLevel = 100; // Battery level variable for battery power monitoring option
uint32_t prevBattReading = 0;
// Helpers functions declarations, functions are defined after the main loop :
bool stateInitialization();                                         // Standard initialisation sequence for most states
void handleWristBlasterState();                                     // Execute the primary state-machine control flow
bool checkIfTrackDoneExit(BlasterState next_state);                 // Change state when the current track timeline ends
bool checkIfSwitchExit(bool switch_state, BlasterState next_state); // Change state when the supplied switch condition is true
bool checkIfTimerExit(uint16_t time, BlasterState next_state);      // Change state when the current state timer expires
void enterState(BlasterState next_state);                           // Prepare phase data and enter a new state
void getLEDsSchemeForThisState();                                   // This function contains animations settings and calling for all states
void getIndicatorScheme();
void getSlowBlowIndicatorScheme();
void getTopWhiteIndicatorScheme();
void getTopYellowIndicatorScheme();
void getFrontOrangeIndicatorScheme();
void getFireButtonIndicatorScheme();
void getVentScheme();
void getPanelBarMeterScheme();
void getCyclotronScheme();
void getFiringRodScheme();
void getPowerCellScheme();                        // This function contains optional Power Cell animations
void playThisStateTrack();                        // play state track
void playThisTrack(uint8_t track);                // Play a specific track other than the state track
uint16_t getDuration();                           // Get the usable animation/effect duration of the current state
uint16_t getSpecificDuration(BlasterState state); // Get the usable animation/effect duration of a specific state
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
#if defined(PCBM_MAX72XX_ON_PBM_CHAIN)
constexpr uint8_t PBM_MAX72XX_DEVICE_COUNT = 2;
#else
constexpr uint8_t PBM_MAX72XX_DEVICE_COUNT = 1;
#endif

SBK_MAX72xxSoft PBM_driver(PBM_DIN_PIN, PBM_CLK_PIN, PBM_LOAD_PIN, PBM_MAX72XX_DEVICE_COUNT);
#if defined(PBM_BL28_3005SK_MAPPING)
SBK_BarDrive<SBK_MAX72xxSoft> panelBarMeter(&PBM_driver, PBM_BARDRIVE_DEVICE_INDEX,
                                            MatrixPreset::BL28_3005SK,
                                            static_cast<BarDirection>(PBM_DIRECTION));
#else
SBK_BarDrive<SBK_MAX72xxSoft> panelBarMeter(&PBM_driver, PBM_BARDRIVE_DEVICE_INDEX,
                                            MatrixPreset::BL28_3005SA,
                                            static_cast<BarDirection>(PBM_DIRECTION));
#endif
#elif defined(PBM_HT16K33)
#if defined(PBM_BL28_3005SK_MAPPING)
SBK_HT16K33 PBM_driver(1);
SBK_BarDrive<SBK_HT16K33> panelBarMeter(&PBM_driver, PBM_BARDRIVE_DEVICE_INDEX,
                                        MatrixPreset::BL28_3005SK,
                                        static_cast<BarDirection>(PBM_DIRECTION));
#else
SBK_HT16K33 PBM_driver(1);
SBK_BarDrive<SBK_HT16K33> panelBarMeter(&PBM_driver, PBM_BARDRIVE_DEVICE_INDEX,
                                        MatrixPreset::BL28_3005SA,
                                        static_cast<BarDirection>(PBM_DIRECTION));
#endif
#endif

const uint8_t PANEL_MAX_PERCENT = 100;
uint8_t panelHeatFloorPercent = 0;

/***********************************************/
/*               WS2812 LEDs strip             */
/***********************************************/
// LEDs index, positions and animations directions should be defined in SBK_WRISTBLASTER_CONFIG.h file
// Define the main WS2812 LEDs strip for the wrist blaster. If the Power Cell LEDs are on the same strip,
// the total number of LEDs includes both the blaster and the Power Cell LEDs.
#if defined(POWERCELL_STRIP) && POWERCELL_ON_SAME_STRIP
Adafruit_NeoPixel blasterLeds = Adafruit_NeoPixel(TOTAL_LEDS_NUMBER + POWERCELL_NUMLEDS, LEDS_STRIP1_PIN, MAIN_STRIP_COLOR_ORDER + NEO_KHZ800);
#else
Adafruit_NeoPixel blasterLeds = Adafruit_NeoPixel(TOTAL_LEDS_NUMBER, LEDS_STRIP1_PIN, MAIN_STRIP_COLOR_ORDER + NEO_KHZ800);
#endif
FiringRod firingRod(&blasterLeds,
                    FIRE_ROD_POT_PIN, HUE_POT_READY,
                    ROD_NUMLEDS, LED_INDEX_TIP_1ST, LED_INDEX_TIP_LAST);
Vent vent(&blasterLeds, LED_INDEX_VENT);
Indicator slowBlowIndicator(&blasterLeds, LED_INDEX_SLOWBLOW);
Indicator topYellowIndicator(&blasterLeds, LED_INDEX_TOP_YELLOW);
Indicator topWhiteIndicator(&blasterLeds, LED_INDEX_TOP_WHITE);
Indicator frontOrangeIndicator(&blasterLeds, LED_INDEX_FRONT_ORANGE);
Cyclotron cyclotron(&blasterLeds,
                    CYC_NUMLEDS, LED_INDEX_CYC_START, LED_INDEX_CYC_END,
                    CYC_RING_1ST, CYC_RING_LAST, CYC_CENTER,
                    CYCLOTRON_DIRECTION);

/***********************************************************/
/*     POWER CELL LEDS STRIP OR BARMETER (OPTIONAL)        */
/***********************************************************/
#if defined(POWERCELL_STRIP) || defined(POWERCELL_BARMETER)
#define POWERCELL_EXIST
#endif
#if defined(POWERCELL_STRIP) && POWERCELL_ON_SAME_STRIP
PowerCell powerCell(&blasterLeds, POWERCELL_NUMLEDS, POWERCELL_FIRST, POWERCELL_LAST, POWERCELL_DIRECTION);
#elif defined(POWERCELL_STRIP) && defined(LEDS_STRIP2_PIN)
Adafruit_NeoPixel powercellLeds = Adafruit_NeoPixel(POWERCELL_NUMLEDS, LEDS_STRIP2_PIN, POWERCELL_STRIP_COLOR_ORDER + NEO_KHZ800);
PowerCell powerCell(&powercellLeds, POWERCELL_NUMLEDS, POWERCELL_FIRST, POWERCELL_LAST, POWERCELL_DIRECTION);
#elif defined(POWERCELL_STRIP) && !defined(LEDS_STRIP2_PIN)
#error "Power Cell LEDs STRIP : Optional NEW Power Cell LEDs strip enabled but strip pin is not defined, wrong board or wrong pins definition."
#endif
#if defined(POWERCELL_BARMETER) && defined(PCBM_MAX72XX_ON_PBM_CHAIN)

// No new driver instance: use device 1 of PBM_driver.
SBK_BarDrive<SBK_MAX72xxSoft> powerCellBarMeter(
    &PBM_driver, PCBM_BARDRIVE_DEVICE_INDEX,
    PCBM_BARDRIVE_ROWS, PCBM_BARDRIVE_COLUMNS,
    POWERCELL_DIRECTION ? BarDirection::REVERSE : BarDirection::FORWARD);

#elif defined(POWERCELL_BARMETER) && defined(PCBM_MAX72XX_SEPARATE)

#if (defined(PCBM_DIN_PIN) + defined(PCBM_CLK_PIN) + defined(PCBM_LOAD_PIN)) != 3
#error "POWER CELL BAR METER : bar meter with MAX72xx deriver is defined but the pins are undefined (PCBM_DIN_PIN, PCBM_CLK_PIN, PCBM_LOAD_PIN). Wrong board or wrong pins definition."
#endif
SBK_MAX72xxSoft PCBM_driver(PCBM_DIN_PIN, PCBM_CLK_PIN, PCBM_LOAD_PIN, 1);
SBK_BarDrive<SBK_MAX72xxSoft> powerCellBarMeter(
    &PCBM_driver, PCBM_BARDRIVE_DEVICE_INDEX,
    PCBM_BARDRIVE_ROWS, PCBM_BARDRIVE_COLUMNS,
    POWERCELL_DIRECTION ? BarDirection::REVERSE : BarDirection::FORWARD);
#elif defined(POWERCELL_BARMETER) && defined(PCBM_HT16K33)
SBK_HT16K33 PCBM_driver(1);
SBK_BarDrive<SBK_HT16K33> powerCellBarMeter(
    &PCBM_driver, PCBM_BARDRIVE_DEVICE_INDEX,
    PCBM_BARDRIVE_ROWS, PCBM_BARDRIVE_COLUMNS,
    POWERCELL_DIRECTION ? BarDirection::REVERSE : BarDirection::FORWARD);
#endif

#ifdef POWERCELL_BARMETER
uint8_t powerCellBatteryPercent = 100;
uint16_t powerCellAnimationInterval = PC_POWER_ON_UPDATE_INT / 2;
uint16_t powerCellRampStartInterval = PC_POWER_ON_UPDATE_INT / 2;
uint16_t powerCellRampTargetInterval = PC_POWER_ON_UPDATE_INT / 2;
uint16_t powerCellRampDuration = 0;
uint32_t powerCellRampStartTime = 0;
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
SingleColorIndicator fireButtonSingleLed(FIRE_BUTTON_LED_PIN, FIRE_BUTTON_LED_READY);

/*********************************************/
/*    AUDIO PLAYER definition and helpers    */
/*********************************************/
/****************************/
/*    PLAYER definitions    */
/****************************/
Player player(VOLUME_MAX, VOLUME_START,
              RX_PIN, TX_PIN, BUSY_PIN,
              AMP_MUTE_PIN,
              VOL_POT_PIN, VOL_POT_READY,
              PLAYER_COMMAND_DELAY
#ifndef PLAYER_USE_BUSY_PIN
              ,
              AUDIO_ADVANCE
#endif
);
/************************************/
/* Audio board SERIAL COMMUNICATION */
/************************************/
#if defined(PLAYER_SOFTSERIAL) && !defined(DISABLE_AUDIO_PLAYER)
#include <SoftwareSerial.h>
SoftwareSerial SoftSerial(RX_PIN, TX_PIN);
#pragma message("Compiling for DFPlayer with SoftSerial communication.")
#elif defined(PLAYER_SERIAL1) && !defined(DISABLE_AUDIO_PLAYER)
#pragma message("Compiling for DFPlayer with Serial1 communication.")
#elif defined(DISABLE_AUDIO_PLAYER)
#pragma message("Compiling with audio player disabled (animation timing stub enabled).")
#endif

/*********************************************/
/*                                           */
/* BUTTONS & SWITCHES definition and helpers */
/*                                           */
/*********************************************/
// All switches and buttons use the MCU's internal pull-up resistor.
Button PBintensify(INTENSIFY_BUTTON_PIN, ButtonWiring::INTERNAL_PULLUP, INTENSIFY_PB_LOGIC);
Button SWmain(MAIN_POWER_SWITCH_PIN, ButtonWiring::INTERNAL_PULLUP, MAIN_POWER_SW_LOGIC);
Button SWcyclotron(CYCLOTRON_POWER_SWITCH_PIN, ButtonWiring::INTERNAL_PULLUP, CYCLOTRON_POWER_SW_LOGIC);
Button SWactivate(ACTIVATE_SWITCH_PIN, ButtonWiring::INTERNAL_PULLUP, ACTIVATE_SW_LOGIC);
Button PBfire(FIRE_BUTTON_PIN, ButtonWiring::INTERNAL_PULLUP, FIRE_PB_LOGIC);

#ifdef SMOKE_FEATURES_ENABLED
/*********************************************/
/*                SMOKER & FAN               */
/*********************************************/
Smoker smoker(SMOKE_RELAY_PIN, FAN_RELAY_PIN,
              SMOKER_MIN_OFF_TIME, SMOKER_MAX_ON_TIME);
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
#if defined(PLAYER_SOFTSERIAL) && !defined(DISABLE_AUDIO_PLAYER)
  SoftSerial.begin(PLAYER_BAUDRATE);
  player.begin(SoftSerial);
#elif defined(PLAYER_SERIAL1) && !defined(DISABLE_AUDIO_PLAYER)
  Serial1.begin(PLAYER_BAUDRATE);
  player.begin(Serial1);
#elif !defined(DISABLE_AUDIO_PLAYER)
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

  // Initialize the Panel BarMeter driver once. A shared PowerCell uses the
  // same MAX72xx instance and therefore requires no second begin().
#ifdef PBM_HT16K33
  PBM_driver.setAddress(PBM_BARDRIVE_DEVICE_INDEX, PBM_DRIVER_ADDRESS);
#endif
  PBM_driver.begin();

// Setup Optional PowerCell animations
#ifdef POWERCELL_EXIST
#if (defined POWERCELL_STRIP) && !POWERCELL_ON_SAME_STRIP // Power Cell is on a strip alone
  powercellLeds.begin();
  powercellLeds.setBrightness(255);
  powercellLeds.clear();
  powercellLeds.show();
#endif
#ifdef POWERCELL_BARMETER
#if defined(PCBM_MAX72XX_ON_PBM_CHAIN)
  // PBM_driver was initialized above for the complete shared chain.
#else
#if defined(PCBM_HT16K33)
  PCBM_driver.setAddress(PCBM_BARDRIVE_DEVICE_INDEX, PCBM_DRIVER_ADDRESS);
#endif
  PCBM_driver.begin();
#endif
  powerCellBatteryPercent = constrain(map(battLevel, 20, 70, 20, 100), 20, 100);
  powerCellBarMeter.clear();
#if defined(PCBM_MAX72XX_ON_PBM_CHAIN)
  PBM_driver.show(PCBM_BARDRIVE_DEVICE_INDEX);
#else
  PCBM_driver.show(PCBM_BARDRIVE_DEVICE_INDEX);
#endif
#else
  powerCell.begin(battLevel, selectedBattery);
  powerCell.clear();
  powerCell.update(battLevel);
#endif
#endif

  // Setup Single Led Indicator
  fireButtonSingleLed.begin();

  // setup bar meter
  panelBarMeter.clear();
  PBM_driver.show(PBM_BARDRIVE_DEVICE_INDEX);

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

  // Report state and initialization changes for troubleshooting.
#ifdef DEBUG_TO_SERIAL
  {
    if (WBstate != prevState || stateInitialized != prevStateInitialized)
    {
      DEBUG_PRINT("Wrist Blaster State = ");
      DEBUG_PRINT(WBstate);
      DEBUG_PRINT("  Initialized = ");
      DEBUG_PRINTLN(stateInitialized);

    }
  }
#endif

  // Get time for this loop
  now = millis();

  // Capture the previous state and initialization value for comparison in the next loop.
  if (WBstate != prevState)
      {
        prevState = WBstate;
      }
      if (stateInitialized != prevStateInitialized)
      {
        prevStateInitialized = stateInitialized;
      }

  // Check battery state and goes into POWER_DOWN then LOW_BATT if too low...
  if (now - prevBattReading > 2000)
  {
    prevBattReading = now;

    batt.updateReading();
    battLevel = batt.readBattPercentage();
    if (POWER_MONITORING &&
        WBstate != STATE_POWER_ON_TO_OFF &&
        WBstate != STATE_LOW_BATT &&
        batt.isBattTooLow())
    {
      enterState(STATE_POWER_ON_TO_OFF);
    }
  }

  // LEDS UPDATE
  bool update_leds_chain = false;
  getLEDsSchemeForThisState(); // Get new leds schemes for this loop
  // Update simple LEDs states to last animations schemes.
  panelBarMeter.animations().update(now);
  panelBarMeter.show();
  fireButtonSingleLed.update(now);
  // Optional Power Cell LEDs/BARMETER
// Update optional PowerCell addressable LEDs chain with last color schemes.
#ifdef POWERCELL_EXIST
  getPowerCellScheme();
#if defined(POWERCELL_STRIP) && !POWERCELL_ON_SAME_STRIP
  if (powerCell.update(now, battLevel))
    powercellLeds.show();
#elif defined(POWERCELL_STRIP) && POWERCELL_ON_SAME_STRIP
  {
    update_leds_chain |= powerCell.update(now, battLevel);
  }
#elif defined(POWERCELL_BARMETER)
  powerCellBarMeter.animations().update(now);
#if defined(PCBM_MAX72XX_ON_PBM_CHAIN)
  PBM_driver.show(PCBM_BARDRIVE_DEVICE_INDEX);
#else
  PCBM_driver.show(PCBM_BARDRIVE_DEVICE_INDEX);
#endif
#endif
#endif

  // Update addressable LEDs chain with last color schemes.
  update_leds_chain |= slowBlowIndicator.update(now);
  update_leds_chain |= topWhiteIndicator.update(now);
  update_leds_chain |= topYellowIndicator.update(now);
  update_leds_chain |= frontOrangeIndicator.update(now);
  update_leds_chain |= cyclotron.update(now);
  update_leds_chain |= vent.update(now);
  update_leds_chain |= firingRod.update(now);
  if (update_leds_chain) // Update only if required
    blasterLeds.show();

  // Check buttons and switches readings and states
  PBintensify.update(now);
  SWmain.update(now);
  SWcyclotron.update(now);
  SWactivate.update(now);
  PBfire.update(now);

#ifdef SMOKE_FEATURES_ENABLED
  // Update smoker
  smoker.update(now);
#endif

  // DFPlayer Mini Management
  player.update(now);
  player.setVolWithPot(); // Set audio volume with potentiometer

  // Keep updating engines above, but wait for the DFPlayer command interval
  // before sending another audio command or processing state transitions.
  if (!player.checkCommandDelay())
    return;

  // The player is ready: process the current state's runtime logic.
  handleWristBlasterState();
}
/********************** END_SEQ MAIN LOOP *******************/

///////////////////////////////////////////////////////////////
// Execute the control flow for the current blaster state.
// Phases inside a state coordinate animation and effect transitions.
///////////////////////////////////////////////////////////////
void handleWristBlasterState()
{
  switch (WBstate)
  {

  default:
  case STATE_ZERO:
  {
    DEBUG_PRINTLN("Invalid WBstate... Recovering to POWER-OFF state");
    DEBUG_PRINTLN();
    player.stop();
    enterState(STATE_POWER_OFF);
    break;
  }

  //////////////////////////////////////////////
  case STATE_LOW_BATT:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_LOW_BATT");

      // Specifics state initializers :
      heatLevel = 0; // Reset heat level to 0
      player.stop(); // Stop player, no track for this state
      DEBUG_PRINTLN("Player STOP");
      DEBUG_PRINTLN();

      // Standard initializers
      stateStartTime = now;
      stateInitialized = true; // End state initialization when stateInitialized is 1
      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    // Leave LOW_BATT once the pack voltage has recovered. If cutoff is
    // disabled, isBattTooLow() is false and this state exits safely too.
    if (!batt.isBattTooLow())
    {
      enterState(STATE_POWER_OFF);
      break;
    }
    break;
  }

  //////////////////////////////////////////////
  case STATE_PARTY_MODE:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_PARTY_MODE");

      // Specifics state initializers :
      player.setThemesPlaymode(); // Play files in folder 01 on SD Card
      DEBUG_PRINTLN("Play FOLDER");
      DEBUG_PRINTLN();

      // Standard initializers
      stateStartTime = now;
      stateInitialized = true; // End state initialization when stateInitialized is 1
      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    // Check Fire button to play next/previous themes track, needs a press and release
    checkNextPreviousButton();

    // Wrist blaster state exits by priority : check if wrist blaster goes into PARTY_MODE_OUT
    if (checkIfSwitchExit(!getPartyModeState(), STATE_PARTY_MODE_OUT))
      break;

    break;
  }

  //////////////////////////////////////////////
  case STATE_PARTY_MODE_IN:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_PARTY_MODE_IN");
      DEBUG_PRINTLN();

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1
      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    // Wrist blaster state exits by priority :
    if (checkIfTrackDoneExit(STATE_PARTY_MODE)) // Check if track is ended before going into STATE_PARTY_MODE
      break;
    break;
  }

  //////////////////////////////////////////////
  case STATE_PARTY_MODE_OUT:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_PARTY_MODE_OUT");
      DEBUG_PRINTLN();

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    // Wrist blaster state exits by priority :

    // Determine next state :
    BlasterState next = WBstate;
    if (SWmain.isReleased())
      next = STATE_POWER_OFF;
    else if (SWcyclotron.isReleased())
      next = STATE_POWER_OFF_TO_ON;
    else
      next = SWactivate.isPressed() ? STATE_CYCLOTRON_ON_TO_FULL : STATE_CYCLOTRON_OFF_TO_ON;

    // If track's done, go to next state
    if (checkIfTrackDoneExit(next))
      break;
    break;
  }

  //////////////////////////////////////////////
  case STATE_POWER_OFF:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_POWER_OFF");

#ifdef SMOKE_FEATURES_ENABLED
      smoker.smoke(DISABLE, DISABLE_FAN);
#endif

      // Specifics state initializers :
      heatLevel = 0; // Reset heat level to 0
      player.stop(); // Stop player, no track for this state
      DEBUG_PRINTLN("Player STOP");
      DEBUG_PRINTLN();

      // Standard initializers
      stateStartTime = now;
      stateInitialized = true; // End state initialization when stateInitialized is 1

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    //  Specifics state mechanics :
#ifdef SMOKE_FEATURES_ENABLED
    checkSmokerEnabling(); // Enable/disable smoker with fire button while in POWER OFF state
#endif

    // Wrist blaster state exits by priority :
    if (checkIfSwitchExit(getPartyModeState(), STATE_PARTY_MODE_IN)) // Check if the Intensify Switch is ON, goes into STATE_PARTY_MODE_IN
      break;

    if (checkIfSwitchExit(SWmain.isPressed(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is ON, goes into main booting state STATE_POWER_OFF_TO_ON
      break;

    break;
  }

  //////////////////////////////////////////////
  case STATE_POWER_OFF_TO_ON:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_POWER_OFF_TO_ON");
      DEBUG_PRINTLN();

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    //  Specifics state mechanics :
    heatLevelCooling();

    // Wrist blaster state exits by priority :
    if (checkIfSwitchExit(SWmain.isReleased(), STATE_POWER_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
      break;

    if (checkIfTrackDoneExit(STATE_POWER_ON)) // Check if track is ended before going into STATE_POWER_ON
      break;

    break;
  }

    //////////////////////////////////////////////
  case STATE_POWER_ON_TO_OFF:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_POWER_ON_TO_OFF");
      DEBUG_PRINTLN();

#ifdef SMOKE_FEATURES_ENABLED
      smoker.smoke(DISABLE, DISABLE_FAN);
#endif

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

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
    if (checkIfSwitchExit(SWmain.isPressed(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is ON, goes into main booting state STATE_POWER_OFF_TO_ON
      break;

    // If there is no restart request, complete the regular shutdown path.
    if (checkIfTrackDoneExit(STATE_POWER_OFF)) // Check if track is ended before going into STATE_POWER_OFF
      break;
#endif
    break;
  }

  //////////////////////////////////////////////
  case STATE_POWER_ON:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_POWER_ON");
      DEBUG_PRINTLN();

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    //  Specifics state mechanics :
    heatLevelCooling();

    // Wrist blaster state exits by priority :
    if (checkIfSwitchExit(getPartyModeState(), STATE_PARTY_MODE_IN)) // Check if the Intensify Switch is ON, goes into STATE_PARTY_MODE_IN
      break;

    if (checkIfSwitchExit(SWmain.isReleased(), STATE_POWER_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_POWER_ON_TO_OFF
      break;

    if (checkIfSwitchExit(SWcyclotron.isPressed(), SWactivate.isPressed() ? STATE_CYCLOTRON_ON_TO_FULL : STATE_CYCLOTRON_OFF_TO_ON)) // Check if Cyclotron Switch is ON, goes into cyclotron booting state STATE_CYCLOTRON_OFF_TO_ON
      break;

    break;
  }

    //////////////////////////////////////////////
  case STATE_CYCLOTRON_OFF_TO_ON:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CYCLOTRON_OFF_TO_ON");
      DEBUG_PRINTLN();

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    //  Specifics state mechanics :
    heatLevelCooling();

    // Wrist blaster state exits by priority :
    if (checkIfSwitchExit(SWmain.isReleased(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
      break;

    if (checkIfSwitchExit(SWcyclotron.isReleased(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
      break;

    if (checkIfSwitchExit(SWactivate.isPressed(), STATE_CYCLOTRON_ON_TO_FULL)) // Activate switch is ON: ramp the cyclotron to FULL power
      break;

    if (checkIfTrackDoneExit(STATE_CYCLOTRON_ON)) // Check if track is ended before going into STATE_CYCLOTRON_ON
      break;

    break;
  }

  //////////////////////////////////////////////
  case STATE_CYCLOTRON_ON_TO_OFF:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CYCLOTRON_ON_TO_OFF");
      DEBUG_PRINTLN();

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    //  Specifics state mechanics :
    heatLevelCooling();

    // Wrist blaster state exits by priority :
    if (checkIfSwitchExit(SWmain.isReleased(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
      break;

    if (checkIfSwitchExit(SWcyclotron.isPressed(), STATE_CYCLOTRON_OFF_TO_ON)) // Check if Cyclotron Switch is ON, goes into cyclotron booting state STATE_CYCLOTRON_OFF_TO_ON
      break;

    if (checkIfTrackDoneExit(STATE_POWER_ON)) // Check if track is ended before going into STATE_POWER_ON witch cyclotron OFF
      break;

    break;
  }

  //////////////////////////////////////////////
  case STATE_CYCLOTRON_ON:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CYCLOTRON_ON");
      DEBUG_PRINTLN();

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1

      DEBUG_PRINTLN();
      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    //  Specifics state mechanics :
    heatLevelCooling();

    // Wrist blaster state exits by priority :
    if (checkIfSwitchExit(getPartyModeState(), STATE_PARTY_MODE_IN)) // Check if the Intensify Switch is ON, goes into STATE_PARTY_MODE_IN
      break;

    if (checkIfSwitchExit(SWmain.isReleased(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
      break;

    if (checkIfSwitchExit(SWcyclotron.isReleased(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
      break;

    if (checkIfSwitchExit(SWactivate.isPressed(), STATE_CYCLOTRON_ON_TO_FULL)) // Check if Cyclotron Activate Switch is ON, goes into cyclotron booting FULL state STATE_CYCLOTRON_ON_TO_FULL
      break;

    if (checkIfSwitchExit(PBfire.isPressed(), STATE_CAPTURE))
      break;
    break;
  }

  //////////////////////////////////////////////
  case STATE_CYCLOTRON_ON_TO_FULL:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CYCLOTRON_ON_TO_FULL");
      DEBUG_PRINTLN();

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    //  Specifics state mechanics :
    heatLevelCooling();

    // Wrist blaster state exits by priority :
    if (checkIfSwitchExit(SWmain.isReleased(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
      break;

    if (checkIfSwitchExit(SWcyclotron.isReleased(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
      break;

    if (checkIfSwitchExit(SWactivate.isReleased(), STATE_CYCLOTRON_FULL_TO_ON)) // Check if Cyclotron Activate Switch is OFF, goes into cyclotron returning to normal STATE_CYCLOTRON_FULL_TO_ON
      break;

    if (checkIfTrackDoneExit(STATE_CYCLOTRON_FULL_POWER)) // Check if track is ended before going into STATE_CYCLOTRON_FULL_POWER
      break;

    break;
  }

  //////////////////////////////////////////////
  case STATE_CYCLOTRON_FULL_TO_ON:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CYCLOTRON_FULL_TO_ON");
      DEBUG_PRINTLN();

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    //  Specifics state mechanics :
    heatLevelCooling();

    // Wrist blaster state exits by priority :
    if (checkIfSwitchExit(SWmain.isReleased(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
      break;

    if (checkIfSwitchExit(SWcyclotron.isReleased(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
      break;

    if (checkIfSwitchExit(SWactivate.isPressed(), STATE_CYCLOTRON_ON_TO_FULL)) /// Check if Cyclotron Activate Switch is ON, goes into cyclotron booting FULL state STATE_CYCLOTRON_ON_TO_FULL
      break;

    if (checkIfTrackDoneExit(STATE_CYCLOTRON_ON)) // Check if track is ended before going into STATE_CYCLOTRON_ON
      break;
    break;
  }

  //////////////////////////////////////////////
  case STATE_CYCLOTRON_FULL_POWER:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CYCLOTRON_FULL_POWER");
      DEBUG_PRINTLN();

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    //  Specifics state mechanics :
    heatLevelCooling();

    // Wrist blaster state exits by priority :
    if (checkIfSwitchExit(getPartyModeState(), STATE_PARTY_MODE_IN)) // Check if the Intensify Switch is ON, goes into STATE_PARTY_MODE_IN
      break;

    if (checkIfSwitchExit(SWmain.isReleased(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
      break;

    if (checkIfSwitchExit(SWcyclotron.isReleased(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OFF
      break;

    if (checkIfSwitchExit(SWactivate.isReleased(), STATE_CYCLOTRON_FULL_TO_ON)) // Check if Cyclotron Activate Switch is OFF, goes into cyclotron returning to normal STATE_CYCLOTRON_FULL_TO_ON
      break;

    // Floating-point threshold division replaced by an equivalent integer cross-multiplication.
    if (checkIfSwitchExit(PBfire.isPressed(),
                          ((uint32_t)heatLevel * MAX_BURST_SHOTS < (uint32_t)(MAX_BURST_SHOTS - 1) * 100U)
                              ? STATE_BURST
                              : STATE_BURST_OVERHEAT))
      break;

    break;
  }

  //////////////////////////////////////////////
  case STATE_CAPTURE:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CAPTURE");
      DEBUG_PRINTLN();

      //  Specifics initializations :
      fireType = CAPTURE; // tail and reboot to STATE_CYCLOTRON_ON

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    //  Specifics state mechanics :
    heatLevelRisingCapture();

    // Wrist blaster state exits by priority :
    if (checkIfSwitchExit(SWmain.isReleased(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
      break;

    if (checkIfSwitchExit(SWcyclotron.isReleased(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
      break;

    if (checkIfSwitchExit(SWactivate.isPressed(), STATE_CYCLOTRON_ON_TO_FULL)) // Activate switch is ON: leave Capture and ramp to FULL power
      break;

    // Wrist blaster going into tail before overheat warning:
    if (checkIfSwitchExit(PBfire.isReleased(), STATE_CAPTURE_TAIL))
      break;

    // Handle capture warning transition when heat level is getting high
    if (now - stateStartTime >= DURATION_CAPTURE_TAKEOFF_RAMP &&
        heatLevel > getCaptureScaledDuration())
    {
      enterState(STATE_CAPTURE_OVERHEAT);
      break;
    }
    break;
  }

  //////////////////////////////////////////////
  case STATE_CAPTURE_OVERHEAT:
  {
    if (!stateInitialized) // WARNING initialisation :
    {
      DEBUG_PRINTLN("STATE_CAPTURE_OVERHEAT");

      //  Specifics initializations :
      fireType = CAPTURE; // tail and reboot to STATE_CYCLOTRON_ON

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1
      phaseStartTime = now;
#ifdef SMOKE_FEATURES_ENABLED
      smoker.smoke(ENABLE); // Put the smoke and pump on, but not the fan
#endif
      phaseInitialized = true;
      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    switch (overheatPhase)
    {
    case PHASE_WARNING:
    {
      if (!phaseInitialized)
      {
        DEBUG_PRINTLN("PHASE_WARNING");
        phaseStartTime = now;    // Record the start time of the phase
        phaseInitialized = true; // Mark the phase as initialized
      }

      //  Specifics state mechanics :
      heatLevelRisingCapture();

      // Wrist blaster state exits by priority :

      if (checkIfSwitchExit(SWmain.isReleased(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
        break;

      if (checkIfSwitchExit(SWcyclotron.isReleased(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
        break;

      if (checkIfSwitchExit(SWactivate.isPressed(), STATE_CYCLOTRON_ON_TO_FULL)) // Activate switch is ON: leave the warning and ramp to FULL power
        break;

      // Wrist blaster going into tail before overheat warning:
      if (checkIfSwitchExit(PBfire.isReleased(), STATE_CAPTURE_TAIL))
      {
#ifdef SMOKE_FEATURES_ENABLED
        smoker.smoke(DISABLE, DISABLE_FAN);
#endif
        break;
      }

      if (now - phaseStartTime >= CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_WARNING])
      {
        overheatPhase = PHASE_OVERHEAT; // Start with the overheat phase
        phaseInitialized = false;       // Next phase needs initialization
        break;
      }
      break;
    }

    case PHASE_OVERHEAT:
    {
      if (!phaseInitialized)
      {
        DEBUG_PRINTLN("PHASE_OVERHEAT");
        // Specifics initializations :
#ifdef SMOKE_FEATURES_ENABLED
        smoker.startBurst(CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT], WITH_FAN); // Run smoke, pump, and fan for the overheat phase
#endif
        phaseStartTime = now;    // Record the start time of the phase
        phaseInitialized = true; // Mark the phase as initialized
      }

      if (now - phaseStartTime >= CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT])
      {
        overheatPhase = PHASE_COOLING; // Start with the cooling phase
        phaseInitialized = false;      // Next phase needs initialization
        break;
      }
      break;
    }

    case PHASE_COOLING:
    {
      if (!phaseInitialized)
      {
        DEBUG_PRINTLN("PHASE_COOLING");
        DEBUG_PRINTLN();
        phaseStartTime = now;    // Record the start time of the phase
        phaseInitialized = true; // Mark the phase as initialized
      }
      //  Specifics state mechanics :
      heatLevel = 0; // Venting and cooling done...

      // Determine next state
      BlasterState next = SWmain.isReleased()        ? STATE_POWER_ON_TO_OFF
                          : SWcyclotron.isReleased() ? STATE_POWER_ON // Cyclotron switch is off, goes into POWER ON
                          : SWactivate.isPressed()   ? STATE_CYCLOTRON_ON_TO_FULL
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
      DEBUG_PRINTLN("WRONG PHASE, recovering to PHASE_WARNING");
      DEBUG_PRINTLN();
      overheatPhase = PHASE_WARNING;
      phaseInitialized = false;
      break;
    }
    } // END of OVERHEAT PHASES SWITCH/CASES

    break;
  } // End of STATE_CAPTURE_OVERHEAT case

  //////////////////////////////////////////////
  case STATE_CAPTURE_TAIL:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_CAPTURE_TAIL");
      DEBUG_PRINTLN();

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    //  Specifics state mechanics :
    heatLevelCooling();

    // Wrist blaster state exits by priority :
    if (checkIfSwitchExit(SWmain.isReleased(), STATE_ALL_ON_TO_OFF)) // Check if Main Switch is OFF, goes into main shutting state STATE_ALL_ON_TO_OFF
      break;

    // if (checkIfSwitchExit(SWcyclotron.isReleased(), STATE_CYCLOTRON_ON_TO_OFF)) // Check if Cyclotron Switch is OFF, goes into cyclotron shutting state STATE_CYCLOTRON_ON_TO_OF
    //   break;

    // Check if tail track is done
    if (!player.isPlaying()) // || (now - stateStartTime) >= TRACK_LENGTH[WBstate])
    {
      // When the tail ends, return to regular or full cyclotron power according to the firing type.
      enterState(fireType ? STATE_CYCLOTRON_FULL_POWER : STATE_CYCLOTRON_ON);
      break;
    }

    // Check if Firing Button pushed, goes into capture firing ramp state STATE_CAPTURE
    if (checkIfSwitchExit(PBfire.isPressed(),
                          (heatLevel < getCaptureScaledDuration())
                              ? STATE_CAPTURE
                              : STATE_CAPTURE_OVERHEAT))
      break;
    break;
  }

    //////////////////////////////////////////////
  case STATE_BURST:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_BURST");
      DEBUG_PRINTLN();

      //  Specifics initializations :
      fireType = BURST; // tail and reboot to STATE_CYCLOTRON_FULL_POWER

      //  Specifics state mechanics :
      heatLevelRisingBurst(); // increment heatLevel for this shot

      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1
      phaseStartTime = now;
      phaseInitialized = true;

      // Wait for the DFPlayer command delay before running this state.
      break;
    }

    switch (burstPhase)
    {
    case BURST_PHASE_FIRING:
    {
      if (!phaseInitialized)
      {
        DEBUG_PRINTLN("PHASE_FIRING");
        phaseStartTime = now;
        phaseInitialized = true;
      }

      if (checkIfSwitchExit(SWmain.isReleased(), STATE_ALL_ON_TO_OFF))
        break;
      if (checkIfSwitchExit(SWcyclotron.isReleased(), STATE_CYCLOTRON_ON_TO_OFF))
        break;
      if (checkIfSwitchExit(SWactivate.isReleased(), STATE_CYCLOTRON_FULL_TO_ON))
        break;
      if (now - phaseStartTime >= BURST_PHASE_LENGTH[BURST_PHASE_FIRING])
      {
        burstPhase = BURST_PHASE_TAIL;
        phaseInitialized = false;
      }
      break;
    }
    case BURST_PHASE_TAIL:
    {
      if (!phaseInitialized)
      {
        DEBUG_PRINTLN("PHASE_TAIL");
        DEBUG_PRINTLN();
        phaseStartTime = now;
        phaseInitialized = true;
      }

      if (checkIfSwitchExit(SWmain.isReleased(), STATE_ALL_ON_TO_OFF))
        break;
      if (!player.isPlaying())
      {
        enterState(fireType ? STATE_CYCLOTRON_FULL_POWER : STATE_CYCLOTRON_ON);
        break;
      }
      if (PBfire.isPressed())
      {
        enterState(((uint32_t)heatLevel * MAX_BURST_SHOTS < (uint32_t)(MAX_BURST_SHOTS - 1) * 100U) ? STATE_BURST : STATE_BURST_OVERHEAT);
      }
      break;
    }
    default:
      DEBUG_PRINTLN("WRONG PHASE, resetting to FIRING phase");
      DEBUG_PRINTLN();
      burstPhase = BURST_PHASE_FIRING;
      phaseInitialized = false;
      break;
    }
    break;
  }

  //////////////////////////////////////////////
  case STATE_BURST_OVERHEAT:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
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
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1
      phaseStartTime = now;
      phaseInitialized = true;

      // Wait for the DFPlayer command delay before running this state.
      break;
    }
    switch (overheatPhase)
    {
    case PHASE_WARNING:
    {
      if (!phaseInitialized)
      {
        DEBUG_PRINTLN("PHASE_WARNING");
        phaseStartTime = now;
        phaseInitialized = true;
      }

      if (checkIfSwitchExit(SWmain.isReleased(), STATE_ALL_ON_TO_OFF))
        break;

      if (checkIfSwitchExit(SWcyclotron.isReleased(), STATE_CYCLOTRON_ON_TO_OFF))
        break;

      if (checkIfSwitchExit(SWactivate.isReleased(), STATE_CYCLOTRON_FULL_TO_ON))
        break;

      if (now - phaseStartTime >= BURST_OVERHEAT_PHASE_LENGTH[PHASE_WARNING])
      {
        overheatPhase = PHASE_OVERHEAT;
        phaseInitialized = false;
      }
      break;
    }

    case PHASE_OVERHEAT:
    {
      if (!phaseInitialized)
      {
        DEBUG_PRINTLN("PHASE_OVERHEAT");
#ifdef SMOKE_FEATURES_ENABLED
        smoker.startBurst(BURST_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT], WITH_FAN);
#endif
        phaseStartTime = now;
        phaseInitialized = true;
      }

      if (now - phaseStartTime >= BURST_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT])
      {
        overheatPhase = PHASE_COOLING;
        phaseInitialized = false;
      }
      break;
    }

    case PHASE_COOLING:
    {
      if (!phaseInitialized)
      {
        DEBUG_PRINTLN("PHASE_COOLING");
        DEBUG_PRINTLN();
        phaseStartTime = now;
        phaseInitialized = true;
      }
      heatLevel = 0;
      const BlasterState next = SWmain.isReleased()        ? STATE_POWER_ON_TO_OFF
                                : SWcyclotron.isReleased() ? STATE_POWER_ON
                                : SWactivate.isPressed()   ? STATE_CYCLOTRON_ON_TO_FULL
                                                           : STATE_CYCLOTRON_OFF_TO_ON;
      if (checkIfTrackDoneExit(next))
      {
#ifdef SMOKE_FEATURES_ENABLED
        smoker.smoke(DISABLE, DISABLE_FAN);
#endif
      }
      break;
    }

    default:
    {
      DEBUG_PRINTLN("WRONG PHASE, recovering to PHASE_WARNING");
      DEBUG_PRINTLN();
      overheatPhase = PHASE_WARNING;
      phaseInitialized = false;
      break;
    }
    }
    break;
  }

  //////////////////////////////////////////////
  case STATE_ALL_ON_TO_OFF:
  {
    if (!stateInitialized) // Initiate this wrist blaster State :
    {
      DEBUG_PRINTLN("STATE_ALL_ON_TO_OFF");
      DEBUG_PRINTLN();
#ifdef SMOKE_FEATURES_ENABLED
      smoker.smoke(DISABLE, DISABLE_FAN);
#endif
      // Standard initializers
      stateInitialized = stateInitialization(); // End state initialization when stateInitialized is 1
      // Wait for the DFPlayer command delay before running this state.
      break;
    }

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
    if (checkIfSwitchExit(SWmain.isPressed(), STATE_POWER_OFF_TO_ON)) // Check if Main Switch is ON, goes into main booting state STATE_POWER_OFF_TO_ON
      break;

    // If there is no restart request, complete the regular shutdown path.
    if (checkIfTrackDoneExit(STATE_POWER_OFF)) // Check if track is ended before going into STATE_POWER_OFF
      break;
#endif
    break;
  }
  }
}

//////////////////////////////////////////////////////////////////////////
///////////////////// *** HELPER FUNCTIONS *** ///////////////////////////
//////////////////////////////////////////////////////////////////////////

/*******************************************/
/*   ANIMATIONS functions in wrist blaster states   */
/*******************************************/
// Scheme functions run before the main state handler on every loop. While an
// initialization flag is false, each device configures its new state or phase
// effect and stops there. Keep these guards and their break even when no device
// setup is required: they make every scheme wait for the main state machine to
// complete the shared transition, keeping all effects synchronized. Runtime
// animation updates begin on the next loop.
void getLEDsSchemeForThisState()
{
  getIndicatorScheme();
  getVentScheme();
  getPanelBarMeterScheme();
  getCyclotronScheme();
  getFiringRodScheme();
}

void getIndicatorScheme()
{
  getSlowBlowIndicatorScheme();
  getTopWhiteIndicatorScheme();
  getTopYellowIndicatorScheme();
  getFrontOrangeIndicatorScheme();
  getFireButtonIndicatorScheme();
}

void getSlowBlowIndicatorScheme()
{
  switch (WBstate)
  {
  case STATE_ZERO:
  case STATE_LOW_BATT:
  case STATE_POWER_OFF:
  {
    if (!stateInitialized)
    {
      slowBlowIndicator.clear();
      break;
    }
    break;
  }

  case STATE_PARTY_MODE:
  case STATE_PARTY_MODE_IN:
  case STATE_PARTY_MODE_OUT:
  case STATE_POWER_ON:
  case STATE_CYCLOTRON_OFF_TO_ON:
  case STATE_CYCLOTRON_ON_TO_OFF:
  case STATE_CYCLOTRON_ON:
  case STATE_CYCLOTRON_ON_TO_FULL:
  case STATE_CYCLOTRON_FULL_TO_ON:
  case STATE_CYCLOTRON_FULL_POWER:
  case STATE_CAPTURE:
  case STATE_CAPTURE_TAIL:
  {
    if (!stateInitialized)
    {
      slowBlowIndicator.initParam(RED, 100);
      break;
    }
    slowBlowIndicator.solid();
    break;
  }

  case STATE_POWER_OFF_TO_ON:
  {
    if (!stateInitialized)
    {
      slowBlowIndicator.initParam(RED, 100, FAST_BLINK_SP);
      break;
    }
    slowBlowIndicator.blink(DISABLE);
    break;
  }

  case STATE_POWER_ON_TO_OFF:
  case STATE_ALL_ON_TO_OFF:
  {
    if (!stateInitialized)
    {
      uint16_t duration = getDuration();
      slowBlowIndicator.initParam(BLACK, 100, SOLID, duration);
      break;
    }
    slowBlowIndicator.ramp();
    break;
  }

  case STATE_BURST:
  {
    switch (burstPhase)
    {
    case BURST_PHASE_FIRING:
      if (!phaseInitialized)
      {
        slowBlowIndicator.initParam(RED, 100);
        break;
      }
      slowBlowIndicator.solid();
      break;

    case BURST_PHASE_TAIL:
      if (!phaseInitialized)
      {
        slowBlowIndicator.initParam(RED, 100);
        break;
      }
      slowBlowIndicator.solid();
      break;
    }
    break;
  }

  case STATE_CAPTURE_OVERHEAT:
  case STATE_BURST_OVERHEAT:
  {
    if (!stateInitialized)
    {
      slowBlowIndicator.initParam(RED, 100);
      break;
    }
    if (!phaseInitialized)
      break;
    slowBlowIndicator.solid();
    break;
  }
  }
}

void getTopWhiteIndicatorScheme()
{
  switch (WBstate)
  {
  case STATE_ZERO:
  case STATE_POWER_OFF_TO_ON:
  {
    if (!stateInitialized)
    {
      topWhiteIndicator.clear();
      break;
    }
    break;
  }

  case STATE_LOW_BATT:
  {
    if (!stateInitialized)
    {
      topWhiteIndicator.initParam(RED, 50, MEDIUM_BLINK_SP);
      break;
    }
    topWhiteIndicator.blink(DISABLE);
    break;
  }

  case STATE_PARTY_MODE:
  {
    if (!stateInitialized)
    {
      topWhiteIndicator.initParam(WHITE, 75);
      break;
    }
    topWhiteIndicator.flash(517); // 116 bpm : 60 000 / 116 = 517ms
    break;
  }

  case STATE_PARTY_MODE_IN:
  case STATE_PARTY_MODE_OUT:
  {
    if (!stateInitialized)
    {
      topWhiteIndicator.initParam(WHITE, 75, FAST_BLINK_SP);
      break;
    }
    topWhiteIndicator.blink(DISABLE);
    break;
  }

  case STATE_POWER_OFF:
    // the indicator behavior when the device is powered off
    // It flashes to show that the prop is still powered from the battery.
    // Flash color also indicate if smoke device is enabled : RED = ENABLE, GREEN = DISABLE
    {
      if (!stateInitialized)
      {
#ifdef SMOKE_FEATURES_ENABLED
        topWhiteIndicator.initParam(smoker.enable() ? GREEN : RED, 50);
#else
        topWhiteIndicator.initParam(GREEN, 50);
#endif
        break;
      }
      if (PBfire.isReleased())
        topWhiteIndicator.flash(5000); // set LED_INDEX_TOP_WHITE led green flashing
      break;
    }

  case STATE_POWER_ON_TO_OFF:
  case STATE_ALL_ON_TO_OFF:
  {
    if (!stateInitialized)
    {
      uint16_t duration = getDuration();
      topWhiteIndicator.initParam(WHITE, 0, SOLID, duration);
      break;
    }
    topWhiteIndicator.ramp();
    break;
  }

  case STATE_POWER_ON:
  {
    if (!stateInitialized)
    {
      topWhiteIndicator.initParam(WHITE, 75, SLOW_BLINK_SP);
      break;
    }
    topWhiteIndicator.blink(DISABLE);
    break;
  }

  case STATE_CYCLOTRON_OFF_TO_ON:
  {
    if (!stateInitialized)
    {
      topWhiteIndicator.initParam(WHITE, 75, MEDIUM_BLINK_SP, getDuration());
      frontOrangeIndicator.initParam(ORANGE, 100, MEDIUM_BLINK_SP, topWhiteIndicator.getPrevBlink(), topWhiteIndicator.getPulse());
      break;
    }
    topWhiteIndicator.blink(ENABLE_RAMP);
    break;
  }

  case STATE_CYCLOTRON_ON_TO_OFF:
  {
    if (!stateInitialized)
    {
      uint16_t duration = getDuration();
      topWhiteIndicator.initParam(WHITE, 75, SLOW_BLINK_SP, duration);
      break;
    }
    topWhiteIndicator.blink(ENABLE_RAMP);
    break;
  }

  case STATE_CYCLOTRON_ON:
  case STATE_CAPTURE:
  case STATE_CAPTURE_TAIL:
  {
    if (!stateInitialized)
    {
      topWhiteIndicator.initParam(WHITE, 75, MEDIUM_BLINK_SP);
      break;
    }
    topWhiteIndicator.blink(DISABLE_RAMP);
    break;
  }

  case STATE_CYCLOTRON_ON_TO_FULL:
  {
    if (!stateInitialized)
    {
      topWhiteIndicator.initParam(WHITE, 75, FAST_BLINK_SP, getDuration());
      break;
    }
    topWhiteIndicator.blink(ENABLE_RAMP);
    break;
  }

  case STATE_CYCLOTRON_FULL_TO_ON:
  {
    if (!stateInitialized)
    {
      topWhiteIndicator.initParam(WHITE, 75, MEDIUM_BLINK_SP, getDuration());
      break;
    }
    topWhiteIndicator.blink(ENABLE_RAMP);
    break;
  }

  case STATE_CYCLOTRON_FULL_POWER:
  {
    if (!stateInitialized)
    {
      topWhiteIndicator.initParam(WHITE, 75, FAST_BLINK_SP);
      break;
    }
    topWhiteIndicator.blink(DISABLE_RAMP);
    break;
  }

  case STATE_BURST:
  {
    if (!stateInitialized)
    {
      topWhiteIndicator.initParam(WHITE, 75, FAST_BLINK_SP);
      break;
    }
    if (!phaseInitialized)
      break;
    topWhiteIndicator.blink(DISABLE_RAMP);
    break;
  }

  case STATE_CAPTURE_OVERHEAT:
  case STATE_BURST_OVERHEAT:
  {
    if (!stateInitialized)
    {
      topWhiteIndicator.initParam(WHITE, 75, MEDIUM_BLINK_SP);
      break;
    }
    if (!phaseInitialized)
      break;
    topWhiteIndicator.blink(DISABLE_RAMP);
    break;
  }
  }
}

void getTopYellowIndicatorScheme()
{
  switch (WBstate)
  {
  case STATE_ZERO:
  case STATE_LOW_BATT:
  case STATE_POWER_OFF:
  case STATE_POWER_OFF_TO_ON:
  {
    if (!stateInitialized)
    {
      topYellowIndicator.clear();
      break;
    }
    break;
  }

  case STATE_PARTY_MODE:
  case STATE_PARTY_MODE_IN:
  case STATE_PARTY_MODE_OUT:
  case STATE_CYCLOTRON_OFF_TO_ON:
  case STATE_CYCLOTRON_ON_TO_OFF:
  case STATE_CYCLOTRON_ON:
  case STATE_CYCLOTRON_ON_TO_FULL:
  case STATE_CYCLOTRON_FULL_TO_ON:
  case STATE_CYCLOTRON_FULL_POWER:
  case STATE_CAPTURE:
  case STATE_CAPTURE_TAIL:
  {
    if (!stateInitialized)
    {
      topYellowIndicator.initParam(YELLOW, 100);
      break;
    }
    topYellowIndicator.solid();
    break;
  }

  case STATE_POWER_ON_TO_OFF:
  case STATE_ALL_ON_TO_OFF:
  {
    if (!stateInitialized)
    {
      uint16_t duration = getDuration();
      topYellowIndicator.initParam(BLACK, 100, SOLID, duration);
      break;
    }
    topYellowIndicator.ramp();
    break;
  }

  case STATE_POWER_ON:
  {
    if (!stateInitialized)
    {
      topYellowIndicator.initParam(YELLOW, 255);
      break;
    }
    topYellowIndicator.solid();
    break;
  }

  case STATE_BURST:
  {
    if (!stateInitialized)
    {
      topYellowIndicator.initParam(YELLOW, 100);
      break;
    }
    if (!phaseInitialized)
      break;
    topYellowIndicator.solid();
    break;
  }

  case STATE_CAPTURE_OVERHEAT:
  case STATE_BURST_OVERHEAT:
  {
    if (!stateInitialized)
    {
      topYellowIndicator.initParam(YELLOW, 100, FAST_BLINK_SP, topWhiteIndicator.getPrevBlink(), topWhiteIndicator.getPulse());
      break;
    }
    if (!phaseInitialized)
      break;
    topYellowIndicator.blink(DISABLE_RAMP);
    break;
  }
  }
}

void getFrontOrangeIndicatorScheme()
{
  switch (WBstate)
  {
  case STATE_ZERO:
  case STATE_LOW_BATT:
  case STATE_POWER_OFF:
  case STATE_POWER_OFF_TO_ON:
  case STATE_PARTY_MODE:
  case STATE_PARTY_MODE_IN:
  case STATE_PARTY_MODE_OUT:
  case STATE_POWER_ON_TO_OFF:
  case STATE_POWER_ON:
  case STATE_ALL_ON_TO_OFF:
  {
    if (!stateInitialized)
    {
      frontOrangeIndicator.clear();
      break;
    }
    break;
  }

  case STATE_CYCLOTRON_OFF_TO_ON:
  {
    if (!stateInitialized)
    {
      frontOrangeIndicator.initParam(ORANGE, 100, MEDIUM_BLINK_SP, topWhiteIndicator.getPrevBlink(), topWhiteIndicator.getPulse());
      break;
    }
    frontOrangeIndicator.blink(DISABLE_RAMP);
    break;
  }

  case STATE_CYCLOTRON_ON_TO_OFF:
  {
    if (!stateInitialized)
    {
      uint16_t duration = getDuration();
      frontOrangeIndicator.initParam(ORANGE, 0, SOLID, duration);
      break;
    }
    frontOrangeIndicator.ramp();
    break;
  }

  case STATE_CYCLOTRON_ON:
  case STATE_CYCLOTRON_ON_TO_FULL:
  case STATE_CYCLOTRON_FULL_TO_ON:
  case STATE_CYCLOTRON_FULL_POWER:
  case STATE_CAPTURE:
  case STATE_CAPTURE_TAIL:
  {
    if (!stateInitialized)
    {
      frontOrangeIndicator.initParam(ORANGE, 100);
      break;
    }
    frontOrangeIndicator.solid();
    break;
  }

  case STATE_BURST:
  {
    if (!stateInitialized)
    {
      frontOrangeIndicator.initParam(ORANGE, 100);
      break;
    }
    if (!phaseInitialized)
      break;
    frontOrangeIndicator.solid();
    break;
  }

  case STATE_CAPTURE_OVERHEAT:
  case STATE_BURST_OVERHEAT:
  {
    switch (overheatPhase)
    {
    case PHASE_WARNING:
      if (!phaseInitialized)
      {
        frontOrangeIndicator.initParam(ORANGE, 100);
        break;
      }
      frontOrangeIndicator.solid();
      break;
    case PHASE_OVERHEAT:
      if (!phaseInitialized)
      {
        if (WBstate == STATE_CAPTURE_OVERHEAT)
          frontOrangeIndicator.initParam(ORANGE,
                                         0,
                                         SOLID,
                                         CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT] + CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_COOLING]);
        else
          frontOrangeIndicator.initParam(ORANGE,
                                         0,
                                         SOLID,
                                         BURST_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT] + BURST_OVERHEAT_PHASE_LENGTH[PHASE_COOLING]);
        break;
      }
      frontOrangeIndicator.ramp();
      break;
    case PHASE_COOLING:
      if (!phaseInitialized)
        break;
      frontOrangeIndicator.ramp();
      break;
    }
    break;
  }
  }
}

void getFireButtonIndicatorScheme()
{
  switch (WBstate)
  {
  case STATE_ZERO:
  case STATE_LOW_BATT:
  case STATE_POWER_OFF:
  case STATE_POWER_OFF_TO_ON:
  case STATE_POWER_ON_TO_OFF:
  case STATE_POWER_ON:
  case STATE_ALL_ON_TO_OFF:
  {
    if (!stateInitialized)
    {
      fireButtonSingleLed.clear();
      break;
    }
    break;
  }

  case STATE_PARTY_MODE:
  case STATE_CYCLOTRON_ON:
  case STATE_CYCLOTRON_FULL_POWER:
  {
    if (!stateInitialized)
    {
      fireButtonSingleLed.on();
      break;
    }
    break;
  }

  case STATE_PARTY_MODE_IN:
  case STATE_PARTY_MODE_OUT:
  {
    if (!stateInitialized)
    {
      fireButtonSingleLed.blinkInit(MEDIUM_BLINK_SP);
      break;
    }
    fireButtonSingleLed.blink();
    break;
  }

  case STATE_CYCLOTRON_OFF_TO_ON:
  case STATE_CYCLOTRON_ON_TO_OFF:
  case STATE_CYCLOTRON_ON_TO_FULL:
  case STATE_CYCLOTRON_FULL_TO_ON:
  case STATE_CAPTURE_TAIL:
  {
    if (!stateInitialized)
    {
      fireButtonSingleLed.blinkInit(FAST_BLINK_SP);
      break;
    }
    fireButtonSingleLed.blink();
    break;
  }

  case STATE_CAPTURE:
  {
    if (!stateInitialized)
    {
      fireButtonSingleLed.off();
      break;
    }
    break;
  }

  case STATE_BURST:
  {
    switch (burstPhase)
    {
    case BURST_PHASE_FIRING:
      if (!phaseInitialized)
      {
        fireButtonSingleLed.off();
        break;
      }
      break;
    case BURST_PHASE_TAIL:
      if (!phaseInitialized)
      {
        fireButtonSingleLed.blinkInit(FAST_BLINK_SP);
        break;
      }
      fireButtonSingleLed.blink();
      break;
    }
    break;
  }

  case STATE_CAPTURE_OVERHEAT:
  case STATE_BURST_OVERHEAT:
  {
    switch (overheatPhase)
    {
    case PHASE_WARNING:
      if (!phaseInitialized)
      {
        fireButtonSingleLed.off();
        break;
      }
      break;
    case PHASE_OVERHEAT:
      if (!phaseInitialized)
        break;
      break;

    case PHASE_COOLING:
      if (!phaseInitialized)
      {
        fireButtonSingleLed.blinkInit(FAST_BLINK_SP);
        break;
      }
      fireButtonSingleLed.blink();
      break;
    }
    break;
  }
  }
}

void getVentScheme()
{
  // Some sequences need to be initialized with wrist blaster state, stateInitialized is used to know if wrist blaster state is in initialization

  switch (WBstate)
  {
  case STATE_ZERO:
  case STATE_LOW_BATT:
  case STATE_POWER_OFF:
  case STATE_POWER_OFF_TO_ON:
  case STATE_POWER_ON:
  case STATE_PARTY_MODE:
  case STATE_PARTY_MODE_IN:
  case STATE_PARTY_MODE_OUT:
  {
    if (!stateInitialized)
    {
      vent.clear();
      break;
    }
    break;
  }

  case STATE_POWER_ON_TO_OFF:
  case STATE_ALL_ON_TO_OFF:
  {
    if (!stateInitialized)
    {
      vent.initParam(WARM_WHITE, 0, 2000);
      break;
    }
    vent.ramp();
    break;
  }

  case STATE_CYCLOTRON_OFF_TO_ON:
  case STATE_CYCLOTRON_FULL_TO_ON:
  {
    if (!stateInitialized)
    {
      vent.initParam(WARM_WHITE, 50, getDuration());
      break;
    }
    vent.ramp(); // White, not at full brightness
    break;
  }

  case STATE_CYCLOTRON_ON_TO_OFF:
  {
    if (!stateInitialized)
    {
      vent.initParam(WARM_WHITE, 0, getDuration());
      break;
    }
    vent.ramp();
    break;
  }

  case STATE_CYCLOTRON_ON:
  {
    if (!stateInitialized)
    {
      vent.initParam(WARM_WHITE, 50); // finishing fade if not done
      break;
    }
    vent.solid(); // finishing fade if not done
    break;
  }

  case STATE_CYCLOTRON_ON_TO_FULL:
  {
    if (!stateInitialized)
    {
      vent.initParam(WARM_WHITE, 75, getDuration()); // finishing fade if not done
      break;
    }
    vent.ramp(); // finishing fade if not done
    break;
  }

  case STATE_CYCLOTRON_FULL_POWER:
  {
    if (!stateInitialized)
    {
      vent.initParam(WARM_WHITE, 75); // finishing fade if not done
      break;
    }
    vent.solid(); // finishing fade if not done
    break;
  }

  case STATE_CAPTURE:
  {
    if (!stateInitialized)
    {
      vent.initParam(ORANGE, 50, getDuration());
      break;
    }
    vent.flicker(25, 25);
    break;
  }

  case STATE_CAPTURE_OVERHEAT:
  {
    switch (overheatPhase)
    {
    case PHASE_WARNING:
      if (!phaseInitialized)
      {
        vent.initParam(RED, 100, CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_WARNING]);
        break;
      }
      vent.flicker(25, 25);
      break;
    case PHASE_OVERHEAT:
      if (!phaseInitialized)
      {
        vent.initParam(BLUE, 100, CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT]);
        break;
      }
      vent.ramp();
      break;
    case PHASE_COOLING:
      if (!phaseInitialized)
      {
        vent.initParam(BLUE, 0, CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_COOLING]);
        break;
      }
      vent.ramp();
      break;
    }
    break;
  }

  case STATE_CAPTURE_TAIL:
  {
    if (!stateInitialized)
    {
      vent.initParam(WARM_WHITE, 50, 300);
      break;
    }
    vent.ramp();
    break;
  }

  case STATE_BURST:
  {
    switch (burstPhase)
    {
    case BURST_PHASE_FIRING:
      if (!phaseInitialized)
      {
        uint8_t intensity = 50 + (heatLevel / MAX_BURST_SHOTS);
        vent.initParam(ORANGE, intensity, getDuration());
        break;
      }
      vent.flicker(25, 25);
      break;
    case BURST_PHASE_TAIL:
      if (!phaseInitialized)
      {
        uint8_t intensity = 50 + (heatLevel / MAX_BURST_SHOTS);
        vent.initParam(WARM_WHITE, intensity, 300);
        break;
      }
      vent.ramp();
      break;
    }
    break;
  }

  case STATE_BURST_OVERHEAT:
  {
    switch (overheatPhase)
    {
    case PHASE_WARNING:
      if (!phaseInitialized)
      {
        vent.initParam(RED, 100, BURST_OVERHEAT_PHASE_LENGTH[PHASE_WARNING] / 2);
        break;
      }
      vent.flicker(25, 25);
      break;
    case PHASE_OVERHEAT:
      if (!phaseInitialized)
      {
        vent.initParam(BLUE, 100, BURST_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT] / 4);
        break;
      }
      vent.ramp();
      break;
    case PHASE_COOLING:
      if (!phaseInitialized)
      {
        vent.initParam(BLUE, 0, BURST_OVERHEAT_PHASE_LENGTH[PHASE_COOLING]);
        break;
      }
      vent.ramp();
      break;
    }
    break;
  }
  }
}

void getPanelBarMeterScheme()
{
  auto &animations = panelBarMeter.animations();

  switch (WBstate)
  {
  case STATE_ZERO:
  case STATE_LOW_BATT:
  case STATE_POWER_OFF:
  case STATE_POWER_OFF_TO_ON:
  case STATE_POWER_ON_TO_OFF:
  case STATE_POWER_ON:
  {
    if (!stateInitialized)
    {
      animations.stop();
      break;
    }
    break;
  }

  case STATE_PARTY_MODE:
  {
    if (!stateInitialized)
    {
      animations.stop().beatPulse(116);
      break;
    }
    break;
  }

  case STATE_PARTY_MODE_IN:
  {
    if (!stateInitialized)
    {
      uint16_t interval = constrain(max(5U, getDuration()) / (PBM_SEG_NUMBER * 2U), 10U, 255U);
      animations.stop()
          .fillUpIntv(interval)
          .enqueue()
          .emptyDownIntv(interval)
          .enqueue()
          .startQueue();

      break;
    }
    break;
  }

  case STATE_PARTY_MODE_OUT:
  case STATE_CYCLOTRON_ON_TO_OFF:
  case STATE_ALL_ON_TO_OFF:
  {
    if (!stateInitialized)
    {
      int32_t available = max(
          5L,
          (int32_t)getDuration() - 10L * PBM_SEG_NUMBER);
      int32_t denominator = PBM_SEG_NUMBER * 11L;
      uint16_t emptyInterval = constrain(
          (available * 10L + denominator / 2) / denominator,
          10L,
          255L);
      animations.stop()
          .fillUpIntv(10)
          .enqueue()
          .emptyDownIntv(emptyInterval)
          .enqueue()
          .startQueue();

      break;
    }
    break;
  }

  case STATE_CYCLOTRON_OFF_TO_ON:
  case STATE_CYCLOTRON_ON_TO_FULL:
  {
    if (!stateInitialized)
    {
      uint16_t interval = constrain(max(5U, getDuration()) / (PBM_SEG_NUMBER * 2U), 10U, 255U);
      animations.stop()
          .fillDownIntv(interval)
          .enqueue()
          .emptyDownIntv(interval)
          .enqueue()
          .startQueue();

      break;
    }
    break;
  }

  case STATE_CYCLOTRON_ON:
  {
    if (!stateInitialized)
    {
      animations.bounceFillFromEdgesIntv(25, 25, &PANEL_MAX_PERCENT, &heatLevel)
          .loop();

      break;
    }
    break;
  }

  case STATE_CYCLOTRON_FULL_TO_ON:
  {
    if (!stateInitialized)
    {
      int32_t available = max(5L, (int32_t)getDuration() - 10L * PBM_SEG_NUMBER);
      int32_t denominator = PBM_SEG_NUMBER * 11L;
      uint16_t emptyInterval = constrain(
          (available * 10L + denominator / 2) / denominator,
          10L,
          255L);
      animations.stop()
          .fillUpIntv(10)
          .enqueue()
          .emptyDownIntv(emptyInterval)
          .enqueue()
          .startQueue();

      break;
    }
    break;
  }

  case STATE_CYCLOTRON_FULL_POWER:
  {
    if (!stateInitialized)
    {
      animations.bounceFillFromCenterIntv(25, 25, &PANEL_MAX_PERCENT, &heatLevel)
          .loop();
      break;
    }
    break;
  }

  case STATE_CAPTURE:

  {
    // Keep the block speed synchronized with the live heat level.
    uint16_t speed = map(heatLevel, 0, 100, 25, 10);
    if (!stateInitialized)
    {
      animations.stop().collidingBlocks(speed, 8, PBM_BLOCK_SPACING, 0);
      break;
    }
    // Keep the animation responsive as the heat level changes.
    animations.setUpdateInterval(speed);
    break;
  }

  case STATE_CAPTURE_TAIL:
  {
    static bool blocksStopped = false;
    // Keep the block speed synchronized with the live heat level.
    uint16_t speed = map(heatLevel, 0, 100, 25, 10);
    if (!stateInitialized)
    {
      // Reverse the Capture blocks so the remaining blocks drain outward.
      animations.toggleLogic();
      blocksStopped = false;
      break;
    }
    if (!blocksStopped)
    {
      // Continue updating the blocks while the tail lets them drain.
      animations.setUpdateInterval(speed);

      // Capture tail is a separate state, so its emission delay starts at
      // stateStartTime. Stop creating blocks but let emitted blocks finish.
      if (now - stateStartTime >= PBM_BLOCK_ENDING_OFFSET)
      {
        animations.stopBlockEmission();
      }
      if (!animations.isRunning())
      {
        // Reset the completed animation and its retained frame before queuing
        // the tail animation; otherwise the old final frame can briefly flash.
        animations.stop()
            .wait(PBM_BLOCK_ENDING_DELAY)
            .enqueue()
            .bounceFillFromEdgesIntv(25, 25, &PANEL_MAX_PERCENT, &heatLevel)
            .loop()
            .enqueue()
            .startQueue();
        blocksStopped = true;
      }
    }
    break;
  }

  case STATE_BURST:
  {
    // Keep the block speed synchronized with the live heat level.
    uint16_t speed = map(heatLevel, 0, 100, 25, 10);
    static bool blocksStopped = false;

    switch (burstPhase)
    {
    case BURST_PHASE_FIRING:
      if (!phaseInitialized)
      {
        animations.stop().explodingBlocks(speed, 8, PBM_BLOCK_SPACING, 0);
        break;
      }
      animations.setUpdateInterval(speed);
      break;

    case BURST_PHASE_TAIL:
      if (!phaseInitialized)
      {
        // Reverse the firing blocks so the remaining blocks drain outward.
        animations.toggleLogic();
        blocksStopped = false;
        break;
      }
      if (!blocksStopped)
      {
        // Continue updating the blocks while the tail lets them drain.
        animations.setUpdateInterval(speed);

        // Burst tail is a phase inside STATE_BURST, so its emission delay
        // starts at phaseStartTime rather than stateStartTime.
        if (now - phaseStartTime >= PBM_BLOCK_ENDING_OFFSET)
        {
          animations.stopBlockEmission();
        }
        if (!animations.isRunning())
        {
          // Reset the completed animation and its retained frame before
          // queuing the tail animation to prevent a transition flash.
          animations.stop()
              .wait(PBM_BLOCK_ENDING_DELAY)
              .enqueue()
              .bounceFillFromCenterIntv(25, 25, &PANEL_MAX_PERCENT, &heatLevel)
              .loop()
              .enqueue()
              .startQueue();
          blocksStopped = true;
        }
      }
      break;
    }
    break;
  }

  case STATE_CAPTURE_OVERHEAT:
  case STATE_BURST_OVERHEAT:
  {
    // Keep the block speed synchronized with the live heat level.
    uint16_t speed = map(heatLevel, 0, 100, 25, 10);
    switch (overheatPhase)
    {
    case PHASE_WARNING:
      if (!phaseInitialized)
      {
        if (WBstate == STATE_CAPTURE_OVERHEAT && prevState == STATE_CYCLOTRON_ON) // Only init if it's direct warning without going through STATE_CAPTURE
          animations.stop().collidingBlocks(speed, 8, PBM_BLOCK_SPACING, 0);
        else if (WBstate == STATE_BURST_OVERHEAT)
          animations.stop().explodingBlocks(speed, 8, PBM_BLOCK_SPACING, 0);
        break;
      }
      // Keep the animation responsive as the heat level changes.
      animations.setUpdateInterval(speed);
      break;
    case PHASE_OVERHEAT:
      if (!phaseInitialized)
      {
        // Reverse the warning blocks for the overheat discharge direction.
        animations.invertLogic();
        break;
      }
      // Keep the animation responsive while existing blocks drain.
      animations.setUpdateInterval(speed);
      if (now - phaseStartTime > PBM_BLOCK_ENDING_DELAY)
        animations.stopBlockEmission();
      break;
    case PHASE_COOLING:
      if (!phaseInitialized)
      {
        uint16_t interval = BURST_OVERHEAT_PHASE_LENGTH[PHASE_COOLING] / (2 * PBM_SEG_NUMBER);
        uint16_t blockSize = PBM_SEG_NUMBER;
        animations.stop()
            .explodingBlocks(interval, blockSize, blockSize, 1);
        break;
      }
      break;
    }
    break;
  }
  }
}

static_assert(CYCLOTRON_STATE_CONFIG_COUNT == STATE_LOW_BATT + 1,
              "CYCLOTRON_STATE_CONFIG must contain one entry per BlasterState");

uint16_t getCyclotronRampTime(uint8_t rampTime)
{
  switch (rampTime)
  {
  case CYC_RAMP_TRACK:
    return getDuration();
  case CYC_RAMP_FINISH:
    return 500;
  case CYC_RAMP_CAPTURE_TAKEOFF:
    return DURATION_CAPTURE_TAKEOFF_RAMP;
  default:
    return 0;
  }
}

void getCyclotronScheme()
{
  const CyclotronStateConfig config = {
      pgm_read_byte(&CYCLOTRON_STATE_CONFIG[WBstate].target),
      pgm_read_byte(&CYCLOTRON_STATE_CONFIG[WBstate].rampTime)};

  if (config.target != CYC_TARGET_PHASED)
  {
    if (config.target == CYC_TARGET_CLEAR)
    {
      if (!stateInitialized)
        cyclotron.clear();
      return;
    }
    if (!stateInitialized)
    {
      cyclotron.rampInit(getCyclotronTarget(config.target,
                                            !SWmain.isReleased(),
                                            !SWcyclotron.isReleased(),
                                            SWactivate.isPressed()),
                         getCyclotronRampTime(config.rampTime));
      return;
    }
    cyclotron.ramp();
    return;
  }

  // Only the three phase-driven state IDs reach this switch.
  switch ((uint8_t)WBstate)
  {
  case STATE_CAPTURE_OVERHEAT:
  {
    switch (overheatPhase)
    {
    case PHASE_WARNING:
      if (!phaseInitialized)
      {
        cyclotron.rampInit(CYC_CAPTURE_WARNING, CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_WARNING]);
        break;
      }
      cyclotron.ramp();
      break;
    case PHASE_OVERHEAT:
      if (!phaseInitialized)
      {
        cyclotron.rampInit(CYC_OFF,
                           CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT] +
                               CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_COOLING] -
                               1000);
        break;
      }
      cyclotron.ramp();
      break;
    case PHASE_COOLING:
      if (!phaseInitialized)
        break;
      cyclotron.ramp();
      break;
    }
    break;
  }

  case STATE_BURST:
  {
    switch (burstPhase)
    {
    case BURST_PHASE_FIRING:
      if (!phaseInitialized)
      {
        cyclotron.rampInit(CYC_BURST_MAX, BURST_PHASE_LENGTH[BURST_PHASE_FIRING]);
        break;
      }
      cyclotron.ramp();
      break;
    case BURST_PHASE_TAIL:
      if (!phaseInitialized)
      {
        // A burst returns to full power, so complete that transition during
        // its tail instead of undershooting and correcting in the next state.
        cyclotron.rampInit(CYC_FULL, BURST_PHASE_LENGTH[BURST_PHASE_TAIL]);
        break;
      }
      cyclotron.ramp();
      break;
    }
    break;
  }

  case STATE_BURST_OVERHEAT:
  {
    switch (overheatPhase)
    {
    case PHASE_WARNING:
      if (!phaseInitialized)
      {
        cyclotron.rampInit(CYC_BURST_WARNING, BURST_OVERHEAT_PHASE_LENGTH[PHASE_WARNING]);
        break;
      }
      cyclotron.ramp();
      break;
    case PHASE_OVERHEAT:
      if (!phaseInitialized)
      {
        cyclotron.rampInit(CYC_OFF,
                           BURST_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT] +
                               BURST_OVERHEAT_PHASE_LENGTH[PHASE_COOLING] -
                               1000);
        break;
      }
      cyclotron.ramp();
      break;
    case PHASE_COOLING:
      if (!phaseInitialized)
        break;
      cyclotron.ramp();
      break;
    }
    break;
  }
  }
}

void getFiringRodScheme()
{
  switch (WBstate)
  {
  case STATE_ZERO:
  case STATE_LOW_BATT:
  case STATE_POWER_OFF:
  case STATE_POWER_OFF_TO_ON:
  case STATE_PARTY_MODE:
  case STATE_PARTY_MODE_IN:
  case STATE_PARTY_MODE_OUT:
  case STATE_POWER_ON_TO_OFF:
  case STATE_POWER_ON:
  case STATE_CYCLOTRON_OFF_TO_ON:
  case STATE_CYCLOTRON_ON_TO_OFF:
  case STATE_CYCLOTRON_ON:
  case STATE_CYCLOTRON_ON_TO_FULL:
  case STATE_CYCLOTRON_FULL_TO_ON:
  case STATE_CYCLOTRON_FULL_POWER:
  {
    if (!stateInitialized)
    {
      firingRod.clear();
      break;
    }
    break;
  }

  case STATE_CAPTURE:
  {
    if (!stateInitialized)
    {
      firingRod.strobeInit(SHUFFLE, 100, 300);
      break;
    }
    firingRod.strobe();
    break;
  }

  case STATE_CAPTURE_OVERHEAT:
  case STATE_BURST_OVERHEAT:
  {
    switch (overheatPhase)
    {
    case PHASE_WARNING:
      if (!phaseInitialized)
      {
        firingRod.strobeInit(SHUFFLE, 100, 300);
        break;
      }
      firingRod.strobe();
      break;
    case PHASE_OVERHEAT:
      if (!phaseInitialized)
      {
        firingRod.strobeInit(NO_SHUFFLE, 0, 2000);
        break;
      }
      firingRod.strobe();
      break;
    case PHASE_COOLING:
      if (!phaseInitialized)
        break;
      firingRod.strobe();
      break;
    }
    break;
  }

  case STATE_CAPTURE_TAIL:
  case STATE_ALL_ON_TO_OFF:
  {
    if (!stateInitialized)
    {
      firingRod.strobeInit(NO_SHUFFLE, 0, 1000);
      break;
    }
    firingRod.strobe();
    break;
  }

  case STATE_BURST:
  {
    switch (burstPhase)
    {
    case BURST_PHASE_FIRING:
      if (!phaseInitialized)
      {
        firingRod.strobeInit(SHUFFLE, 100, 300);
        break;
      }
      firingRod.strobe();
      break;
    case BURST_PHASE_TAIL:
      if (!phaseInitialized)
      {
        firingRod.strobeInit(NO_SHUFFLE, 0, 1000);
        break;
      }
      firingRod.strobe();
      break;
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
#ifdef POWERCELL_BARMETER
void initPowerCellBarMeterSpeedRamp(uint16_t targetSpeed, uint16_t rampDuration)
{
  powerCellBatteryPercent = constrain(map(battLevel, 20, 70, 20, 100), 20, 100);
  powerCellRampStartInterval = powerCellAnimationInterval;
  powerCellRampTargetInterval = constrain(targetSpeed / 2, 5, 255);
  powerCellRampDuration = rampDuration;
  powerCellRampStartTime = now;

  if (rampDuration == 0)
    powerCellAnimationInterval = powerCellRampTargetInterval;

  powerCellBarMeter.animations().stop().fillUpIntv(powerCellAnimationInterval, &powerCellBatteryPercent).loop();
}

void updatePowerCellBarMeterSpeedRamp()
{
  powerCellBatteryPercent = constrain(map(battLevel, 20, 70, 20, 100), 20, 100);

  if (powerCellRampDuration != 0)
  {
    uint32_t elapsed = min(now - powerCellRampStartTime, (uint32_t)powerCellRampDuration);
    int32_t difference = (int32_t)powerCellRampTargetInterval - powerCellRampStartInterval;
    powerCellAnimationInterval = powerCellRampStartInterval +
                                 difference * elapsed / powerCellRampDuration;

    // Finalize the target and mark the ramp inactive so subsequent updates
    // do not keep recalculating an already completed interpolation.
    if (elapsed >= powerCellRampDuration)
    {
      powerCellAnimationInterval = powerCellRampTargetInterval;
      powerCellRampDuration = 0;
    }
  }

  powerCellBarMeter.animations().setUpdateInterval(powerCellAnimationInterval);
}
#endif

// The state/phase flow below is shared by both Power Cell display types.
// These backend helpers translate semantic effects into the implementation
// selected at compile time, keeping hardware details out of the state switch.
void powerCellOffInit()
{
#ifdef POWERCELL_BARMETER
  powerCellBarMeter.animations().stop();
#else
  powerCell.clear();
#endif
}

void powerCellLowBattInit()
{
#ifdef POWERCELL_BARMETER
  powerCellBarMeter.animations().stop().blinkPixel(0, MEDIUM_BLINK_SP);
#else
  powerCell.lowBattInit(MEDIUM_BLINK_SP);
#endif
}

void powerCellLowBattUpdate()
{
#ifndef POWERCELL_BARMETER
  powerCell.lowBatt();
#endif
}

void powerCellPoweredDownInit()
{
#ifdef POWERCELL_BARMETER
  powerCellBarMeter.animations().stop().blinkPixel(0, 50, 4950, 0);
#else
  powerCell.poweredDownInit(5000);
#endif
}

void powerCellPoweredDownUpdate()
{
#ifndef POWERCELL_BARMETER
  powerCell.poweredDown();
#endif
}

void powerCellBootInit(uint16_t duration)
{
#ifdef POWERCELL_BARMETER
  const uint16_t steps = 1 + (POWERCELL_SEG_NUMBER / 2 + 1) * (POWERCELL_SEG_NUMBER / 2 + 2) / 2;
  const uint16_t interval = max(5, duration / steps);
  powerCellBatteryPercent = constrain(map(battLevel, 20, 70, 20, 100), 20, 100);
  powerCellAnimationInterval = PC_POWER_ON_UPDATE_INT / 2;
  powerCellBarMeter.animations().stop().downStackingBlocks(interval, 2).enqueue().fillUpIntv(powerCellAnimationInterval, &powerCellBatteryPercent).loop().enqueue().startQueue();
#else
  powerCell.bootInit(duration);
#endif
}

void powerCellBootUpdate()
{
#ifndef POWERCELL_BARMETER
  powerCell.boot();
#endif
}

void initPowerCellSpeedRamp(uint16_t targetSpeed, uint16_t rampDuration)
{
#ifdef POWERCELL_BARMETER
  initPowerCellBarMeterSpeedRamp(targetSpeed, rampDuration);
#else
  powerCell.initSpeedRamp(targetSpeed, rampDuration);
#endif
}

void updatePowerCellSpeedRamp()
{
#ifdef POWERCELL_BARMETER
  updatePowerCellBarMeterSpeedRamp();
#else
  powerCell.updateSpeedRamp();
#endif
}

void powerCellShutdownInit(uint16_t duration)
{
#ifdef POWERCELL_BARMETER
  const uint16_t steps = 1 + (POWERCELL_SEG_NUMBER / 2 + 1) * (POWERCELL_SEG_NUMBER / 2 + 2) / 2;
  const uint16_t durationPerStep = max(5, duration / steps);
  powerCellBarMeter.animations().stop().upUnstackingBlocks(durationPerStep, 2);
#else
  powerCell.shutDownInit(duration);
#endif
}

void powerCellShutdownUpdate()
{
#ifndef POWERCELL_BARMETER
  powerCell.shutDown();
#endif
}

static_assert(POWERCELL_STATE_CONFIG_COUNT == STATE_LOW_BATT + 1,
              "POWERCELL_STATE_CONFIG must contain one entry per BlasterState");

uint16_t getPowerCellRampTime(uint8_t rampTime)
{
  switch (rampTime)
  {
  case PC_RAMP_TRACK:
    return getDuration();
  case PC_RAMP_BOOT:
    return min(getDuration(), getSpecificDuration(STATE_POWER_ON_TO_OFF));
  default:
    return 0;
  }
}

void getPowerCellScheme()
{
  const PowerCellStateConfig config = {
      pgm_read_byte(&POWERCELL_STATE_CONFIG[WBstate].target),
      pgm_read_byte(&POWERCELL_STATE_CONFIG[WBstate].rampTime)};

  if (config.target != PC_TARGET_PHASED)
  {
    if (!stateInitialized)
    {
      switch (config.target)
      {
      case PC_TARGET_OFF:
        powerCellOffInit();
        break;
      case PC_TARGET_LOW_BATT:
        powerCellLowBattInit();
        break;
      case PC_TARGET_POWERED_DOWN:
        powerCellPoweredDownInit();
        break;
      case PC_TARGET_BOOT:
        powerCellBootInit(getPowerCellRampTime(config.rampTime));
        break;
      case PC_TARGET_SHUTDOWN:
        powerCellShutdownInit(getPowerCellRampTime(config.rampTime));
        break;
      default:
        initPowerCellSpeedRamp(getPowerCellTargetSpeed(
                                   config.target,
                                   !SWmain.isReleased(),
                                   !SWcyclotron.isReleased(),
                                   SWactivate.isPressed()),
                               getPowerCellRampTime(config.rampTime));
        break;
      }
      return;
    }

    switch (config.target)
    {
    case PC_TARGET_LOW_BATT:
      powerCellLowBattUpdate();
      break;
    case PC_TARGET_POWERED_DOWN:
      powerCellPoweredDownUpdate();
      break;
    case PC_TARGET_BOOT:
      powerCellBootUpdate();
      break;
    case PC_TARGET_SHUTDOWN:
      powerCellShutdownUpdate();
      break;
    case PC_TARGET_OFF:
      break;
    default:
      updatePowerCellSpeedRamp();
      break;
    }
    return;
  }

  // Only the three phase-driven state IDs reach this switch.
  switch ((uint8_t)WBstate)
  {

  case STATE_CAPTURE_OVERHEAT:
    switch (overheatPhase)
    {
    case PHASE_WARNING:
      if (!phaseInitialized)
      {
        initPowerCellSpeedRamp(PC_FIRING_MAX_UPDATE_INT, CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_WARNING]);
        break;
      }
      updatePowerCellSpeedRamp();
      break;

    case PHASE_OVERHEAT:
      if (!phaseInitialized)
      {
        // Start one continuous ramp across both phases. Cooling deliberately
        // keeps updating this ramp instead of reinitializing it at its boundary.
        initPowerCellSpeedRamp(PC_CYC_ON_UPDATE_INT,
                               CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT] +
                                   CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_COOLING]);
        break;
      }
      updatePowerCellSpeedRamp();
      break;

    case PHASE_COOLING:
      if (!phaseInitialized)
        break;
      updatePowerCellSpeedRamp();
      break;
    }
    break;

  case STATE_BURST:
    switch (burstPhase)
    {
    case BURST_PHASE_FIRING:
      if (!phaseInitialized)
      {
        initPowerCellSpeedRamp(PC_FIRING_MAX_UPDATE_INT, BURST_PHASE_LENGTH[BURST_PHASE_FIRING]);
        break;
      }
      updatePowerCellSpeedRamp();
      break;

    case BURST_PHASE_TAIL:
      if (!phaseInitialized)
      {
        initPowerCellSpeedRamp(PC_CYC_ON_UPDATE_INT, BURST_PHASE_LENGTH[BURST_PHASE_TAIL]);
        break;
      }
      updatePowerCellSpeedRamp();
      break;
    }
    break;

  case STATE_BURST_OVERHEAT:
    switch (overheatPhase)
    {
    case PHASE_WARNING:
      if (!phaseInitialized)
      {
        initPowerCellSpeedRamp(PC_FIRING_MAX_UPDATE_INT, BURST_OVERHEAT_PHASE_LENGTH[PHASE_WARNING]);
        break;
      }
      updatePowerCellSpeedRamp();
      break;

    case PHASE_OVERHEAT:
      if (!phaseInitialized)
      {
        // Start one continuous ramp here and let cooling finish it.
        initPowerCellSpeedRamp(PC_CYC_ON_UPDATE_INT,
                               BURST_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT] +
                                   BURST_OVERHEAT_PHASE_LENGTH[PHASE_COOLING]);
        break;
      }
      updatePowerCellSpeedRamp();
      break;

    case PHASE_COOLING:
      if (!phaseInitialized)
        break;
      updatePowerCellSpeedRamp();
      break;
    }
    break;

  }
}
#endif
//  END_SEQ of Optional PowerCell animations functions in wrist blaster states
////////////////////////////////////////////////

void enterState(BlasterState next_state)
{
  // Phase-driven engines are evaluated before the state handler in loop().
  // Select their first phase here so the very first engine pass sees valid,
  // pending phase data rather than a phase left over from the prior sequence.
  switch (next_state)
  {
  case STATE_BURST:
    burstPhase = BURST_PHASE_FIRING;
    phaseInitialized = false;
    break;

  case STATE_CAPTURE_OVERHEAT:
  case STATE_BURST_OVERHEAT:
    overheatPhase = PHASE_WARNING;
    phaseInitialized = false;
    break;

  default:
    break;
  }

  WBstate = next_state;
  stateInitialized = false;
}

bool checkIfTrackDoneExit(BlasterState next_state)
{
  if (!player.isPlaying())
  {
    // Track done or time elapsed, change state
    enterState(next_state);
    return true;
  }
  return false;
}

bool checkIfSwitchExit(bool switch_state, BlasterState next_state)
{
  if (switch_state)
  {
    // Switch is triggered and delay has passed, change state
    enterState(next_state);
    return true;
  }
  return false;
}

bool checkIfTimerExit(uint16_t time, BlasterState next_state)
{
  if (now - stateStartTime < time)
    return false;

  // The state timer expired; continue with the requested state.
  enterState(next_state);
  return true;
}

bool stateInitialization() // Standard initializers for most states
{
  playThisStateTrack();
  stateStartTime = now;
  return true; // Mark the state as initialized
}

void playThisStateTrack()
{
  if (TRACK_LENGTH[WBstate] == 0)
    player.stop();
  else if (TRACK_LOOPING[WBstate])
    player.loopFileNum(WBstate);
  else
#ifdef PLAYER_USE_BUSY_PIN
    player.playFileNum(WBstate);
#else
    player.playFileNum(WBstate, TRACK_LENGTH[WBstate]);
#endif

  DEBUG_PRINTLN("Track: " + String(WBstate) + "  length: " +
                String(TRACK_LENGTH[WBstate]) +
                " Loop required: " + String(TRACK_LOOPING[WBstate]));
}

void playThisTrack(uint8_t track)
{

  if (TRACK_LOOPING[track])
    player.loopFileNum(track);
  else
#ifdef PLAYER_USE_BUSY_PIN
    player.playFileNum(track);
#else
    player.playFileNum(track, TRACK_LENGTH[track]);
#endif

  DEBUG_PRINTLN("Track: " + String(track) + "  length: " +
                String(TRACK_LENGTH[track]) +
                " Loop required: " + String(TRACK_LOOPING[track]));
}

uint16_t getDuration() // Get the animation/effect timeline duration for the current state
{
  const uint16_t trackLength = TRACK_LENGTH[WBstate];
  return trackLength > AUDIO_ADVANCE ? trackLength - AUDIO_ADVANCE : 0;
}

uint16_t getSpecificDuration(BlasterState state)
{ // Get the animation/effect timeline duration for a specific state
  const uint16_t trackLength = TRACK_LENGTH[state];
  return trackLength > AUDIO_ADVANCE ? trackLength - AUDIO_ADVANCE : 0;
}

void checkNextPreviousButton()
{
  static uint32_t pbfirePrev = 0;
  if (PBfire.justPressed())
  {
    pbfirePrev = now;
    return;
  }

  if ((PBfire.justReleased()))
  {
    uint32_t pressDuration = now - pbfirePrev;

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

  if (now - heatLevelPrevUpdate > 125)
  // Decrease heat level every 125ms, about 12.5secondes before going into overheat.
  {
    heatLevelPrevUpdate = now;

    if (heatLevel > 0) // Ensure that heatLevel doesn't overflow as unsigned...
      heatLevel -= 1;
  }
}

void heatLevelRisingCapture() // // Increase heat level from 0 to 100 over DURATION_CAPTURE_MAX.
{                             // HeatLevel 0 -100 %, at 100%, wrist blaster goes into overheat...
  if (now - heatLevelPrevUpdate < (DURATION_CAPTURE_MAX / 100))
    return;

  heatLevelPrevUpdate = now;

  heatLevel += 1;
  heatLevel = min(100, heatLevel);
  return;
}

void heatLevelRisingBurst() // Increase heat level by approximately 100 / MAX_BURST_SHOTS for each burst.
{                           // HeatLevel 0 -100 %, at 100%, wrist blaster goes into overheat...
  heatLevelPrevUpdate = now;

  // round() replaced by integer rounding: add half the denominator before division.
  heatLevel += (100U + MAX_BURST_SHOTS / 2U) / MAX_BURST_SHOTS;
  heatLevel = min(100, heatLevel);
  return;
}

uint8_t getCaptureScaledDuration()
{
  uint16_t maxDuration = constrain(DURATION_CAPTURE_MAX, 10000, TRACK_LENGTH[STATE_CAPTURE]);
  uint16_t warningDuration = CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_WARNING];
  if (warningDuration >= maxDuration)
    return 0;

  const uint32_t numerator = 100UL * (maxDuration - warningDuration);
  // round() replaced by integer rounding: add half the denominator before division.
  return (numerator + maxDuration / 2U) / maxDuration;
}

#ifdef SMOKE_FEATURES_ENABLED
void checkSmokerEnabling()
{
  static uint32_t pushedDetected = 0;
  static bool flag = false;

  if (PBfire.justPressed())
  {
    pushedDetected = now;
    topWhiteIndicator.solid();
  }

  if (PBfire.isPressed() &&
      now - pushedDetected >= 3000 &&
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
  if (PBfire.justReleased())
  {
    flag = false;
    topWhiteIndicator.initParam(smoker.enable() ? GREEN : RED, 50);
    // topWhiteIndicator.blink(DISABLE);
  }
}
#endif

bool getPartyModeState()
{
  // Output is like a switch
  if (INTENSIFY_IS_A_SWITCH)
    return PBintensify.isPressed();

  // Output latches on each push-button release
  return PBintensify.latchedState();
}
