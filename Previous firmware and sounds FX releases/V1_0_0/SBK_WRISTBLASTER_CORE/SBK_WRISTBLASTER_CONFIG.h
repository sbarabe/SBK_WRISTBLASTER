/**
 * @file        SBK_WRISTBLASTER_CONFIG.h
 * @brief       Configuration file for the SBK_WRISTBLASTER_CORE system.
 *
 * @author      Samuel Barabé
 * @copyright   Copyright (c) 2025-2026 Samuel Barabé
 * @license     MIT License
 * @version     1.1.0
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
 * @see https://github.com/PowerBroker2/DFPlayerMini_Fast
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
 * - Audio timing is based on track durations defined in this file — not on the BUSY pin.
 * - Incorrect durations may result in animation/audio desynchronization.
 * - Advanced users may modify animations or state transitions in the core engine logic.
 *
 *  TIP: Save a backup of this file after customization.
 *
 ***********************************************************************************************/

#pragma once

#include <Arduino.h>

/*********************************************/
/*                                           */
/*              DEBUG TO SERIAL              */
/*                                           */
/*********************************************/
// DEBUG TO SERIAL information about wrist blaster states and stages this engine controlled components
// Uncomment/comment the following line to send/stop DEBUG_TO_SERIAL_CORE info to serial
//**********************************************************
// #define DEBUG_TO_SERIAL
//**********************************************************
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

/*********************************************/
/*                                           */
/*          MCU PINS CONFIGURATION           */
/*                                           */
/*********************************************/
// Define (UNCOMMENT) only one PCB or define custom pins definition in SBK_WRISTBLASTER_PINS_DEF.h
/*********************************************/
// #define CUSTOM_PINS_DEFINITION
// #define SBK_WRIST_BLASTER_PCB_V2
// #define SBK_WRIST_BLASTER_II_PCB_Vx
#define SBK_PROPCORE_ONE_Vx
// #define SBK_PROPCORE_ONE_PLUS_Vx
/*********************************************/
#include "SBK_WRISTBLASTER_PINS_DEF.h"
// Ensure only one PCB is defined
#if (defined(CUSTOM_PINS_DEFINITION) + defined(SBK_WRIST_BLASTER_PCB_V2) + defined(SBK_WRIST_BLASTER_II_PCB_Vx) + defined(SBK_PROPCORE_ONE_Vx) + defined(SBK_PROPCORE_ONE_PLUS_Vx)) != 1
#error "SBK_WRISTBLASTER_CONFIG.h : You must define a PCB, and exactly one PCB type only!"
#endif

/*********************************************/
/*          SWITCHES AND BUTTONS             */
/*********************************************/
#include "SBK_WB_SwitchEngine_V1_1_0.h"
// INTENSIFY BUTTON OR SWITCH
// If your Intensify push button is replaced by a switch in your Wrist Blaster, set this to "true".
// Intensify is a Push Button, set this to "false".
const bool INTENSIFY_IS_A_SWITCH = false;
SwitchLogicType INTENSIFY_PB_LOGIC = DIRECT_LOGIC; // Except if replace by a switch Intensify PB should always have DIRECT_LOGIC
// Define others switches/buttons logic here :
// If your switch level isn't the way you want, you could reverse it here instead of redoing your wires.
SwitchLogicType MAIN_POWER_SW_LOGIC = DIRECT_LOGIC;      // DIRECT_LOGIC / REVERSE_LOGIC
SwitchLogicType CYCLOTRON_POWER_SW_LOGIC = DIRECT_LOGIC; // DIRECT_LOGIC / REVERSE_LOGIC
SwitchLogicType ACTIVATE_SW_LOGIC = DIRECT_LOGIC;        // DIRECT_LOGIC / REVERSE_LOGIC
SwitchLogicType FIRE_PB_LOGIC = DIRECT_LOGIC;            // PB should always have DIRECT LOGIC

