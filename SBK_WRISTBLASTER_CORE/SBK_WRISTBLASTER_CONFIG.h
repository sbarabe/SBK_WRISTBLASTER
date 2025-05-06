/*
 *  This code is part of SBK_WRISTBLASTER_CORE (VERSION 0), a codebase for animations and effects
 *  for a Wrist Blaster prop inspired by the movie Ghostbusters: Frozen Empire.
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

/**********************************************************************************************
 *  SEE GENERAL INFO IN SBK_WRISTBLASTER_CORE.ino FOR A FULL OVERVIEW OF HOW TO USE THIS CODE.
 *
 *  THIS IS THE CONFIGURATION FILE.  
 *  - All your hardware and functional settings should be made here.  
 *  - The main core file (SBK_WRISTBLASTER_CORE.ino) should remain untouched.  
 *  - This approach keeps your code modular, easier to update, and cleaner to maintain.  
 *
 *  MAKE SURE TO SAVE A BACKUP OF YOUR CONFIGURATION FILE IF YOU CUSTOMIZE IT.
 *
 ***********************************************************************************************/

/**********************************************************************************************
 *  
 *  THIS CONFIG FILE PARAMETERS ARE SET FOR THIS HARDWARE:
 * 
 *      MCU board -> Arduino Nano Every:    The Nano Every is a 5V board with the same pinout and
 *                                          footprint as the Nano, but with more memory and a faster MCU.
 *                                          The code is too large for a regular Nano or even the Uno, 
 *                                          it will not compile on those boards.
 * 
 *      Mains PCB -> SBK_WRISTBLASTER_II V0
 *      Bar Meter Driver -> MAX7219 or MAX7221: the latter induces less voltage noise/ripples that 
 *                                              could affect audio quality.
 *      Bar Meter Driver PCBs -> SBK_WB_BG_SK_DR V0
 *      Bar Meter Holder PCB -> SBK_WB_BG_28SEG V0
 *      Bar Meter -> BL28Z-3005SK (common cathode)
 *      Fire Button -> With a LED indicator: The LED should draw no more than 20mA, with the appropriate
 *                                           series resistor to limit forward voltage. The SBK_WRISTBLASTER_II V0
 *                                           PCB provides space for a LED resistor. For example, a yellow 20mA,
 *                                           1.8-2.0V forward voltage LED should use a 150-ohm series resistor for
 *                                           a 5V driving signal from the MCU pin.
 *      Intensify Switch/Button -> Set up for a SWITCH, but can be modified to use a PUSH BUTTON setup in the 
 *                                 SWITCHES AND BUTTONS section.
 *      Software Volume Potentiometer -> Enable: If there is no potentiometer attached to this pin, 
 *                                               the volume may vary unpredictably.
 *      Firing Strobe Hue Potentiometer -> Enable: If there is no potentiometer attached to this pin,
 *                                                 the hue may vary because the analog value of a floating pin is undetermined.
 *      Smoke Module Control -> The fan and pump can be connected to the driver pins. The smoke module should be 
 *                              activated via a vacuum or with a proper external device (relay/MOSFET) driven
 *                              by the pump signal.
 *      Sound FX Tracks (as provided in the example) -> The code is based on track lengths. If you change the tracks, you will need to adjust the track lengths in the SOUND FX TRACK LENGTHS AND LOOPING sections.
 *                                                      You should also adjust the SOME STATE/STAGE PARAMETERS section where certain lengths are defined relative to the sound FX.
 *  
 *   All of the above can be modified in this file without needing to change the main core code.
 * 
 ***********************************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/*********************************************/
/*                                           */
/*              DEBUG TO SERIAL              */
/*                                           */
/*********************************************/
// DEBUG TO SERIAL information about wrist blaster states and stagesthis engine controlled components
// Uncomment/comment the following line to send/stop DEBUG_TO_SERIAL_CORE info to serial

