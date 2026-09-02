/**
 * Cyclotron state scheme and target selection.
 *
 * Part of SBK_WRISTBLASTER_CORE.
 * Copyright (c) 2025-2026 Samuel Barabe. MIT License.
 */

#pragma once

#include <Arduino.h>
#include "SBK_WB_CyclotronEngine_V2_0_0.h"

enum CyclotronTargetId : uint8_t
{
    CYC_TARGET_CLEAR,
    CYC_TARGET_OFF,
    CYC_TARGET_ON,
    CYC_TARGET_FULL,
    CYC_TARGET_CAPTURE_MAX,
    CYC_TARGET_PARTY_OUT,
    CYC_TARGET_PHASED
};

enum CyclotronRampTimeId : uint8_t
{
    CYC_RAMP_NONE,
    CYC_RAMP_TRACK,
    CYC_RAMP_FINISH,
    CYC_RAMP_CAPTURE_TAKEOFF
};

struct CyclotronStateConfig
{
    uint8_t target;
    uint8_t rampTime;
};

constexpr uint8_t CYCLOTRON_STATE_CONFIG_COUNT = 21;

// INDEXED BY BlasterState: entry order must exactly match the BlasterState enum.
// The count assertions cannot detect a semantically misordered entry.
// Phase-driven firing/overheat states keep their explicit phase logic in
// getCyclotronScheme().
const CyclotronStateConfig CYCLOTRON_STATE_CONFIG[] PROGMEM = {
    {CYC_TARGET_CLEAR, CYC_RAMP_NONE},                  // STATE_ZERO
    {CYC_TARGET_ON, CYC_RAMP_TRACK},                    // STATE_PARTY_MODE_IN
    {CYC_TARGET_PARTY_OUT, CYC_RAMP_TRACK},             // STATE_PARTY_MODE_OUT
    {CYC_TARGET_CLEAR, CYC_RAMP_NONE},                  // STATE_POWER_OFF_TO_ON
    {CYC_TARGET_CLEAR, CYC_RAMP_NONE},                  // STATE_POWER_ON_TO_OFF
    {CYC_TARGET_CLEAR, CYC_RAMP_NONE},                  // STATE_POWER_ON
    {CYC_TARGET_ON, CYC_RAMP_TRACK},                    // STATE_CYCLOTRON_OFF_TO_ON
    {CYC_TARGET_OFF, CYC_RAMP_TRACK},                   // STATE_CYCLOTRON_ON_TO_OFF
    {CYC_TARGET_ON, CYC_RAMP_FINISH},                   // STATE_CYCLOTRON_ON
    {CYC_TARGET_FULL, CYC_RAMP_TRACK},                  // STATE_CYCLOTRON_ON_TO_FULL
    {CYC_TARGET_ON, CYC_RAMP_TRACK},                    // STATE_CYCLOTRON_FULL_TO_ON
    {CYC_TARGET_FULL, CYC_RAMP_FINISH},                 // STATE_CYCLOTRON_FULL_POWER
    {CYC_TARGET_CAPTURE_MAX, CYC_RAMP_CAPTURE_TAKEOFF}, // STATE_CAPTURE
    {CYC_TARGET_ON, CYC_RAMP_TRACK},                    // STATE_CAPTURE_TAIL
    {CYC_TARGET_PHASED, CYC_RAMP_NONE},                 // STATE_CAPTURE_OVERHEAT
    {CYC_TARGET_PHASED, CYC_RAMP_NONE},                 // STATE_BURST
    {CYC_TARGET_PHASED, CYC_RAMP_NONE},                 // STATE_BURST_OVERHEAT
    {CYC_TARGET_OFF, CYC_RAMP_TRACK},                   // STATE_ALL_ON_TO_OFF
    {CYC_TARGET_ON, CYC_RAMP_FINISH},                   // STATE_PARTY_MODE
    {CYC_TARGET_CLEAR, CYC_RAMP_NONE},                  // STATE_POWER_OFF
    {CYC_TARGET_CLEAR, CYC_RAMP_NONE}                   // STATE_LOW_BATT
};

static_assert(sizeof(CYCLOTRON_STATE_CONFIG) / sizeof(CYCLOTRON_STATE_CONFIG[0]) == CYCLOTRON_STATE_CONFIG_COUNT,
              "CYCLOTRON_STATE_CONFIG has an unexpected number of entries");

inline const CycParams &getCyclotronTarget(uint8_t target, bool mainEnabled,
                                            bool cyclotronEnabled, bool fullPower)
{
    if (target == CYC_TARGET_PARTY_OUT)
    {
        if (!mainEnabled || !cyclotronEnabled)
            return CYC_OFF;
        return fullPower ? CYC_FULL : CYC_ON;
    }

    switch (target)
    {
    case CYC_TARGET_OFF:
        return CYC_OFF;
    case CYC_TARGET_FULL:
        return CYC_FULL;
    case CYC_TARGET_CAPTURE_MAX:
        return CYC_CAPTURE_MAX;
    default:
        return CYC_ON;
    }
}