/*********************************************/
/*               SMOKE MACHINE               */
/*********************************************/
// **** SMOKE DEVICE HARDWARE AND CODE NOT TESTED YET ****
// Enable or disable all smoke-related features here :
/***********************************************************/
// #define SMOKE_FEATURES_ENABLED
/***********************************************************/
#ifdef SMOKE_FEATURES_ENABLED
#include "SBK_WB_SmokeEngine_V1_1_0.h"
// DEFINE the maximum time that the smoker can be ON
const uint16_t SMOKER_MAX_ON_TIME = 15000; // in ms
// DEFINE the minimum OFF time of the smoker, to prevent short cycling
const uint16_t SMOKER_MIN_OFF_TIME = 10000; // in ms
// Smoker is activated, if minimum off time is respected, when the wrist blaster goes into
// STATE_CAPTURE_OVERHEAT and stop after STATE_OVERHEATED tail
// SMOKE ENABLING :
// Smoke effects are DISABLE when MCU is powered, you need to activate them in POWER OFF STATE with the Fire Button.
// Hold fire button, it will show the actual state on TOP WHITE INDICATOR : Red = off, Green = ON;
// If you hold the button for 3 second, the ENABLING/DISABLING will switch and you'll see the
// indicator switching color for the new enable state.
// HARWARE :
// Check SBK_WRISTBLASTER_PINS_DEF.h for the correct pin assignments.
// There should be pins for fan and smoke device activation
#endif // SMOKE_FEATURES_ENABLED

/*********************************************/
/*           BAR METER & DRIVER(s)           */
/*********************************************/
#include "SBK_WB_PanelBarMeterEngine_V1_1_0.h"
// DEFINE BAR METER TOTAL NUMBER OF SEGEMENTS
const uint8_t PBM_SEG_NUMBER = 28;
const bool PBM_DIRECTION = FORWARD; // animation direction (FORWARD/REVERSE)
// SELECT (uncomment) ONE DRIVER TYPE :
/***********************************************************/
#define PBM_MAX72XX // MAX7219/7221 uses 3 pins serial communication : data, clock, load.
// #define PBM_HT16K33 // I2C LEDs driver like Adafruit backpack, uses 2 pins : SDA, SDC.
/***********************************************************/
#if (defined(PBM_MAX72XX) + defined(PBM_HT16K33)) != 1
#error "Panel Bar Meter LEDs driver definition in SBK_WRISTBLASTER_CONFIG.h file. Please define exactly ONE driver type (PBM_MAX72XX or PBM_HT16K33)."
#elif defined(PBM_MAX72XX)
#define PBM_DRIVER_ADDRESS 0 // first driver in the chain
#pragma message("Compiling for Panel Bar Meter with MAX72xx driver")
#elif defined(PBM_HT16K33)
#pragma message("Compiling for Panel Bar Meter with HT16K33 driver")
/***********************************************************/
/* DEFINE DRIVER I2C ADDRESS IF REQUIRED     */
#define PBM_DRIVER_ADDRESS 0x70 // for I2C drivers type
/***********************************************************/
#endif

