/**
 * @file        SBK_WRISTBLASTER_CONFIG.h
 * @brief       Configuration file for the SBK_WRISTBLASTER_CORE system.
 *
 * @author      Samuel Barabé
 * @copyright   Copyright (c) 2025-2026 Samuel Barabé
 * @license     MIT License
 * @version     2.0.0
 * @link        https://github.com/sbarabe/SBK_WRISTBLASTER
 *
 * @details
 * This file contains all user-adjustable settings for the SBK Wrist Blaster system.
 *
 * It includes:
 * - Pin assignments (per board type)
 * - LED indexes
 * - Track durations
 * - Feature toggles
 *
 * Modify this file to match your specific hardware configuration and desired behaviors.
 *
 * For full setup instructions and documentation, and to support this project, please visit:
 * https://github.com/sbarabe/SBK_WRISTBLASTER
 *
 *
 * @see https://opensource.org/licenses/MIT
 * DFPlayer Mini commands are sent by the lightweight SBK_WB_PlayerEngine.
 * @see https://github.com/adafruit/Adafruit_NeoPixel
 */

/**********************************************************************************************
 *
 *  SBK_WRISTBLASTER CONFIGURATION FILE
 *
 *  - All your hardware and functional settings should be defined here.
 *  - Do NOT modify the core file (SBK_WRISTBLASTER_CORE.ino) unless you are an advanced user.
 *  - Avoid editing PINS_DEF unless you are defining a custom pin mapping.
 *  - This modular approach keeps the code clean, maintainable, and easy to update.
 *
 *  The codebase is modular and object-oriented, with the engine components located in the
 *  `SBK_WristBlaster_lib` folder.
 *
 * Important:
 * - Track durations define the timeline for state transitions, animations, and effects.
 *   When the BUSY pin is not used, they also determine audio playback completion.
 * - Incorrect durations may desynchronize animations and effects from the audio track.
 * - Advanced users may modify animations or state transitions in the core engine logic.
 *
 *  HOW TO USE THIS FILE:
 *  - Edit values only in blocks marked "USER SETTINGS".
 *  - For "Select ONE" choices, uncomment one #define and comment out the others.
 *  - Leave blocks marked "INTERNAL SETUP / VALIDATION" unchanged.
 *
 *  TIP: Save a backup of this file after customization.
 *
 ***********************************************************************************************/

#pragma once

#include <Arduino.h>

/******************************************************************************
 * DEBUG TO SERIAL
 ******************************************************************************/

// --- USER SETTINGS ----------------------------------------------------------
// Uncomment to send state and initialization diagnostics to the Serial monitor.
// #define DEBUG_TO_SERIAL

// --- INTERNAL SETUP / VALIDATION --------------------------------------------
#ifdef DEBUG_TO_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif
#ifdef ARDUINO_AVR_NANO_EVERY
#define DEBUG_BAUDRATE 115200
#else
#define DEBUG_BAUDRATE 9600
#endif

/******************************************************************************
 * MCU AND PCB PINS
 ******************************************************************************/

// --- USER SETTINGS: SELECT EXACTLY ONE --------------------------------------
// Use CUSTOM_PINS_DEFINITION only after adding its pin map to PINS_DEF.
// #define CUSTOM_PINS_DEFINITION
// #define SBK_WRIST_BLASTER_PCB_V2
// #define SBK_WRIST_BLASTER_II_PCB_Vx
#define SBK_PROPCORE_ONE_Vx
// #define SBK_PROPCORE_ONE_PLUS_Vx

// --- INTERNAL SETUP / VALIDATION --------------------------------------------
#include "SBK_WRISTBLASTER_PINS_DEF.h"
// Ensure only one PCB is defined
#if (defined(CUSTOM_PINS_DEFINITION) + defined(SBK_WRIST_BLASTER_PCB_V2) + defined(SBK_WRIST_BLASTER_II_PCB_Vx) + defined(SBK_PROPCORE_ONE_Vx) + defined(SBK_PROPCORE_ONE_PLUS_Vx)) != 1
#error "SBK_WRISTBLASTER_CONFIG.h : You must define a PCB, and exactly one PCB type only!"
#endif

/******************************************************************************
 * SWITCHES AND BUTTONS
 ******************************************************************************/

// --- EXTERNAL LIBRARY DEPENDENCY --------------------------------------------
#include <SBK_Button.h>

