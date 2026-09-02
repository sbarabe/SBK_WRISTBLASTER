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

// Battery Ranges
enum BatteryType
{
    NONE,
    LIPO_2S,
    LIPO_3S,
    NIMH_5S,
    NIMH_6S,
    NIMH_7S,
    NIMH_8S,
    NIMH_9S
};



// Some constants values for animations
// Voltage Divider Constants
#define BATT_R1 100000UL      // 100kΩ
#define BATT_R2 47000UL       // 47kΩ
#define BATT_REF_VOLTAGE 5000 // mVolts, measure with multimeter between 5V pin and gnd
#define BATT_ADC_MAX 1023UL

class BattMoniroting
{
public:
    BattMoniroting(const uint8_t batt_pin = 255,
                   const bool power_monitoring = DISABLE,
                   BatteryType batt_type = NONE,
                   const bool low_cutoff = DISABLE,
                   const uint16_t scaling_factor_per_mille = 1000);

    void begin();
    bool isBattTooLow();
    void updateReading();
    uint16_t readBattVoltage();
    uint8_t readBattPercentage();

protected:
    const uint8_t _BATT_PIN;
    const bool _POWER_MONITORING;
    BatteryType _selectedBattery;
    const bool _LOW_CUT_OFF;
    const uint16_t _SCALING_FACTOR_PER_MILLE;
    uint16_t _batteryVoltage;
    uint8_t _batteryPercent;
    bool _lowBatt;
    static const uint8_t _NUM_SAMPLES = 8;
    uint16_t _samples[_NUM_SAMPLES];
    uint32_t _sum;
    uint8_t _index;

    uint16_t _readScaledVoltage();
    uint8_t _getBattSoC(uint16_t battmV);
    uint8_t _NiMhSoCperCell(uint16_t mV);
    uint8_t _LiPoSoCperCell(uint16_t mV);
};