/*********************************************/
/*        BAR METER SEGMENTS MAPPING         */
/*********************************************/
// BAR METER SEGMENTS MAPPING FOR THE DRIVER
// DEFINE SEGMENTS MAPPING on bar meter driver {ROW,COL}.
// SELECT (uncomment) ONLY ONE mapping type below :
/**************************************************/
#define PBM_SK_MAPPING // Common anode bar meter type SK
// #define PBM_SA_MAPPING // Common cathode bar meter type SA
/**************************************************/
#ifdef PBM_SK_MAPPING
//  MAPPING 1 matrix definition, more associated with common cathode SK bar meter
//  This mapping works for this MAX72xx driver PCB "SBK_WB_BG_SK_PCB_Vx"
//  with bar meter holder PCB "SBK_WB_BG_28SEG_PCB_Vx"
const uint8_t PBM_SEG_MAP[28][2] = {
    {0, 0}, // SEG #1
    {0, 1}, // SEG #2
    {0, 2}, // SEG #3
    {0, 3}, // SEG #4
    {1, 0}, // SEG #5
    {1, 1}, // SEG #6
    {1, 2}, // SEG #7
    {1, 3}, // SEG #8
    {2, 0}, // SEG #9
    {2, 1}, // SEG #10
    {2, 2}, // SEG #11
    {2, 3}, // SEG #13
    {3, 0}, // SEG #12
    {3, 1}, // SEG #14
    {3, 2}, // SEG #15
    {3, 3}, // SEG #16
    {4, 0}, // SEG #17
    {4, 1}, // SEG #18
    {4, 2}, // SEG #19
    {4, 3}, // SEG #20
    {5, 0}, // SEG #21
    {5, 1}, // SEG #22
    {5, 2}, // SEG #23
    {5, 3}, // SEG #24
    {6, 0}, // SEG #25
    {6, 1}, // SEG #26
    {6, 2}, // SEG #27
    {6, 3}  // SEG #28
};
#elif defined(PBM_SA_MAPPING)
//  MAPPING 2 matrix definition, more associated with common cathode SA bar meter
//  This mapping works for this MAX72xx driver PCB "SBK_WB_BG_SA_PCB_Vx"
//  with bar meter holder PCB "SBK_WB_BG_28SEG_PCB_Vx"
const uint8_t PBM_SEG_MAP[28][2] = {
    {0, 0}, // SEG #1
    {1, 0}, // SEG #2
    {2, 0}, // SEG #3
    {3, 0}, // SEG #4
    {0, 1}, // SEG #5
    {1, 1}, // SEG #6
    {2, 1}, // SEG #7
    {3, 1}, // SEG #8
    {0, 2}, // SEG #9
    {1, 2}, // SEG #10
    {2, 2}, // SEG #11
    {3, 2}, // SEG #13
    {0, 3}, // SEG #12
    {1, 3}, // SEG #14
    {2, 3}, // SEG #15
    {3, 3}, // SEG #16
    {0, 4}, // SEG #17
    {1, 4}, // SEG #18
    {2, 4}, // SEG #19
    {3, 4}, // SEG #20
    {0, 5}, // SEG #21
    {1, 5}, // SEG #22
    {2, 5}, // SEG #23
    {3, 5}, // SEG #24
    {0, 6}, // SEG #25
    {1, 6}, // SEG #26
    {2, 6}, // SEG #27
    {3, 6}  // SEG #28
};
#endif

/*********************************************/
/*            WS2812 lEDS STRIP              */
/*********************************************/
#include "SBK_WB_LedsStripBaseEngine_V1_1_0.h"
const uint8_t TOTAL_LEDS_NUMBER = 19; // vent + indicators + firing jewel + cyclotron total WS21812 pixels
/***********************************************/
/*             LEDS INDEX                      */
/***********************************************/
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
/***********************************************/
/*                  VENT LED                   */
/***********************************************/
#include "SBK_WB_VentEngine_V1_1_0.h"
/***********************************************/
/*               CYCLOTRON LEDs                */
/***********************************************/
#include "SBK_WB_CyclotronEngine_V1_1_0.h"
const bool CYCLOTRON_DIRECTION = FORWARD; // animation direction (FORWARD/REVERSE)
// Cyclotron ring and center positions pixels for the index in the WS2812 wrist blaster chain
// Cyclotron jewel has 7 pixels
const uint8_t CYC_NUMLEDS = 7;
// Identify specific pixel order on the jewel
const uint8_t CYC_RING_1ST = 1;
const uint8_t CYC_RING_LAST = 6;
const uint8_t CYC_CENTER = 0;
/***********************************************/
/*               FIRE ROD LEDs                */
/***********************************************/
#include "SBK_WB_RodEngine_V1_1_0.h"
// Fire Rod jewel has 7 pixels
const uint8_t ROD_NUMLEDS = 7;
// HUE POTENTIOMETER :
// If you have no potentiometer hooked to the FIRE_ROD_POT_PIN, disable this feature.
// If enabled and no pot on the pin, fire rod hue will change erratically.
const bool HUE_POT_READY = ENABLE;
/***********************************************/
/*               INDICATORS LEDs               */
/***********************************************/
#include "SBK_WB_IndicatorEngine_V1_1_0.h"
// Indicator flashing speeds
const uint8_t FAST_BLINK_SP = 100;
const uint16_t MEDIUM_BLINK_SP = 500;
const uint16_t SLOW_BLINK_SP = 1000;
/***********************************************/
/*           FIRE BUTTON SINGLE LED            */
/***********************************************/
// This is not an addressable LED. It is a single LED driven directly from the MCU pin through a series resistor.
// Keep the LED current below 15mA on an Arduino Nano Every pin (other MCUs may have different current limits).
// The yellow LED typically used has a forward voltage of about 2.0V.
// With a 5V MCU output, a 330 Ohm resistor limits the LED current to approximately 9mA.
// You can disable this indicator here:
const bool FIRE_BUTTON_LED_READY = ENABLE;