// --- USER SETTINGS ----------------------------------------------------------
// INTENSIFY BUTTON OR SWITCH
// If your Intensify push button is replaced by a switch in your Wrist Blaster, set this to "true".
// Intensify is a Push Button, set this to "false".
const bool INTENSIFY_IS_A_SWITCH = false;
const ButtonLogic INTENSIFY_PB_LOGIC = ButtonLogic::NORMAL; // Except if replaced by a switch, Intensify PB should normally use NORMAL logic
// Define others switches/buttons logic here :
// If your switch level isn't the way you want, you could reverse it here instead of redoing your wires.
const ButtonLogic MAIN_POWER_SW_LOGIC = ButtonLogic::NORMAL;      // ButtonLogic::NORMAL / ButtonLogic::INVERTED
const ButtonLogic CYCLOTRON_POWER_SW_LOGIC = ButtonLogic::NORMAL; // ButtonLogic::NORMAL / ButtonLogic::INVERTED
const ButtonLogic ACTIVATE_SW_LOGIC = ButtonLogic::NORMAL;        // ButtonLogic::NORMAL / ButtonLogic::INVERTED
const ButtonLogic FIRE_PB_LOGIC = ButtonLogic::NORMAL;            // PB should normally use NORMAL logic

/******************************************************************************
 * SMOKE MACHINE
 ******************************************************************************/

// --- USER SETTINGS ----------------------------------------------------------
// **** SMOKE DEVICE HARDWARE AND CODE NOT TESTED YET ****
// Uncomment to enable all smoke-related features.
// #define SMOKE_FEATURES_ENABLED

#ifdef SMOKE_FEATURES_ENABLED
// These values apply only when smoke features are enabled.
const uint16_t SMOKER_MAX_ON_TIME = 15000; // ms; maximum continuous ON time
const uint16_t SMOKER_MIN_OFF_TIME = 10000; // ms; prevents short cycling

// --- EXTERNAL LIBRARY DEPENDENCY --------------------------------------------
#include <SBK_WB_SmokeEngine_V2_0_0.h>

// --- INTERNAL SETUP / VALIDATION --------------------------------------------
// Smoker is activated, if minimum off time is respected, when the wrist blaster goes into
// STATE_CAPTURE_OVERHEAT and stop after STATE_OVERHEATED tail
// SMOKE ENABLING :
// Smoke effects are DISABLE when MCU is powered, you need to activate them in POWER OFF STATE with the Fire Button.
// Hold the Fire button to show the current state on the TOP WHITE INDICATOR: red = disabled, green = enabled.
// If you hold the button for 3 second, the ENABLING/DISABLING will switch and you'll see the
// indicator switching color for the new enable state.
// HARWARE :
// Check SBK_WRISTBLASTER_PINS_DEF.h for the correct pin assignments.
// There should be pins for fan and smoke device activation
#endif // SMOKE_FEATURES_ENABLED

/******************************************************************************
 * PANEL BAR METER
 ******************************************************************************/

// --- USER SETTINGS ----------------------------------------------------------
const bool PBM_DIRECTION = false; // false = forward, true = reverse
const uint8_t PBM_SEG_NUMBER = 28;

// Select exactly ONE driver type:
#define PBM_MAX72XX // MAX7219/7221 uses 3 pins serial communication : data, clock, load.
// #define PBM_HT16K33 // I2C LEDs driver like Adafruit backpack, uses 2 pins : SDA, SDC.

// Used only with PBM_HT16K33:
#define PBM_HT16K33_I2C_ADDRESS 0x70

// Select exactly ONE native BL28-3005 display mapping:
#define PBM_BL28_3005SK_MAPPING // 4 anodes x 7 cathodes
// #define PBM_BL28_3005SA_MAPPING // 7 anodes x 4 cathodes

// --- INTERNAL SETUP / VALIDATION --------------------------------------------
#if (defined(PBM_MAX72XX) + defined(PBM_HT16K33)) != 1
#error "Panel Bar Meter LEDs driver definition in SBK_WRISTBLASTER_CONFIG.h file. Please define exactly ONE driver type (PBM_MAX72XX or PBM_HT16K33)."
#elif defined(PBM_MAX72XX)
#define PBM_DRIVER_ADDRESS 0 // first driver in the chain
#pragma message("Compiling for Panel Bar Meter with MAX72xx driver")
#elif defined(PBM_HT16K33)
#pragma message("Compiling for Panel Bar Meter with HT16K33 driver")
#define PBM_DRIVER_ADDRESS PBM_HT16K33_I2C_ADDRESS
#endif
constexpr uint8_t PBM_BARDRIVE_DEVICE_INDEX = 0;

