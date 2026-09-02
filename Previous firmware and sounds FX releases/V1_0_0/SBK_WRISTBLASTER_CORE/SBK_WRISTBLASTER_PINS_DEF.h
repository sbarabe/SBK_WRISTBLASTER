/**
 * @file        SBK_WRISTBLASTER_PINS_DEF.h
 * @brief       Pin definition file for the SBK_WRISTBLASTER_CORE system.
 *
 * @author      Samuel Barabe
 * @copyright   Copyright (c) 2025-2026 Samuel Barabe
 * @license     MIT License
 * @version     1.1.0
 * @link        https://github.com/sbarabe/SBK_WRISTBLASTER
 *
 * @details
 * This file defines all pin assignments for the SBK Wrist Blaster system.
 * Pin mappings are based on the PCB/board type specified in the CONFIG file.
 *
 * Avoid editing this file unless you need to define a custom pin configuration.
 * In all cases, configure your setup through `SBK_WRISTBLASTER_CONFIG.h`.
 *
 * To use a custom pin mapping, define `CUSTOM_PINS_DEFINITION` in your config file.
 *
 * For full setup instructions and documentation, and to support this project, please visit:
 * https://github.com/sbarabe/SBK_WRISTBLASTER
 *
 * @see https://opensource.org/licenses/MIT
 */

#pragma once

/*********************************************/
/*                                           */
/*          MCU PINS CONFIGURATION           */
/*                                           */
/*********************************************/
// See MCU PINS CONFIGURATION section in SBK_WRISTBLASTER_CONFIG.H to select the right board.
// If you don't want to use a pre-defined board pins definition, you can define a custom pins definition below.

/********************************************************/
#if defined(CUSTOM_PINS_DEFINITION)
#pragma message("Compiling for Custom Pins definition.")
// Define your CUSTOM pins definition here :
/********************************************************/
// ARDUINO NANO EVERY PINS DEFINITION
#define PLAYER_SOFTSERIAL // Use software serial for DFPlayer command
#define RX_PIN 2          // If Software Serial is used, SoftSerial receiving pin to audio board Tx pin
#define TX_PIN 3          // If Software Serial is used, SoftSerial transmitting pin to audio board Rx pin
#define BUSY_PIN 12       // Audio player BUSY pin - NOT USED unless BUSY_PIN_READY is enabled. Track lengths are used by default instead of the BUSY signal for faster playback timing.
#define AMP_MUTE_PIN 13   // Onboard Amplifier mute pin
// Bar Meter driver pins
#define PBM_DIN_PIN A4  // connected to bar meter driver DataIn pin / SDA in case of I2C driver
#define PBM_CLK_PIN A5  // connected to bar meter driver Clock pin / SCL in case of I2C driver
#define PBM_LOAD_PIN A3 // connected to bar meter driver Load pin - NOT USED for I2C driver
// WS2812 Leds chain and others leds pins
#define LEDS_STRIP1_PIN A0     // for WS2812 LEDs chain
#define FIRE_BUTTON_LED_PIN 11 // #1 on PCB Switches header, with an in line resistor to protect led : illuminated fire button led output (yellow)
// Buttons and switches
#define FIRE_BUTTON_PIN 10           // #2 on PCB Switches header, should be the Fire Button
#define ACTIVATE_SWITCH_PIN 9        // #3 on PCB Switches header, should be the "Activate" Switch, put Cyclotron in full power for bursts firing
#define CYCLOTRON_POWER_SWITCH_PIN 8 // #4 on PCB Switches header, should be the Top Switch on the gunbox, power the cyclotron and ready to fire.
#define MAIN_POWER_SWITCH_PIN 7      // #5 on PCB Switches header, should be the Bottom Switch on the gunbox, power the blaster to Power On state
#define INTENSIFY_BUTTON_PIN 6       // #6 on PCB Switches header, puts the blaster in party mode, playing themes one after the other. Use fire button to play next.
// SPARE IOs pins on IOs connector
#define SPARE_PIN_IO7 A1 
#define SPARE_PIN_IO8 A2 
// Smoke module pins
#define SMOKE_RELAY_PIN 5  
#define FAN_RELAY_PIN 4   
// Potentiometers input
#define VOL_POT_PIN A6      // For software volume control with audio player
#define FIRE_ROD_POT_PIN A7 // For fire rod hue control

