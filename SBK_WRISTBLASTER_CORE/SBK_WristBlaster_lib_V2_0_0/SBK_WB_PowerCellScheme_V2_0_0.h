/**
 * PowerCell state scheme and animation-speed targets.
 *
 * Part of SBK_WRISTBLASTER_CORE.
 * Copyright (c) 2025-2026 Samuel Barabe. MIT License.
 */

#pragma once

#include <Arduino.h>

#ifndef DISABLE
#define DISABLE 0
#endif

#ifndef ENABLE
#define ENABLE 1
#endif

constexpr uint16_t PC_POWER_ON_UPDATE_INT = 125;
constexpr uint16_t PC_CYC_ON_UPDATE_INT = 100;
constexpr uint16_t PC_CYC_FULL_UPDATE_INT = 80;
constexpr uint16_t PC_FIRING_MAX_UPDATE_INT = 40;

enum PowerCellTargetId : uint8_t
{
    PC_TARGET_OFF,
    PC_TARGET_LOW_BATT,
    PC_TARGET_POWERED_DOWN,
    PC_TARGET_BOOT,
    PC_TARGET_POWER_ON,
    PC_TARGET_CYC_ON,
    PC_TARGET_CYC_FULL,
    PC_TARGET_FIRING_MAX,
    PC_TARGET_PARTY_OUT,
    PC_TARGET_SHUTDOWN,
    PC_TARGET_PHASED
};

enum PowerCellRampTimeId : uint8_t
{
    PC_RAMP_NONE,
    PC_RAMP_TRACK,
    PC_RAMP_BOOT
};

struct PowerCellStateConfig
{
    uint8_t target;
    uint8_t rampTime;
};

constexpr uint8_t POWERCELL_STATE_CONFIG_COUNT = 21;

// INDEXED BY BlasterState: entry order must exactly match the BlasterState enum.
// The count assertions cannot detect a semantically misordered entry.
// States whose target changes between phases retain explicit logic in
// getPowerCellScheme().
const PowerCellStateConfig POWERCELL_STATE_CONFIG[] PROGMEM = {
    {PC_TARGET_OFF, PC_RAMP_NONE},            // STATE_ZERO
    {PC_TARGET_CYC_ON, PC_RAMP_TRACK},        // STATE_PARTY_MODE_IN
    {PC_TARGET_PARTY_OUT, PC_RAMP_TRACK},     // STATE_PARTY_MODE_OUT
    {PC_TARGET_BOOT, PC_RAMP_BOOT},           // STATE_POWER_OFF_TO_ON
    {PC_TARGET_POWER_ON, PC_RAMP_TRACK},      // STATE_POWER_ON_TO_OFF
    {PC_TARGET_POWER_ON, PC_RAMP_NONE},       // STATE_POWER_ON
    {PC_TARGET_CYC_ON, PC_RAMP_TRACK},        // STATE_CYCLOTRON_OFF_TO_ON
    {PC_TARGET_POWER_ON, PC_RAMP_TRACK},      // STATE_CYCLOTRON_ON_TO_OFF
    {PC_TARGET_CYC_ON, PC_RAMP_NONE},         // STATE_CYCLOTRON_ON
    {PC_TARGET_CYC_FULL, PC_RAMP_TRACK},      // STATE_CYCLOTRON_ON_TO_FULL
    {PC_TARGET_CYC_ON, PC_RAMP_TRACK},        // STATE_CYCLOTRON_FULL_TO_ON
    {PC_TARGET_CYC_FULL, PC_RAMP_NONE},       // STATE_CYCLOTRON_FULL_POWER
    {PC_TARGET_FIRING_MAX, PC_RAMP_TRACK},    // STATE_CAPTURE
    {PC_TARGET_CYC_ON, PC_RAMP_TRACK},        // STATE_CAPTURE_TAIL
    {PC_TARGET_PHASED, PC_RAMP_NONE},         // STATE_CAPTURE_OVERHEAT
    {PC_TARGET_PHASED, PC_RAMP_NONE},         // STATE_BURST
    {PC_TARGET_PHASED, PC_RAMP_NONE},         // STATE_BURST_OVERHEAT
    {PC_TARGET_SHUTDOWN, PC_RAMP_TRACK},      // STATE_ALL_ON_TO_OFF
    {PC_TARGET_CYC_ON, PC_RAMP_NONE},         // STATE_PARTY_MODE
    {PC_TARGET_POWERED_DOWN, PC_RAMP_NONE},   // STATE_POWER_OFF
    {PC_TARGET_LOW_BATT, PC_RAMP_NONE}        // STATE_LOW_BATT
};

static_assert(sizeof(POWERCELL_STATE_CONFIG) / sizeof(POWERCELL_STATE_CONFIG[0]) == POWERCELL_STATE_CONFIG_COUNT,
              "POWERCELL_STATE_CONFIG has an unexpected number of entries");

inline uint16_t getPowerCellTargetSpeed(uint8_t target, bool mainEnabled,
                                        bool cyclotronEnabled, bool fullPower)
{
    if (target == PC_TARGET_PARTY_OUT)
    {
        if (!mainEnabled || !cyclotronEnabled)
            return PC_POWER_ON_UPDATE_INT;
        return fullPower ? PC_CYC_FULL_UPDATE_INT : PC_CYC_ON_UPDATE_INT;
    }

    switch (target)
    {
    case PC_TARGET_POWER_ON:
        return PC_POWER_ON_UPDATE_INT;
    case PC_TARGET_CYC_FULL:
        return PC_CYC_FULL_UPDATE_INT;
    case PC_TARGET_FIRING_MAX:
        return PC_FIRING_MAX_UPDATE_INT;
    default:
        return PC_CYC_ON_UPDATE_INT;
    }
}