#if (defined(PBM_BL28_3005SK_MAPPING) + defined(PBM_BL28_3005SA_MAPPING)) != 1
#error "Panel Bar Meter mapping: select exactly one BL28-3005 mapping."
#elif defined(PBM_BL28_3005SK_MAPPING)
//  This mapping works for this MAX72xx driver PCB "SBK_WB_BG_SK_PCB_Vx"
//  with bar meter holder PCB "SBK_WB_BG_28SEG_PCB_Vx"
#elif defined(PBM_BL28_3005SA_MAPPING)
//  This mapping works for this MAX72xx driver PCB "SBK_WB_BG_SA_PCB_Vx"
//  with bar meter holder PCB "SBK_WB_BG_28SEG_PCB_Vx"
#endif

/******************************************************************************
 * MAIN ADDRESSABLE LED STRIP
 ******************************************************************************/

// --- EXTERNAL LIBRARY DEPENDENCIES ------------------------------------------
#include <SBK_WB_LedsStripBaseEngine_V2_0_0.h>

// --- USER SETTINGS ----------------------------------------------------------
const uint8_t TOTAL_LEDS_NUMBER = 19; // Vent + indicators + firing rod + cyclotron

// Pixel byte order choices: NEO_GRB, NEO_RGB, NEO_BGR, NEO_BRG, NEO_GBR, NEO_RBG
#define MAIN_STRIP_COLOR_ORDER NEO_GRB

// --- Pixel indexes (the first pixel is index 0) -----------------------------
// The following sections contain indexes for the leds on the chain, starting from 0.
// You will need to update these indexes to match where things are in the chain
// Cyclotron, vent, indicators and firing tip index in the WS2812 wrist blaster chain :
const uint8_t LED_INDEX_TIP_1ST = 0;
const uint8_t LED_INDEX_TIP_LAST = 6;
const uint8_t LED_INDEX_FRONT_ORANGE = 7; // Indicator
const uint8_t LED_INDEX_TOP_YELLOW = 8;   // Indicator
const uint8_t LED_INDEX_TOP_WHITE = 9;    // Indicator
const uint8_t LED_INDEX_SLOWBLOW = 10;    // Indicator
const uint8_t LED_INDEX_VENT = 11;
const uint8_t LED_INDEX_CYC_START = 12;
const uint8_t LED_INDEX_CYC_END = 18;
// --- Cyclotron jewel layout -------------------------------------------------
const bool CYCLOTRON_DIRECTION = false; // false = forward, true = reverse
const uint8_t CYC_NUMLEDS = 7;
const uint8_t CYC_RING_1ST = 1;
const uint8_t CYC_RING_LAST = 6;
// Center MUST either be first or last in the cyclotron ring for animations to work correctly.
const uint8_t CYC_CENTER = 0;

// --- Firing rod -------------------------------------------------------------
const uint8_t ROD_NUMLEDS = 7;
// Disable if no potentiometer is connected; a floating input changes hue randomly.
const bool HUE_POT_READY = ENABLE;

// --- Indicator timing -------------------------------------------------------
const uint8_t FAST_BLINK_SP = 100;    // ms
const uint16_t MEDIUM_BLINK_SP = 500; // ms
const uint16_t SLOW_BLINK_SP = 1000;  // ms

// --- Fire-button single LED -------------------------------------------------
// This is a directly driven LED, not an addressable pixel. Keep current below
// the MCU limit; at 5 V, a 330-ohm resistor gives about 9 mA for a yellow LED.
const bool FIRE_BUTTON_LED_READY = ENABLE;

// --- EXTERNAL LIBRARY DEPENDENCY --------------------------------------------
#include <SBK_WB_VentEngine_V2_0_0.h>
#include <SBK_WB_CyclotronEngine_V2_0_0.h>
#include <SBK_WB_CyclotronScheme_V2_0_0.h>
#include <SBK_WB_RodEngine_V2_0_0.h>
#include <SBK_WB_IndicatorEngine_V2_0_0.h>