/********************************************************/
// End of CUSTOM PINS DEFINITION section
/********************************************************/

#elif defined(SBK_WRIST_BLASTER_PCB_V2)
#pragma message("Compiling for SBK_WRIST_BLASTER_PCB_V2 pcb pins definition.")
// This board has no connector for new PowerCell Leds Strip or PowerCell Bar Meter.
// And also no embedded input for battery monitoring.
// This board works ONLY with Arduino Nano Every since pin A6 and A7 are used as digital IOs.
// ARDUINO NANO EVERY PINS DEFINITION
#define PLAYER_SOFTSERIAL            // Use software serial for DFPlayer command
#define RX_PIN 2                     // If Software Serial is used, SoftSerial receiving pin to audio board Tx pin
#define TX_PIN 3                     // If Software Serial is used, SoftSerial transmitting pin to audio board Rx pin
#define BUSY_PIN 12                  // Audio player BUSY pin - NOT USED unless BUSY_PIN_READY is enabled. Track lengths are used by default instead of the BUSY signal for faster playback timing.
#define AMP_MUTE_PIN 13              // Onboard Amplifier mute pin
// Bar Meter driver pins
#define PBM_DIN_PIN A4               // connected to bar meter driver DataIn pin / SDA in case of I2C driver
#define PBM_CLK_PIN A5               // connected to bar meter driver Clock pin / SCL in case of I2C driver
#define PBM_LOAD_PIN A6              // connected to bar meter driver Load pin - NOT USED for I2C driver
// WS2812 Leds chain and others leds pins
#define LEDS_STRIP1_PIN A7           // for WS2812 LEDs chain
#define FIRE_BUTTON_LED_PIN 11       // #1 on PCB Switches header, with an in line resistor to protect led : illuminated fire button led output (yellow)
// Buttons and switches
#define FIRE_BUTTON_PIN 10           // #2 on PCB Switches header, should be the Fire Button
#define ACTIVATE_SWITCH_PIN 9        // #3 on PCB Switches header, should be the "Activate" Switch, put Cyclotron in full power for bursts firing
#define CYCLOTRON_POWER_SWITCH_PIN 8 // #4 on PCB Switches header, should be the Top Switch on the gunbox, power the cyclotron and ready to fire.
#define MAIN_POWER_SWITCH_PIN 7      // #5 on PCB Switches header, should be the Bottom Switch on the gunbox, power the blaster to Power On state
#define INTENSIFY_BUTTON_PIN 6       // #6 on PCB Switches header, puts the blaster in party mode, playing themes one after the other. Use fire button to play next.
// SPARE IOs pins on IOs connector
#define SPARE_PIN_IO7 5              // Spare pin on IO connector, #7
#define SPARE_PIN_IO8 4              // Spare pin on IO connector, #8
// Smoke module pins
#define SMOKE_RELAY_PIN A2
#define FAN_RELAY_PIN A3
// Potentiometers input
#define VOL_POT_PIN A0      // For software volume control with audio player//
#define FIRE_ROD_POT_PIN A1 // For fire rod hue control //