/***********************************************************/
/* POWER CELL LEDS STRIP or BAR METER DISPLAY  (OPTIONAL)  */
/***********************************************************/

/***********************************************************/
// If you do have a "Power Cell LEDs strip" or a "Power Cell Bar Meter Display" on your Wrist Blaster set up,
// UNCOMMENT the right type below :
 #define POWERCELL_STRIP
// #define POWERCELL_BARMETER
/***********************************************************/

#if (defined(POWERCELL_STRIP) + defined(POWERCELL_BARMETER)) > 1
#error "Power Cell type : multiple types defined in SBK_WRISBLASTER_CONFIG.h. Please select only one type : POWERCELL_STRIP or POWERCELL_BARMETER."
#endif
#ifndef POWERCELL_STRIP
const uint8_t POWERCELL_NUMLEDS = 0; // Default leds count if no strip enable
#endif
#if defined(POWERCELL_STRIP)
#include "SBK_WB_PowerCellLEDsStripEngine_V1_1_0.h"
                                     // If PowerCell LEDs strip exist, define the following parameters :
// Define the leds number in your PowerCell strip and animation DIRECTION.
const bool POWERCELL_DIRECTION = REVERSE; // animation direction (FORWARD/REVERSE).
const uint8_t POWERCELL_NUMLEDS = 8;      // PowerCell pixels number
#define POWERCELL_ON_SAME_STRIP false     // true = PowerCell on the same strip as other blaster pixels, false = on a new strip alone
#if POWERCELL_ON_SAME_STRIP
#pragma message("Compiling for Power Cell with addressable RGB LEDs hooked with other wrist blaster leds. CHECK LEDS INDEX IF LEDS HAVE WRONG BEHAVIORS.")
                                     // Define first led index on the strip :
const uint8_t POWERCELL_FIRST = 19; // IMPORTANT : Also check LEDS INDEX above in this code if the PowerCell LEDS are hooked in between other pixels.
#else
#pragma message("Compiling for Power Cell with addressable RGB LEDs alone on a new strip.")
const uint8_t POWERCELL_FIRST = 0;
#endif
const uint8_t POWERCELL_LAST = POWERCELL_FIRST + POWERCELL_NUMLEDS - 1;
#elif defined(POWERCELL_BARMETER)
#include "SBK_WB_PowerCellBarMeterEngine_V1_1_0.h"
const uint8_t POWERCELL_SEG_NUMBER = 24;
const bool POWERCELL_DIRECTION = REVERSE; // animation direction (FORWARD/REVERSE).

/***********************************************************/
// SELECT exactly ONE Power Cell Bar Meter connection:
// #define PCBM_MAX72XX_SEPARATE     // Use a separate MAX72XX for the Power Cell Bar Meter with 3 pins serial communication : data, clock, load.
#define PCBM_MAX72XX_ON_PBM_CHAIN // Use the MAX72XX for the Power Cell Bar Meter on the same chain as the main Panel Bar Meter.
// #define PCBM_HT16K33                   // I2C LEDs driver like Adafruit backpack, uses 2 pins : SDA, SDC.
/***********************************************************/