/******************************************************************************
 * POWER CELL DISPLAY (OPTIONAL)
 ******************************************************************************/

// --- USER SETTINGS: SELECT ZERO OR ONE DISPLAY TYPE -------------------------
// Leave both commented to build without a Power Cell display.
#define POWERCELL_STRIP
// #define POWERCELL_BARMETER

// --- INTERNAL SETUP / VALIDATION --------------------------------------------
#if (defined(POWERCELL_STRIP) + defined(POWERCELL_BARMETER)) > 1
#error "Power Cell type : multiple types defined in SBK_WRISBLASTER_CONFIG.h. Please select only one type : POWERCELL_STRIP or POWERCELL_BARMETER."
#endif
#ifndef POWERCELL_STRIP
const uint8_t POWERCELL_NUMLEDS = 0; // Default leds count if no strip enable
#endif
#if defined(POWERCELL_STRIP)

// --- EXTERNAL LIBRARY DEPENDENCY --------------------------------------------
#include <SBK_WB_PowerCellLEDsStripEngine_V2_0_0.h>

// --- USER SETTINGS: ADDRESSABLE LED STRIP -----------------------------------
const bool POWERCELL_DIRECTION = true; // false = forward, true = reverse
const uint8_t POWERCELL_NUMLEDS = 8;   // PowerCell pixels number
#define POWERCELL_ON_SAME_STRIP false  // true = PowerCell on the same strip as other blaster pixels, false = on a new strip alone
// Color order for a PowerCell connected to LEDS_STRIP2_PIN.
// This setting is ignored when POWERCELL_ON_SAME_STRIP is true; pixels sharing
// one data chain must use MAIN_STRIP_COLOR_ORDER.
// Pixel byte order choices: NEO_GRB, NEO_RGB, NEO_BGR, NEO_BRG, NEO_GBR, NEO_RBG
#define POWERCELL_STRIP_COLOR_ORDER NEO_RGB

// Used only when POWERCELL_ON_SAME_STRIP is true. Update the main LED indexes
// if the PowerCell pixels are inserted between other main-strip pixels.
const uint8_t POWERCELL_SHARED_STRIP_FIRST = 19;

// --- INTERNAL SETUP / VALIDATION --------------------------------------------
#if POWERCELL_ON_SAME_STRIP
#pragma message("Compiling for Power Cell with addressable RGB LEDs hooked with other wrist blaster leds. CHECK LEDS INDEX IF LEDS HAVE WRONG BEHAVIORS.")
const uint8_t POWERCELL_FIRST = POWERCELL_SHARED_STRIP_FIRST;
#else
#pragma message("Compiling for Power Cell with addressable RGB LEDs alone on a new strip.")
const uint8_t POWERCELL_FIRST = 0;
#endif
const uint8_t POWERCELL_LAST = POWERCELL_FIRST + POWERCELL_NUMLEDS - 1;
#elif defined(POWERCELL_BARMETER)

// --- EXTERNAL LIBRARY DEPENDENCY --------------------------------------------
#include <SBK_WB_PowerCellScheme_V2_0_0.h>

// --- USER SETTINGS: BAR METER DISPLAY ---------------------------------------
const uint8_t POWERCELL_SEG_NUMBER = 24;
const bool POWERCELL_DIRECTION = true; // false = forward, true = reverse

// Select exactly ONE driver/connection:
// #define PCBM_MAX72XX_SEPARATE     // Use a separate MAX72XX for the Power Cell Bar Meter with 3 pins serial communication : data, clock, load.
#define PCBM_MAX72XX_ON_PBM_CHAIN // Use the MAX72XX for the Power Cell Bar Meter on the same chain as the main Panel Bar Meter.
// #define PCBM_HT16K33                   // I2C LEDs driver like Adafruit backpack, uses 2 pins : SDA, SDC.

// Used only with PCBM_HT16K33:
#define PCBM_HT16K33_I2C_ADDRESS 0x71

// Select exactly ONE segment mapping:
#define SBK_BarMeter24_V0
// #define SBK_BarMeter24_V1