// #define DEBUG_TO_SERIAL

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
// Update the pins according to your MCU and connections
// ARDUINO NANO EVERY PINS DEFINITION
#define SW_RX_PIN 2     // If Software Serial is used, SoftSerial receiving pinto audio board Tx pin
#define SW_TX_PIN 3     // If Software Serial is used, SoftSerial transmitting pin to audio board Rx pin
#define BUSY_PIN 12     // Audio player BUSY pin - NOT USE IN THIS CODE
#define AMP_MUTE_PIN 13 // Onboard Amplifier mute pin - NOT USE IN THIS CODE
// Bar Meter driver pins
#define BM_DIN_PIN A4  // connected to bar meter driver DataIn pin / SDA in case of I2C driver
#define BM_CLK_PIN A5  // connected to bar meter driver Clock pin / SCL in cas of I2C driver
#define BM_LOAD_PIN A3 // connected to bar meter driver Load pin - NOT USED for I2C driver
// WS2812 Leds chain and others leds pins
#define LEDS_PIN A0            // for WS2812 LEDs chain
#define FIRE_BUTTON_LED_PIN 11 // #1 on PCB Switches header, with an in line resistor to protect led : illumaneted fire button led output (yellow)
// Buttons and switches
#define FIRE_BUTTON_PIN 10           // #2 on PCB Switches header, should be the Fire Button
#define ACTIVATE_SWITCH_PIN 9        // #3 on PCB Switches header, should be the "Activate" Switch, put Cyclotron in full power for bursts firing
#define CYCLOTRON_POWER_SWITCH_PIN 8 // #4 on PCB Switches header, should be the Top Switch on the gunbox, power the cylclotron and ready to fire.
#define MAIN_POWER_SWITCH_PIN 7      // #5 on PCB Switches header, should be the Bottom Switch on the gunbox, power the blaster to Power On state
#define INTENSIFY_BUTTON_PIN 6       // #6 on PCB Switches header, puts the blaster in party mode, playings themes one after the other. Use fire button to play next.
// Spares pins on the buttons/switches headers
#define SPARE_1_PIN 5 // #7 on PCB Switches header
#define SPARE_2_PIN 4 // #8 on PCB Switches header
// Smoke module pins
#define SMOKE_RELAY_PIN A1
#define FAN_RELAY_PIN A2
// Potentiometers input
#define VOL_POT_PIN A6      /* For software volume control with audio player*/
#define FIRE_ROD_POT_PIN A7 /* For fire rod hue control */

/*********************************************/
/*          SWITCHES AND BUTTONS             */
/*********************************************/
#include "SBK_WB_SwitchEngine.h"
// INTENSIFY BUTTON OR SWITCH
// If your Intensify push button is replaced by a switch in your Wrist Blaster, set this to "true".
// Intensify is a Push Button, set this to "false".
const bool INTENSIFY_IS_A_SWITCH = true;

/*********************************************/
/*               SMOKE MACHINE               */
/*********************************************/
#include "SBK_WB_SmokeEngine.h"
// DEFINE the maximum time that the smoker can be ON
const uint16_t SMOKER_MAX_ON_TIME = 15000; // in ms
// DEFINE the minimum OFF time of the smoker, ti prevent short cylcing
const uint16_t SMOKER_MIN_OFF_TIME = 10000; // in ms
// Smoker is activated, if minimum off time is respected, when the wrist blaster goes into
// STATE_CAPTURE_OVERHEAT and stop after STATE_OVERHEATED tail
// SMOKE ENABLING :
// Smoke effects are DISABLE when MCU is powered, you need to activate them in POWER OFF STATE with the Fire Button.
// Hold fire button, it will show the actual state on TOP WHITE INDICATOR : Red = off, Green = ON;
// If you hold the button for 3 second, the ENABLING/DISABLING will switch and you'll see the
// indicator switching color for the new enable state.

/*********************************************/
/*           BAR METER & DRIVER(s)           */
/*********************************************/
#include "SBK_WB_BarMeterEngine.h"
/* DEFINE BAR METER TOTAL NUMBER OF SEGEMENTS*/
const uint8_t SEG_NUMBER = 28;
const bool BM_DIRECTION = REVERSE; // animation direction (FORWARD/REVERSE)

/*  SELECT (uncomment) ONE DRIVER TYPE :     */
#define BM_MAX72xx // MAX7219/7221 uses 3 pins serial communication : data, clock, load.
// #define BM_HT16K33 // I2C LEDs driver like Adafruit backpack, uses 2 pins : SDA, SDC.

#ifdef BM_HT16K33
/* DEFINE DRIVER I2C ADDRESS IF REQUIRED     */
#define BM_ADDRESS 0x70 // for I2C drivers type
#endif