#if (defined(PCBM_MAX72XX_SEPARATE) + defined(PCBM_MAX72XX_ON_PBM_CHAIN) + defined(PCBM_HT16K33)) != 1
#error "Power Cell Bar Meter Driver in SBK_WRISTBLASTER_CONFIG.h. Please select exactly one driver/connection option."
#endif
#if defined(PCBM_HT16K33)

/***********************************************************/
/* DEFINE DRIVER I2C ADDRESS IF REQUIRED     */
#define PCBM_DRIVER_ADDRESS 0x71 // for I2C drivers type
/***********************************************************/

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
#pragma message("Power Cell Bar Meter uses device 1 of the Panel Bar Meter MAX72xx chain")
#elif defined(PCBM_MAX72XX_SEPARATE)
#define PCBM_DRIVER_ADDRESS 0
#pragma message("Power Cell Bar Meter uses a separate MAX72xx instance")
#endif
#endif

/***********************************************************/
// Power Cell Bar Meter LEDs mapping for driver
// DEFINE only one mapping type for PowerCell Bar Meter :
#define SBK_BarMeter24_V0
// #define SBK_BarMeter24_V1
/**********************************************************/

#if (defined(SBK_BarMeter24_V0) + defined(SBK_BarMeter24_V1)) != 1
#error "Power Cell Bar Meter Driver setup in SBK_WRISBLASTER_CONFIG.h : You must define one and only one segments mapping."
#elif defined(SBK_BarMeter24_V0)
const uint8_t PCBM_SEG_MAP[24][2] = {
    {0, 0}, // SEG #8
    {0, 1}, // SEG #1
    {0, 2}, // SEG #2
    {0, 3}, // SEG #3
    {0, 4}, // SEG #4
    {0, 5}, // SEG #5
    {0, 6}, // SEG #6
    {0, 7}, // SEG #7
    {1, 0}, // SEG #16
    {1, 1}, // SEG #9
    {1, 2}, // SEG #10
    {1, 3}, // SEG #11
    {1, 4}, // SEG #12
    {1, 5}, // SEG #13
    {1, 6}, // SEG #14
    {1, 7}, // SEG #15
    {2, 0}, // SEG #24
    {2, 1}, // SEG #17
    {2, 2}, // SEG #18
    {2, 3}, // SEG #19
    {2, 4}, // SEG #20
    {2, 5}, // SEG #21
    {2, 6}, // SEG #22
    {2, 7}  // SEG #23
};
#elif defined(SBK_BarMeter24_V1)
const uint8_t PCBM_SEG_MAP[24][2] = {
    {0, 0}, // SEG #1
    {0, 1}, // SEG #2
    {0, 2}, // SEG #3
    {0, 3}, // SEG #4
    {0, 4}, // SEG #5
    {0, 5}, // SEG #6
    {0, 6}, // SEG #7
    {0, 7}, // SEG #8
    {1, 0}, // SEG #9
    {1, 1}, // SEG #10
    {1, 2}, // SEG #11
    {1, 3}, // SEG #13
    {1, 4}, // SEG #12
    {1, 5}, // SEG #14
    {1, 6}, // SEG #15
    {1, 7}, // SEG #16
    {2, 0}, // SEG #17
    {2, 1}, // SEG #18
    {2, 2}, // SEG #19
    {2, 3}, // SEG #20
    {2, 4}, // SEG #21
    {2, 5}, // SEG #22
    {2, 6}, // SEG #23
    {2, 7}  // SEG #24
};
#endif
/***********************************************************/
#else
#pragma message("Compiling without Power Cell.")
#endif