// --- INTERNAL SETUP / VALIDATION --------------------------------------------
#if (defined(PCBM_MAX72XX_SEPARATE) + defined(PCBM_MAX72XX_ON_PBM_CHAIN) + defined(PCBM_HT16K33)) != 1
#error "Power Cell Bar Meter Driver in SBK_WRISTBLASTER_CONFIG.h. Please select exactly one driver/connection option."
#endif
#if defined(PCBM_HT16K33)
#define PCBM_DRIVER_ADDRESS PCBM_HT16K33_I2C_ADDRESS
#define PCBM_BARDRIVE_DEVICE_INDEX 0

#if defined(PBM_HT16K33) && (PCBM_DRIVER_ADDRESS == PBM_DRIVER_ADDRESS)
#error "Power Cell and Panel Bar Meter HT16K33 drivers cannot use the same I2C address."
#endif
#pragma message("Compiling for Power Cell Bar Meter with HT16K33 driver")
#elif defined(PCBM_MAX72XX_SEPARATE) || defined(PCBM_MAX72XX_ON_PBM_CHAIN)
#if defined(PCBM_MAX72XX_ON_PBM_CHAIN)
#if !defined(PBM_MAX72XX)
#error "PCBM_MAX72XX_ON_PBM_CHAIN requires the Panel Bar Meter to use PBM_MAX72XX."
#endif
#define PCBM_DRIVER_ADDRESS 1
#define PCBM_BARDRIVE_DEVICE_INDEX 1
#pragma message("Power Cell Bar Meter uses device 1 of the Panel Bar Meter MAX72xx chain")
#elif defined(PCBM_MAX72XX_SEPARATE)
#define PCBM_DRIVER_ADDRESS 0
#define PCBM_BARDRIVE_DEVICE_INDEX 0
#pragma message("Power Cell Bar Meter uses a separate MAX72xx instance")
#endif
#endif

#if (defined(SBK_BarMeter24_V0) + defined(SBK_BarMeter24_V1)) != 1
#error "Power Cell Bar Meter Driver setup in SBK_WRISBLASTER_CONFIG.h : You must define one and only one segments mapping."
#elif defined(SBK_BarMeter24_V0)
constexpr uint8_t PCBM_BARDRIVE_ROWS = 3;
constexpr uint8_t PCBM_BARDRIVE_COLUMNS = 8;
#elif defined(SBK_BarMeter24_V1)
constexpr uint8_t PCBM_BARDRIVE_ROWS = 3;
constexpr uint8_t PCBM_BARDRIVE_COLUMNS = 8;
#endif
static_assert(PCBM_BARDRIVE_ROWS * PCBM_BARDRIVE_COLUMNS == POWERCELL_SEG_NUMBER,
              "PowerCell BarDrive matrix dimensions must match POWERCELL_SEG_NUMBER");
#else
#pragma message("Compiling without Power Cell.")
#endif

/******************************************************************************
 * SELECTED BAR-METER LIBRARIES
 * External dependencies selected by the Panel and PowerCell settings above.
 ******************************************************************************/

#if defined(PBM_MAX72XX) || defined(PCBM_MAX72XX_SEPARATE) || defined(PCBM_MAX72XX_ON_PBM_CHAIN)
#include <SBK_MAX72xxSoft.h>
#endif

#if defined(PBM_HT16K33) || defined(PCBM_HT16K33)
#include <SBK_HT16K33.h>
#endif

#define SBK_BARDRIVE_WITH_ANIM
#define SBK_BARDRIVE_QUEUE_CAPACITY 2
#include <SBK_BarDrive.h>

/******************************************************************************
 * BATTERY MONITORING
 ******************************************************************************/

// --- EXTERNAL DEPENDENCY ----------------------------------------------------
#include <SBK_WB_BattMonitoringEngine_V2_0_0.h>

// --- USER SETTINGS ----------------------------------------------------------
const bool POWER_MONITORING = ENABLE;
// Choices: NONE, LIPO_2S, LIPO_3S, NIMH_5S, NIMH_6S, NIMH_7S, NIMH_8S, NIMH_9S
BatteryType selectedBattery = NIMH_6S;
// When enabled, force LOW BATT state at the minimum voltage to protect the battery.
const bool BATT_LOW_CUTOFF = DISABLE;
// Battery-reading calibration in thousandths: 1000 = 1.000, 1025 = +2.5%, 975 = -2.5%.
const uint16_t BATT_READING_SCALING_FACTOR = 1000;

/******************************************************************************
 * AUDIO PLAYER
 ******************************************************************************/