/*********************************************/
/*        BAR METER SEGMENTS MAPPING         */
/*********************************************/
// BAR METER SEGMENTS MAPPING FOR THE DRIVER
// DEFINE SEGMENTS MAPPING on bar meter driver {ROW,COL}.
// SELECT (uncomment) ONLY ONE mapping type below :
////**************************************************////
#define SK_MAPPING // Common anode bar meter type SK
// #define SA_MAPPING // Common cathode bar meter type SA
////**************************************************////

#ifdef SK_MAPPING
//  MAPPING 1 matrix definition, more associated with common cathode SK bar meter
//  This mapping works for this MAX72xx driver PCB "SBK_WB_BG_SK_PCB_Vx"
//  with bar meter holder PCB "SBK_WB_BG_28SEG_PCB_Vx"
const uint8_t BM_SEG_MAP[28][2] = {
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
    {6, 3}, // SEG #28
};
#elif defined(SA_MAPPING)
//  MAPPING 2 matrix definition, more associated with common cathode SA bar meter
//  This mapping works for this MAX72xx driver PCB "SBK_WB_BG_SA_PCB_Vx"
//  with bar meter holder PCB "SBK_WB_BG_28SEG_PCB_Vx"
const uint8_t BM_SEG_MAP[28][2] = {
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
    {3, 6}, // SEG #28
};
#endif

/*********************************************/
/*            WS2812 lEDS STRIP              */
/*********************************************/
#include "SBK_WB_LedsStripBaseEngine.h"
const uint8_t TOTAL_LEDS_NUMBER = 19; // vent + indicators + firing jewel + cyclotron total WS21812 pixels
/***********************************************/
/*             LEDS INDEX                 */
/***********************************************/
// The following sections contain indexes for the leds on the chain, starting from 0.
// You will need to update these indexes to match where things are in the chain
// Cyclotron, vent, indicators and firing tip index in the WS2812 wrist blaster chain :
const uint8_t LED_INDEX_SLOWBLOW = 0;   // Indicator
const uint8_t LED_INDEX_TOP_YELLOW = 1; // Indicator
const uint8_t LED_INDEX_TOP_WHITE = 2;  // Indicator
const uint8_t LED_INDEX_VENT = 3;
const uint8_t LED_INDEX_FRONT_ORANGE = 4; // Indicator
const uint8_t LED_INDEX_TIP_1ST = 5;
const uint8_t LED_INDEX_TIP_LAST = 11;
const uint8_t LED_INDEX_CYC_START = 12;
const uint8_t LED_INDEX_CYC_END = 18;
/***********************************************/
/*                  VENT LED                   */
/***********************************************/
#include "SBK_WB_VentEngine.h"
/***********************************************/
/*               CYCLOTRON LEDs                */
/***********************************************/
#include "SBK_WB_CyclotronEngine.h"
const bool CYCLOTRON_DIRECTION = FORWARD; // animation direction (FORWARD/REVERSE)
// Cyclotron ring and center positions pixels for the index in the WS2812 wrist blaster chain
// Cyclotron jewel has 7 pixels
const uint8_t CYC_NUMLEDS = 7;
// Identify specific pixel order on the jewel starting from 0
const uint8_t CYC_RING_1ST = 0;
const uint8_t CYC_RING_LAST = 5;
const uint8_t CYC_CENTER = 6;
/***********************************************/
/*               FIRE ROD LEDs                */
/***********************************************/
#include "SBK_WB_RodEngine.h"
// Fire Rod jewel has 7 pixels
const uint8_t ROD_NUMLEDS = 7;
// HUE POTENTIOMETER :
// If you have no potentiometer hooked to the FIRE_ROD_POT_PIN, disable this feature.
// If enabled and no pot on the pin, fire rod hue will change erraticly.
const bool HUE_POT_READY = ENABLE;
/***********************************************/
/*               INDICATORS LEDs               */
/***********************************************/
#include "SBK_WB_IndicatorEngine.h"
// Indicator flashing speeds
const uint8_t FAST_BLINK_SP = 100;
const uint16_t MEDIUM_BLINK_SP = 500;
const uint16_t SLOW_BLINK_SP = 1000;
/***********************************************/
/*           FIRE BUTTON SINGLE LED            */
/***********************************************/
// This not a an adressable LED, it's a single led drive directly from the pin and a serie resistor
// Led should draw 40mA max on the pin.
// The Yellow led typically used needs a 2.0V forward voltage at 20mA :
// for a 5V board, with a 20mA yellow led, a 150 Ohms resistor should be ok to limit forward voltage.
// You can disable this indicator here :
const bool FIRE_BUTTON_LED_READY = ENABLE;