#elif defined(SBK_WRIST_BLASTER_II_PCB_Vx)
#pragma message("Compiling for SBK_WRIST_BLASTER_II_PCB_Vx pcb pins definition.")
// This board has no connector for new PowerCell Leds Strip or PowerCell Bar Meter.
// And also no embedded input for battery monitoring.
// This board works with both Nano and Nano Every.
// ARDUINO NANO AND NANO EVERY PINS DEFINITION
#define PLAYER_SOFTSERIAL            // Use software serial for DFPlayer command
#define RX_PIN 2                     // If Software Serial is used, SoftSerial receiving pin to audio board Tx pin
#define TX_PIN 3                     // If Software Serial is used, SoftSerial transmitting pin to audio board Rx pin
#define BUSY_PIN 12                  // Audio player BUSY pin - NOT USED unless BUSY_PIN_READY is enabled. Track lengths are used by default instead of the BUSY signal for faster playback timing.
#define AMP_MUTE_PIN 13              // Onboard Amplifier mute pin
// Bar Meter driver pins
#define PBM_DIN_PIN A4               // connected to bar meter driver DataIn pin / SDA in case of I2C driver
#define PBM_CLK_PIN A5               // connected to bar meter driver Clock pin / SCL in case of I2C driver
#define PBM_LOAD_PIN A3              // connected to bar meter driver Load pin - NOT USED for I2C driver
// WS2812 Leds chain and others leds pins
#define LEDS_STRIP1_PIN A0           // for WS2812 LEDs chain
#define FIRE_BUTTON_LED_PIN 11       // #1 on PCB Switches header, with an in line resistor to protect led : illuminated fire button led output (yellow)
// Buttons and switches
#define FIRE_BUTTON_PIN 10           // #2 on PCB Switches header, should be the Fire Button
#define ACTIVATE_SWITCH_PIN 9        // #3 on PCB Switches header, should be the "Activate" Switch, put Cyclotron in full power for bursts firing
#define CYCLOTRON_POWER_SWITCH_PIN 8 // #4 on PCB Switches header, should be the Top Switch on the gunbox, power the cyclotron and ready to fire.
#define MAIN_POWER_SWITCH_PIN 7      // #5 on PCB Switches header, should be the Bottom Switch on the gunbox, power the blaster to Power On state
#define INTENSIFY_BUTTON_PIN 6       // #6 on PCB Switches header, puts the blaster in party mode, playing themes one after the other. Use fire button to play next.
// SPARE IOs pins on IOs connector
#define SPARE_PIN_IO7 5              // Spare pin on IO connector, #7
#define SPARE_PIN_IO8 4              // Spare pin on IO connector, #8
// Smoke module pins
#define SMOKE_RELAY_PIN A1
#define FAN_RELAY_PIN A2
// Potentiometers input
#define VOL_POT_PIN A6      // For software volume control with audio player
#define FIRE_ROD_POT_PIN A7 // For fire rod hue control

#elif defined(SBK_PROPCORE_ONE_Vx)
#pragma message("Compiling for SBK PropCore ONE Vx pcb pins definition.")
// This board has a connector for new PowerCell Leds Strip and also an embedded input for battery monitoring.
// This board works with both Nano and Nano Every.
// ARDUINO NANO AND NANO EVERY PINS DEFINITION
#define PLAYER_SOFTSERIAL             // Use software serial for DFPlayer command
#define RX_PIN 7                      // If Software Serial is used, SoftSerial receiving pin to audio board Tx pin
#define TX_PIN 6                      // If Software Serial is used, SoftSerial transmitting pin to audio board Rx pin
#define BUSY_PIN 12                   // Audio player BUSY pin - NOT USED unless BUSY_PIN_READY is enabled. Track lengths are used by default instead of the BUSY signal for faster playback timing.
#define AMP_MUTE_PIN 13               // Onboard Amplifier mute pin
// Bar Meter driver pins
#define PBM_DIN_PIN A4                // connected to bar meter driver DataIn pin / SDA in case of I2C driver
#define PBM_CLK_PIN A5                // connected to bar meter driver Clock pin / SCL in case of I2C driver
#define PBM_LOAD_PIN A3               // connected to bar meter driver Load pin - NOT USED for I2C driver
// WS2812 Leds chain and others leds pins
#define LEDS_STRIP1_PIN 4             // for WS2812 LEDs chain
#define FIRE_BUTTON_LED_PIN 11        // #1 on PCB Switches header, with an in line resistor to protect led : illuminated fire button led output (yellow)
// Buttons and switches
#define FIRE_BUTTON_PIN 10            // #2 on PCB Switches header, should be the Fire Button
#define INTENSIFY_BUTTON_PIN 9       // #3 on PCB Switches header, puts the blaster in party mode, playing themes one after the other. Use fire button to play next.
#define ACTIVATE_SWITCH_PIN 8         // #4 on PCB Switches header, should be the "Activate" Switch, put Cyclotron in full power for bursts firing
#define CYCLOTRON_POWER_SWITCH_PIN A0 // #5 on PCB Switches header, should be the Top Switch on the gunbox, power the cyclotron and ready to fire.
#define MAIN_POWER_SWITCH_PIN A1       // #6 on PCB Switches header, should be the Bottom Switch on the gunbox, power the blaster to Power On state
// Power Cell Optional pins
#define LEDS_STRIP2_PIN 5             // WS2812 data pin for Power Cell strip
// Power monitoring option pins
#define BATT_PIN A2                   // Analog input through voltage divider for battery measurement
// Smoke module pins
#define SMOKE_RELAY_PIN 3
#define FAN_RELAY_PIN 2
// Potentiometers input
#define VOL_POT_PIN A6      // For software volume control with audio player
#define FIRE_ROD_POT_PIN A7 // For fire rod hue control