// --- USER SETTINGS ----------------------------------------------------------
const uint8_t VOLUME_MAX = 30;   // 0-30; limit according to the amplifier
const uint8_t VOLUME_START = 20; // 0-30; startup level without a volume pot
const uint8_t PLAYER_COMMAND_DELAY = 150; // ms between player commands
const uint16_t PLAYER_BAUDRATE = 9600;
const uint8_t AUDIO_ADVANCE = 40; // ms; normally 25-50

// Uncomment only for experimentation. Track-length timing is recommended.
// #define PLAYER_USE_BUSY_PIN

// Disable if no volume potentiometer is connected; a floating input changes volume randomly.
const bool VOL_POT_READY = ENABLE;

// --- EXTERNAL DEPENDENCY ----------------------------------------------------
#include <SBK_WB_PlayerEngine_V2_0_0.h>

/******************************************************************************
 * BLASTER STATES (INTERNAL - DO NOT REORDER)
 ******************************************************************************/
/* IMPORTANT STATE-INDEX INVARIANT                                              */
/* Several tables use a BlasterState value directly as their array index:       */
/* TRACK_LENGTH, TRACK_LOOPING, CYCLOTRON_STATE_CONFIG, and                      */
/* POWERCELL_STATE_CONFIG. BlasterState values must therefore stay contiguous,  */
/* start at zero, and remain in exactly the same semantic order as every table. */
/* When adding, removing, or reordering a state, update every indexed table in  */
/* the same change. The static_assert checks catch count mismatches, but cannot  */
/* detect entries that have the correct count in the wrong semantic order.      */
/*  A state represents one audio-track context and its overall blaster behavior. */
/*  The blaster goes through states (switch/case loop) using flags and triggers. */
/*  A state may contain phases that coordinate animation/effect transitions    */
/*  within that same audio track without changing the main blaster state.      */
/*  Each state is handled by one main switch case; phases add a nested switch only where needed. */
/*  Possible blaster states for main loop switch cases: */
enum BlasterState : uint8_t
{
    STATE_ZERO = 0,                  // Not a state, just an offset to align with player (DFPlayer) track numbers
    STATE_PARTY_MODE_IN = 1,         // Intensify Switch is toggled ON, Blaster going into party mode, playing the intro track
    STATE_PARTY_MODE_OUT = 2,        // Intensify Switch is toggled OFF, Blaster going out of party mode, playing the outro track
    STATE_POWER_OFF_TO_ON = 3,       // "Main Power" switch has turned ON, the blaster is in the process of booting
    STATE_POWER_ON_TO_OFF = 4,       // "Main Power" switch has turned OFF, the blaster is in the process of shutting down
    STATE_POWER_ON = 5,              // The wrist blaster is ON, "Main Power" switch is ON, and blaster has finished booting
    STATE_CYCLOTRON_OFF_TO_ON = 6,   // "Cyclotron Power" switch has turned ON, the cyclotron is loading to REGULAR power...
    STATE_CYCLOTRON_ON_TO_OFF = 7,   // "Cyclotron Power" switch has turned OFF, the cyclotron is unloading from REGULAR power...
    STATE_CYCLOTRON_ON = 8,          // Cyclotron is idling at REGULAR power, ready to fire capture stream
    STATE_CYCLOTRON_ON_TO_FULL = 9,  // "Activate" switch has turned ON, the cyclotron is loading to FULL power...
    STATE_CYCLOTRON_FULL_TO_ON = 10, // "Activate" switch has turned OFF, the cyclotron is unloading to REGULAR power...
    STATE_CYCLOTRON_FULL_POWER = 11, // Cyclotron is idling at FULL power, ready to fire bursts
    STATE_CAPTURE = 12,              // Capture stream firing ramp to max fast and keep going...
    STATE_CAPTURE_TAIL = 13,         // Capture tail (no overheat)
    STATE_CAPTURE_OVERHEAT = 14,     // Capture stream warning before overheat
    STATE_BURST = 15,                // Firing burst
    STATE_BURST_OVERHEAT = 16,       // Burst firing with overheat warning
    STATE_ALL_ON_TO_OFF = 17,        // All systems transitioning from ON to OFF
    STATE_PARTY_MODE = 18,           // Intensify Switch is ON, Blaster in party mode playing music tracks
    STATE_POWER_OFF = 19,            // The blaster is OFF, or has finished shutting down
    STATE_LOW_BATT = 20              // Batteries are low, goes into low batt state
};