/***********************************************************/
/*                POWER MONITORING OPTIONS                 */
/***********************************************************/
#include "SBK_WB_BattMonitoringEngine_V1_1_0.h"
const bool POWER_MONITORING = DISABLE;         // Define if battery power measurement is ENABLE/DISABLE.
BatteryType selectedBattery = NONE;            // Define battery type to set the correct measurement range : NONE, LIPO_2S, LIPO_3S, NIMH_5S , NIMH_6S , NIMH_7S, NIMH_8S, NIMH_9S
const bool BATT_LOW_CUTOFF = DISABLE;          // Define if the Wrist Blaster force to LOW BATT state when battery range is in minimum (prevent battery dropping tow low and improve battery life)
const float BATT_READING_SCALING_FACTOR = 1.0; // If the batt reading seems off (using Serial Debug to check readings), you could used this scaling factor to adjust it.

/*********************************************/
/*                                           */
/*    AUDIO PLAYER definition and helpers    */
/*                                           */
/*********************************************/
#include <SoftwareSerial.h>
#include "SBK_WB_PlayerEngine_V1_1_0.h"
const uint8_t VOLUME_MAX = 30;            // 0-30 If you want to reduce the maximum possible volume according to your amp module, set this here
const uint8_t VOLUME_START = 20;          // 0-30 Volume at start-up, will not change if volume potentiometer doesn't exist
const uint8_t PLAYER_COMMAND_DELAY = 150; // short delay between query/ commands : some player(s) will behave weirdly if there is no delay
const uint16_t PLAYER_BAUDRATE = 9600;    // Native baudrate is 9600 for this player.
// AUDIO ADVANCE
// A short advance to call the next track before the real ending :
// DFPlayer doesn't like when a command is call exactly at the end of a file,
// it's not listening at this moment and may miss the command causing erratic behaviors.
// If you got erratic playing behaviours, try to increase this advance a bit at the time :
// if it's too much, it's gonna cut the tracks a bit in the end...
const uint8_t AUDIO_ADVANCE = 40; // 25-50ms
// USING BUSY PIN INSTEAD OF TRACK LENGTHS
// Uses Busy Pin instead of track length in the logic.
// There is a small moment at the end of a playing track when the player is not responding to command.
// Using Busy Pin sometime make the play commands fall into that moment and cause erratic play commands.
const bool BUSY_PIN_READY = DISABLE; // ENABLING NOT RECOMMENDED : this option is left there for coders who would like to explore this avenue...
// VOLUME POTENTIOMETER :
// If you have no volume potentiometer hooked to the VOL_POT_PIN, disable this feature.
// If enabled and no pot on the pin, volume will change erratically.
const bool VOL_POT_READY = ENABLE;

/****************************/
/*     BLASTER STATES LIST  */
/****************************/
/*  Blaster goes through states (switch/case loop) using flags and triggers. */
/*  Each state is a case, and each state case contain an other switch/case : initital step case for this state, other possible steps and exit steps. */
/*  Possible blaster states for main loop switch cases: */
enum BlasterState : uint8_t
{
    STATE_ZERO = 0,                      // Not a state, just an offset to align with player (DFPlayer) track numbers
    STATE_PARTY_MODE_IN = 1,             // Intensify Switch is toggled ON, Blaster going into party mode, playing the intro track
    STATE_PARTY_MODE_OUT = 2,            // Intensify Switch is toggled OFF, Blaster going out of party mode, playing the outro track
    STATE_POWER_OFF_TO_ON = 3,           // "Main Power" switch has turned ON, the blaster is in the process of booting
    STATE_POWER_ON_TO_OFF = 4,           // "Main Power" switch has turned OFF, the blaster is in the process of shutting down
    STATE_POWER_ON = 5,                  // The wrist blaster is ON, "Main Power" switch is ON, and blaster has finished booting
    STATE_CYCLOTRON_OFF_TO_ON = 6,       // "Cyclotron Power" switch has turned ON, the cyclotron is loading to REGULAR power...
    STATE_CYCLOTRON_ON_TO_OFF = 7,       // "Cyclotron Power" switch has turned OFF, the cyclotron is unloading from REGULAR power...
    STATE_CYCLOTRON_ON = 8,              // Cyclotron is idling at REGULAR power, ready to fire capture stream
    STATE_CYCLOTRON_ON_TO_FULL = 9,      // "Activate" switch has turned ON, the cyclotron is loading to FULL power...
    STATE_CYCLOTRON_FULL_TO_ON = 10,     // "Activate" switch has turned OFF, the cyclotron is unloading to REGULAR power...
    STATE_CYCLOTRON_FULL_POWER = 11,     // Cyclotron is idling at FULL power, ready to fire bursts
    STATE_CAPTURE = 12,                  // Capture stream firing ramp to max fast and keep going...
    STATE_CAPTURE_TAIL = 13,             // Capture tail (no overheat)
    STATE_CAPTURE_WARNING_OVERHEAT = 14, // Capture stream warning before overheat
    STATE_BURST = 15,                    // Firing burst
    STATE_BURST_OVERHEAT = 16,           // Burst firing with overheat warning
    STATE_ALL_ON_TO_OFF = 17,            // All systems transitioning from ON to OFF
    STATE_PARTY_MODE = 18,               // Intensify Switch is ON, Blaster in party mode playing music tracks
    STATE_POWER_OFF = 19,                // The blaster is OFF, or has finished shutting down
    STATE_LOW_BATT = 20                  // Batteries are low, goes into low batt state
};

