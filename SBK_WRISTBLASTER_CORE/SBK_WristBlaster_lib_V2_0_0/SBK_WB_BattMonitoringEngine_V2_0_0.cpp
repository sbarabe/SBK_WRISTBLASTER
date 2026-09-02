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

#include "SBK_WB_BattMonitoringEngine_V2_0_0.h"

/* DEBUG MESSAGES TO SERIAL */
// comment/uncomment #define DEBUG_TO_SERIAL to receive serial message

// #define DEBUG_TO_SERIAL
#ifdef DEBUG_TO_SERIAL
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif

// Battery Voltage Ranges in mVolts 
// ordered by BatteryType enum: NONE, LIPO_2S, LIPO_3S, NIMH_5S, NIMH_6S, NIMH_7S, NIMH_8S, NIMH_9S
const uint16_t BATTERY_MIN_VOLTAGE[] = {0, 6600, 9900, 5500, 6600, 7700, 8800, 9900};
const uint16_t BATTERY_MAX_VOLTAGE[] = {0, 8400, 12600, 7500, 9000, 10500, 12000, 13500};

// Clear the cutoff only after the pack has recovered 10% above its minimum
// voltage. This prevents repeated LOW_BATT transitions near the threshold.
constexpr uint8_t BATT_CUTOFF_HYSTERESIS_PERCENT = 10;

BattMoniroting::BattMoniroting(const uint8_t batt_pin,
                               const bool power_monitoring,
                               BatteryType batt_type,
                               const bool low_cutoff,
                               const uint16_t scaling_factor_per_mille)
    : _BATT_PIN(batt_pin),
      _POWER_MONITORING(power_monitoring),
      _selectedBattery(batt_type),
      _LOW_CUT_OFF(low_cutoff),
      _SCALING_FACTOR_PER_MILLE(scaling_factor_per_mille),
      _batteryPercent(100),
      _lowBatt(false),
      _sum(0),
      _index(0)

{
    for (uint8_t i = 0; i < _NUM_SAMPLES; ++i)
    {
        _samples[i] = 0;
    }
}

void BattMoniroting::begin()
{
    if (!_POWER_MONITORING || _selectedBattery == NONE)
    {
        _batteryVoltage = BATTERY_MAX_VOLTAGE[_selectedBattery];
        _batteryPercent = 100;
        return;
    }

    const uint16_t battery_mV = _readScaledVoltage();

    for (uint8_t i = 0; i < _NUM_SAMPLES; i++)
        _samples[i] = battery_mV;

    _sum = (uint32_t)battery_mV * _NUM_SAMPLES;
    _index = 0;

    _batteryVoltage = battery_mV;

    _batteryPercent = _getBattSoC(_batteryVoltage);
}

bool BattMoniroting::isBattTooLow()
{
    return _LOW_CUT_OFF && _lowBatt;
}

void BattMoniroting::updateReading()
{

    if (!_POWER_MONITORING || _selectedBattery == NONE)
    {
        return;
    }

    const uint16_t battery_mV = _readScaledVoltage();

    // Update rolling buffer
    _sum -= _samples[_index];
    _samples[_index] = battery_mV;
    _sum += battery_mV;
    _index = (_index + 1) % _NUM_SAMPLES;

    // Average in millivolts
    _batteryVoltage = _sum / _NUM_SAMPLES;

    _batteryPercent = _getBattSoC(_batteryVoltage);

    // SOC is used for the battery display; actual cutoff protection is based
    // on the measured pack voltage.
    if (!_LOW_CUT_OFF)
    {
        _lowBatt = false;
        return;
    }

    const uint16_t cutoffVoltage = BATTERY_MIN_VOLTAGE[_selectedBattery];
    const uint16_t releaseVoltage = cutoffVoltage +
                                    (uint32_t)cutoffVoltage * BATT_CUTOFF_HYSTERESIS_PERCENT / 100;

    if (!_lowBatt && _batteryVoltage <= cutoffVoltage)
        _lowBatt = true;
    else if (_lowBatt && _batteryVoltage >= releaseVoltage)
        _lowBatt = false;
}

uint16_t BattMoniroting::readBattVoltage()
{
    return _batteryVoltage;
}

uint16_t BattMoniroting::_readScaledVoltage()
{
    const uint16_t rawADC = analogRead(_BATT_PIN);
    const uint32_t measured_mV =
        ((uint32_t)rawADC * BATT_REF_VOLTAGE + BATT_ADC_MAX / 2U) / BATT_ADC_MAX;
    const uint32_t divider_mV =
        (measured_mV * (BATT_R1 + BATT_R2) + BATT_R2 / 2U) / BATT_R2;

    return (divider_mV * _SCALING_FACTOR_PER_MILLE + 500UL) / 1000UL;
}

