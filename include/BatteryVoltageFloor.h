// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

// Custom (sonniboi fork): voltage floor in addition to the SoC thresholds.
//
// Upstream uses EITHER the SoC thresholds (while the BMS SoC is valid and
// younger than 60 s) OR the voltage thresholds (fallback). A LiFePO4 BMS
// estimates the SoC poorly near empty: on 2026-11-16 the reported SoC jumped
// from 15 % to 4.5 % within minutes while the inverters kept discharging at
// 2.1 kW, and the weakest cell dropped to 2.79 V before the 5 % SoC stop hit.
// The load-corrected voltage had crossed the 49.0 V stop threshold 8 minutes
// earlier.
//
// With this floor the DPL stops when the SoC OR the load-corrected voltage
// reaches its stop threshold. To keep a hysteresis for the voltage floor, a
// restart additionally requires the voltage start threshold.
//
// Voltage thresholds at or above kSentinelVolts disable the floor. They are
// used as "voltage path locked" marker by dpl-ev-guard.sh (60.0 / 59.9 V while
// the wallbox charges); without this exception the DPL would stay in STOP for
// the whole charging session.
namespace BatteryVoltageFloor {

// 16 x 3.65 V, the highest voltage a 16s LiFePO4 pack can reach
constexpr float kSentinelVolts = 58.4f;

inline bool isActive(float startThreshold, float stopThreshold)
{
    return stopThreshold > 0.0f && stopThreshold < kSentinelVolts
        && startThreshold > 0.0f && startThreshold < kSentinelVolts;
}

// true: stop discharging although the SoC is still above its stop threshold
inline bool stopReached(float loadCorrectedVoltage, float startThreshold, float stopThreshold)
{
    if (!isActive(startThreshold, stopThreshold)) { return false; }
    if (loadCorrectedVoltage <= 0.0f) { return false; } // voltage unknown: SoC decides alone
    return loadCorrectedVoltage <= stopThreshold;
}

// false: keep the battery stopped although the SoC reached its start threshold
inline bool startAllowed(float loadCorrectedVoltage, float startThreshold, float stopThreshold)
{
    if (!isActive(startThreshold, stopThreshold)) { return true; }
    if (loadCorrectedVoltage <= 0.0f) { return true; } // voltage unknown: SoC decides alone
    return loadCorrectedVoltage >= startThreshold;
}

} // namespace BatteryVoltageFloor