/****************************/
/*     SOUND FX TRACKS      */
/****************************/
/* Audio track files numbers definition for soundboards DFPlayer mini  */
/* Tracks should be mono WAVE type file, named 001.wav, 002.wav, etc. They should be place in the SD card */
/* root folder. They are played in the order they have been put on the flash drive, not by trackname */
/* Also, TRACK NUMBERS FIT PACK STATES, it's part of the core program, this order should be maintained/corrected that way */
/****************************************/
/* SOUND FX TRACKS LENGTHS AND LOOPING  */
/****************************************/
/* Tracks milliseconds lengths in index order : must be changed according to the yours tracks. */
/*  Those are used to determine the track's playing end in the CORE main loop to minimize delay in switching sound FX tracks, no BUSY pin is used. */
/*  It also prevent using the get track length functions that could cause some delay with some players */
/*  You can get your exact track lengths in Audacity or others audio software*/
/*  DEFINE the tracks lengths in milliseconds here :*/
const uint16_t TRACK_LENGTH[] = {
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
    12500, // track #14, no loop, STATE_CAPTURE_WARNING_OVERHEAT
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
    false, // track #14, no loop, STATE_CAPTURE_WARNING_OVERHEAT
    false, // track #15, no loop, STATE_BURST
    false, // track #16, no loop, STATE_BURST_OVERHEAT
    false, // track #17, no loop, STATE_ALL_ON_TO_OFF
    false, // track #18, no loop, STATE_PARTY_MODE ---> NO TRACK, just a dummy
    false, // track #19, no loop, STATE_POWER_OFF ---> NO TRACK, just a dummy
    false  // track #20, no loop, STATE_BATT_LOW ---> NO TRACK, just a dummy
};

/*******************************/
/* SOME STATE/STAGE PARAMETERS */
/*******************************/
// SECTIONS of track length that need to be defined :
const uint16_t DURATION_CAPTURE_OVERHEAT = 7500;     // CAPTURE_WARNING_OVERHEAT track overheat section duration
const uint16_t DURATION_BURST_TAIL = 2000;           // BURST track tail section duration
const uint16_t DURATION_BURST_OVERHEAT = 7500;       // BURST_OVERHEAT track overheat section duration
const uint16_t DURATION_CAPTURE_TAKEOFF_RAMP = 1000; // To give time for the capture shot to ramp in before going in warning stage
// CAPTURE shot max length before going into overheat
const uint16_t DURATION_CAPTURE_MAX = 20000; // between 10000ms to 30000ms (capture track length) : how long before going into overheat in capture shot
// MAX BURST shot before going into overheat
const uint8_t MAX_BURST_SHOTS = 5; // How many shot before overheat