uint8_t BattMoniroting::readBattPercentage()
{
    return _batteryPercent;
}

uint8_t BattMoniroting::_getBattSoC(uint16_t battmV)
{
    uint8_t percent = _batteryPercent;
    switch (_selectedBattery)
    {
    case NONE:
        break;
    case LIPO_2S:
        percent = _LiPoSoCperCell(battmV / 2);
        break;
    case LIPO_3S:
        percent = _LiPoSoCperCell(battmV / 3);
        break;
    case NIMH_5S:
        percent = _NiMhSoCperCell(battmV / 5);
        break;
    case NIMH_6S:
        percent = _NiMhSoCperCell(battmV / 6);
        break;
    case NIMH_7S:
        percent = _NiMhSoCperCell(battmV / 7);
        break;
    case NIMH_8S:
        percent = _NiMhSoCperCell(battmV / 8);
        break;
    case NIMH_9S:
        percent = _NiMhSoCperCell(battmV / 9);
        break;
    }

    return percent;
}

struct BatteryPoint
{
    uint16_t voltage_mV;
    uint8_t percent;
};

const BatteryPoint NiMH_Lookup[] PROGMEM = {
    {1450, 100}, {1425, 95}, {1400, 90}, {1375, 85}, {1350, 80}, {1325, 75}, {1300, 70}, {1275, 60}, {1250, 50}, {1225, 40}, {1200, 30}, {1175, 20}, {1150, 10}, {1100, 0}};

static uint16_t readBatteryVoltage(const BatteryPoint *table, uint8_t index)
{
    return pgm_read_word(&table[index].voltage_mV);
}

static uint8_t readBatteryPercent(const BatteryPoint *table, uint8_t index)
{
    return pgm_read_byte(&table[index].percent);
}

uint8_t BattMoniroting::_NiMhSoCperCell(uint16_t mVperCell)
{
    constexpr uint8_t N = sizeof(NiMH_Lookup) / sizeof(NiMH_Lookup[0]);

    if (mVperCell >= readBatteryVoltage(NiMH_Lookup, 0))
        return readBatteryPercent(NiMH_Lookup, 0);
    if (mVperCell <= readBatteryVoltage(NiMH_Lookup, N - 1))
        return readBatteryPercent(NiMH_Lookup, N - 1);

    for (uint8_t i = 0; i < N - 1; ++i)
    {
        if (mVperCell >= readBatteryVoltage(NiMH_Lookup, i + 1))
        {
            // Linear interpolation
            uint16_t v1 = readBatteryVoltage(NiMH_Lookup, i);
            uint16_t v2 = readBatteryVoltage(NiMH_Lookup, i + 1);
            uint8_t p1 = readBatteryPercent(NiMH_Lookup, i);
            uint8_t p2 = readBatteryPercent(NiMH_Lookup, i + 1);

            return p1 - (p1 - p2) * (v1 - mVperCell) / (v1 - v2);
        }
    }
    return 0; // Fallback
}

const BatteryPoint LiPo_Lookup[] PROGMEM = {
    {4200, 100}, {4100, 95}, {4000, 90}, {3920, 85}, {3860, 80}, {3800, 75}, {3750, 70}, {3700, 60}, {3650, 50}, {3600, 40}, {3550, 30}, {3500, 20}, {3400, 10}, {3300, 0}};

uint8_t BattMoniroting::_LiPoSoCperCell(uint16_t mVperCell)
{
    constexpr uint8_t N = sizeof(LiPo_Lookup) / sizeof(LiPo_Lookup[0]);

    if (mVperCell >= readBatteryVoltage(LiPo_Lookup, 0))
        return readBatteryPercent(LiPo_Lookup, 0);
    if (mVperCell <= readBatteryVoltage(LiPo_Lookup, N - 1))
        return readBatteryPercent(LiPo_Lookup, N - 1);

    for (uint8_t i = 0; i < N - 1; ++i)
    {
        if (mVperCell >= readBatteryVoltage(LiPo_Lookup, i + 1))
        {
            // Linear interpolation
            uint16_t v1 = readBatteryVoltage(LiPo_Lookup, i);
            uint16_t v2 = readBatteryVoltage(LiPo_Lookup, i + 1);
            uint8_t p1 = readBatteryPercent(LiPo_Lookup, i);
            uint8_t p2 = readBatteryPercent(LiPo_Lookup, i + 1);

            return p1 - (p1 - p2) * (v1 - mVperCell) / (v1 - v2);
        }
    }
    return 0;
}