/*********************************************/
/*                                           */
/*    AUDIO PLAYER definition and helpers    */
/*                                           */
/*********************************************/
#include <SoftwareSerial.h>
#include "SBK_WB_PlayerEngine.h"
const uint8_t VOLUME_MAX = 30;            // 0-30 If you want to reduce the maximum possible volume according to your amp module, set this here
const uint8_t VOLUME_START = 20;          // 0-30 Volume at star-up, will not change if volume potentiometer doesn't exist
const uint8_t PLAYER_COMMAND_DELAY = 150; // short delay between query/ commands : some player(s) will behave weirdly if there is no delay
const uint16_t PLAYER_BAUDRATE = 9600; // Native baudrate is 9600 for this player.
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
const bool BUSY_PIN_READY = DISABLE;   // ENABLING NOT RECOMMANDED : this option is left there for coders who would like to eplore this avenue...
// VOLUME POTENTIOMETER :
// If you have no volume potentiometer hooked to the VOL_POT_PIN, disable this feature.
// If enabled and no pot on the pin, volume will change erraticly.
const bool VOL_POT_READY = ENABLE;

/****************************/
/*     BLASTER STATES LIST  */
/****************************/
/*  Blaster goes trought states (switch/case loop) using flags and triggers. */
/*  Each state is a case, and each state case contain an othe switch/case : initital step case for this state, other possible steps and exit steps. */
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
    STATE_PARTY_MODE = 17,               // Intensify Switch is ON, Blaster in party mode playing music tracks
    STATE_POWER_OFF = 18                 // The blaster is OFF, or has finished shutting down
};

/****************************/
/*     SOUND FX TRACKS      */
/****************************/
/* Audio track files numbers definition for soundboards DFPlayer mini  */
/* Tracks should be mono WAVE type file, named 001.wav, 002.wav, etc. They should be place in the SD card */
/* root folder. They are played in the order they have been put on the flash drive, not by trackname */
/* Also, TRACK NUMBERS FIT PACK STATES, it's part of the core program, this order should be mainntain/corrected that way */
/****************************************/
/* SOUND FX TRACKS LENGTHS AND LOOPING  */
/****************************************/
/* Tracks milliseconds lengths in index order : must be changed according to the yours tracks. */
/*  Those are used to determine the track's playing end in the CORE main loop to minimize delay in switching sound FX tracks, no BUSY pin is used. */
/*  It also prevent using the get track length functions that could cause some delay with some players */
/*  You can get your exact track lengths in Audacity or others audio software*/
/*  DEFINE the tracks lengths in milliseconde here :*/
const uint16_t TRACK_LENGTH[] = {
    0,     // No track, just an offset to be aligned with player (DFPlayer) track number starting from 1...
    1400,  // track #1, no loop, STATE_PARTY_MODE_IN
    2700,  // track #2, no loop, STATE_PARTY_MODE_OUT
    5000,  // track #3, no loop, STATE_POWER_OFF_TO_ON
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
    0,     // track #17, STATE_PARTY_MODE : no track, play files from folfer 01 on SD card
    0      // track #18, STATE_POWER_OFF : player OFF
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
    false, // track #17, no loop, STATE_PARTY_MODE
    false  // track #18, no loop, STATE_POWER_OFF
};

/*******************************/
/* SOME STATE/STAGE PARAMETERS */
/*******************************/
// SECTIONS of track length that need to be defined :
const uint16_t DURATION_CAPTURE_OVERHEAT = 7500;     // CAPTURE_WARNING_OVERHEAT track overheat section duration
const uint16_t DURATION_BURST_TAIL = 2000;           // BURST track tail section duration
const uint16_t DURATION_BURST_OVERHEAT = 7500;       // BURST_OVERHEAT track overheat section duration
const uint16_t DURATION_CAPTURE_TAKEOFF_RAMP = 1000; // To give time for the capture shot to remp in before going in warning stage
// CAPTURE shot max length before going into overheat
const uint16_t DURATION_CAPTURE_MAX = 20000; // between 10000ms to 30000ms (capture track length) : how long before going into over heat in capture shot
// MAX BURST shot before going into overheat
const uint8_t MAX_BURST_SHOTS = 5; // How many shot before overheat

#endif