/******************************************************************************
 * SOUND TRACK TIMING
 ******************************************************************************/
/* Audio track-number definitions for the DFPlayer Mini sound board. */
/* Tracks should be mono WAV files named 001.wav, 002.wav, etc., and placed in the SD card root. */
/* The DFPlayer uses the files' copy order rather than their visible filenames. */
/* Track numbers map directly to blaster states; this order must be preserved. */
// --- USER SETTINGS ----------------------------------------------------------
/* Track lengths in milliseconds and index order; adjust them to match your audio files. */
/* They define state, animation, and effect timing. Without PLAYER_USE_BUSY_PIN, they also */
/* determine when audio playback ends, avoiding player track-length queries that may add delays. */
/* Exact track lengths can be measured with Audacity or similar audio software. */
/* Define the track lengths in milliseconds here: */
constexpr uint16_t TRACK_LENGTH[] = {
    0,     // No track, just an offset to be aligned with player (DFPlayer) track number starting from 1...
    1400,  // track #1, no loop, STATE_PARTY_MODE_IN
    2700,  // track #2, no loop, STATE_PARTY_MODE_OUT
    3000,  // track #3, no loop, STATE_POWER_OFF_TO_ON
    3000,  // track #4, no loop, STATE_POWER_ON_TO_OFF
    10000, // track #5, LOOP, STATE_POWER_ON
    3000,  // track #6, no loop, STATE_CYCLOTRON_OFF_TO_ON
    3000,  // track #7, no loop, STATE_CYCLOTRON_ON_TO_OFF
    10000, // track #8, LOOP, STATE_CYCLOTRON_ON
    2000,  // track #9, no loop, STATE_CYCLOTRON_ON_TO_FULL
    3000,  // track #10, no loop, STATE_CYCLOTRON_FULL_TO_ON
    10000, // track #11, LOOP, STATE_CYCLOTRON_FULL_POWER
    30000, // track #12, no loop, STATE_CAPTURE
    1550,  // track #13, no loop, STATE_CAPTURE_TAIL
    12500, // track #14, no loop, STATE_CAPTURE_OVERHEAT
    3500,  // track #15, no loop, STATE_BURST
    9500,  // track #16, no loop, STATE_BURST_OVERHEAT
    3000,  // track #17, no loop, STATE_ALL_ON_TO_OFF
    0,     // track #18, STATE_PARTY_MODE : no track, play files from folder 01 on SD card
    0,     // track #19, STATE_POWER_OFF : player OFF
    0      // track #20, STATE_LOW_BATT : player off
};

const bool TRACK_LOOPING[] = {
    false, // No track, just an offset to be aligned with player (DFPlayer) track number starting from 1...
    false, // track #1, no loop, STATE_PARTY_MODE_IN
    false, // track #2, no loop, STATE_PARTY_MODE_OUT
    false, // track #3, no loop, STATE_POWER_OFF_TO_ON
    false, // track #4, no loop, STATE_POWER_ON_TO_OFF
    true,  // track #5, LOOP, STATE_POWER_ON
    false, // track #6, no loop, STATE_CYCLOTRON_OFF_TO_ON
    false, // track #7, no loop, STATE_CYCLOTRON_ON_TO_OFF
    true,  // track #8, LOOP, STATE_CYCLOTRON_ON
    false, // track #9, no loop, STATE_CYCLOTRON_ON_TO_FULL
    false, // track #10, no loop, STATE_CYCLOTRON_FULL_TO_ON
    true,  // track #11, LOOP, STATE_CYCLOTRON_FULL_POWER
    false, // track #12, no loop, STATE_CAPTURE
    false, // track #13, no loop, STATE_CAPTURE_TAIL
    false, // track #14, no loop, STATE_CAPTURE_OVERHEAT
    false, // track #15, no loop, STATE_BURST
    false, // track #16, no loop, STATE_BURST_OVERHEAT
    false, // track #17, no loop, STATE_ALL_ON_TO_OFF
    false, // track #18, no loop, STATE_PARTY_MODE ---> NO TRACK, just a dummy
    false, // track #19, no loop, STATE_POWER_OFF ---> NO TRACK, just a dummy
    false  // track #20, no loop, STATE_BATT_LOW ---> NO TRACK, just a dummy
};