#elif defined(SBK_PROPCORE_ONE_PLUS_Vx)
#pragma message("Compiling for SBK PropCore ONE+ Vx pcb pins definition.")
// This board has a connector for new PowerCell Bar Meter (driver) and also an embedded input for battery monitoring.
// This board works ONLY with Arduino Nano Every.
// // This board uses Arduino Nano Every Serial1 RX/TX pins for DFPlayer communication.
// ARDUINO NANO EVERY ONLY PINS DEFINITION
#define PLAYER_SERIAL1
#define RX_PIN 0                     // Use Arduino Nano Every Serial1 RX pin
#define TX_PIN 1                     // Use Arduino Nano Every Serial1 TX pin
#define BUSY_PIN 12                  // Audio player BUSY pin - NOT USED unless BUSY_PIN_READY is enabled. Track lengths are used by default instead of the BUSY signal for faster playback timing.
#define AMP_MUTE_PIN 13              // Onboard Amplifier mute pin
// Bar Meter driver pins
#define PBM_DIN_PIN A4               // connected to bar meter driver DataIn pin / SDA in case of I2C driver
#define PBM_CLK_PIN A5               // connected to bar meter driver Clock pin / SCL in case of I2C driver
#define PBM_LOAD_PIN A3              // connected to bar meter driver Load pin - NOT USED for I2C driver
// WS2812 Leds chain and others leds pins
#define LEDS_STRIP1_PIN 4            // for WS2812 LEDs chain
#define FIRE_BUTTON_LED_PIN 11       // #1 on PCB Switches header, with an in line resistor to protect led : illuminated fire button led output (yellow)
// Buttons and switches
#define FIRE_BUTTON_PIN 10           // #2 on PCB Switches header, should be the Fire Button
#define ACTIVATE_SWITCH_PIN 9        // #3 on PCB Switches header, should be the "Activate" Switch, put Cyclotron in full power for bursts firing
#define CYCLOTRON_POWER_SWITCH_PIN 8 // #4 on PCB Switches header, should be the Top Switch on the gunbox, power the cyclotron and ready to fire.
#define MAIN_POWER_SWITCH_PIN A0     // #5 on PCB Switches header, should be the Bottom Switch on the gunbox, power the blaster to Power On state
#define INTENSIFY_BUTTON_PIN A1      // #6 on PCB Switches header, puts the blaster in party mode, playing themes one after the other. Use fire button to play next.
// Power Cell Optional pins
#define PCBM_DIN_PIN 5               // connected to bar meter driver DataIn pin / SDA in case of I2C driver
#define PCBM_CLK_PIN 6               // connected to bar meter driver Clock pin / SCL in case of I2C driver
#define PCBM_LOAD_PIN 7              // connected to bar meter driver Load pin - NOT USED for I2C driver
// Power monitoring option pins
#define BATT_PIN A2                  // Analog input through voltage divider for battery measurement
// Smoke module pins
#define SMOKE_RELAY_PIN 3
#define FAN_RELAY_PIN 2
// Potentiometers input
#define VOL_POT_PIN A6      // For software volume control with audio player
#define FIRE_ROD_POT_PIN A7 // For fire rod hue control

#endif

/***********************************************************/
/* AUDIO PLAYER SERIAL INTERFACE SAFETY CHECK              */
/***********************************************************/
#if (defined(PLAYER_SOFTSERIAL) + defined(PLAYER_SERIAL1)) != 1
#error "Audio player serial interface: exactly one interface must be defined. Check the selected board definition in SBK_WRISTBLASTER_PINS_DEF.h."
#endif