static_assert(sizeof(TRACK_LENGTH) / sizeof(TRACK_LENGTH[0]) == STATE_LOW_BATT + 1,
              "TRACK_LENGTH must contain one entry for every BlasterState");
static_assert(sizeof(TRACK_LOOPING) / sizeof(TRACK_LOOPING[0]) == STATE_LOW_BATT + 1,
              "TRACK_LOOPING must contain one entry for every BlasterState");

/******************************************************************************
 * STATE AND EFFECT TIMING
 ******************************************************************************/

// --- USER SETTINGS ----------------------------------------------------------
// Each state follows the timeline of its associated audio track. Phases are timed
// animation/effect transitions inside that state and do not start another track.

// STATE_CAPTURE_OVERHEAT & STATE_BURST_OVERHEAT have phases for handling
// overheat warnings, actual overheat and cooling
enum OverheatPhase : uint8_t
{
    PHASE_WARNING,  // Firing with an imminent overheat warning
    PHASE_OVERHEAT, // The blaster has overheated, is temporarily disabled and in venting mode
    PHASE_COOLING   // The blaster is resetting and continues cooling down
};

constexpr uint16_t CAPTURE_OVERHEAT_PHASE_LENGTH[] = {
    6500, // Warning phase length
    1500, // Overheat phase length
    4500  // Cooling phase length
};
static_assert((uint32_t)CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_WARNING] +
                      CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT] +
                      CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_COOLING] <=
                  TRACK_LENGTH[STATE_CAPTURE_OVERHEAT],
              "Capture Overheat phases exceed the Capture Overheat track length");
static_assert((uint32_t)CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT] +
                      CAPTURE_OVERHEAT_PHASE_LENGTH[PHASE_COOLING] >=
                  1000U,
              "Capture overheat and cooling phases must leave at least 1000 ms for the Cyclotron ramp");

constexpr uint16_t BURST_OVERHEAT_PHASE_LENGTH[] = {
    3500, // Warning phase length
    1500, // Overheat phase length
    4500  // Cooling phase length
};
static_assert((uint32_t)BURST_OVERHEAT_PHASE_LENGTH[PHASE_WARNING] +
                      BURST_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT] +
                      BURST_OVERHEAT_PHASE_LENGTH[PHASE_COOLING] <=
                  TRACK_LENGTH[STATE_BURST_OVERHEAT],
              "Burst Overheat phases exceed the Burst Overheat track length");
static_assert((uint32_t)BURST_OVERHEAT_PHASE_LENGTH[PHASE_OVERHEAT] +
                      BURST_OVERHEAT_PHASE_LENGTH[PHASE_COOLING] >=
                  1000U,
              "Burst overheat and cooling phases must leave at least 1000 ms for the Cyclotron ramp");

// BURST sequence has phases for handling firing and tail
enum BurstPhase : uint8_t
{
    BURST_PHASE_FIRING,
    BURST_PHASE_TAIL
};
constexpr uint16_t BURST_PHASE_LENGTH[] = {
    1500, // Firing phase
    2000  // Tail phase
};
static_assert((uint32_t)BURST_PHASE_LENGTH[BURST_PHASE_FIRING] +
                      BURST_PHASE_LENGTH[BURST_PHASE_TAIL] <=
                  TRACK_LENGTH[STATE_BURST],
              "Burst firing and tail phases exceed the Burst track length");

// --- Additional state/effect settings ---------------------------------------
// Additional portions of state timelines that need to be configured:
const uint16_t DURATION_CAPTURE_TAKEOFF_RAMP = 1000; // Minimum Capture ramp time before entering the warning phase
// CAPTURE shot max length before going into overheat
const uint16_t DURATION_CAPTURE_MAX = 20000; // 10000 ms to the Capture track length: maximum firing time before overheat
// MAX BURST shot before going into overheat
const uint8_t MAX_BURST_SHOTS = 5;                          // How many shot before overheat
const uint8_t PBM_BLOCK_SPACING = (1 + PBM_SEG_NUMBER / 2); // Block spacing for PBM animations
const uint8_t PBM_BLOCK_ENDING_OFFSET = 100;                // Block stop emission offset for PBM animations in ms
const uint16_t PBM_BLOCK_ENDING_DELAY = 750;                // Block ending delay for PBM animations in ms
