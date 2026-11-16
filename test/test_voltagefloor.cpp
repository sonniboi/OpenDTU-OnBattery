#include <iostream>
#include <cassert>

#include "../include/BatteryVoltageFloor.h"

using namespace BatteryVoltageFloor;

// normal thresholds on the custom build (dpl-ev-guard.sh N_U_START / N_U_STOP)
const float kStart = 50.0f;
const float kStop = 49.0f;

void testStopOnVoltage() {
    std::cout << "Testing: stop on load-corrected voltage" << std::endl;
    // 2026-11-16 12:18:18: 48.74 V @ 2121 W, factor 0.00012 -> 48.99 V, SoC still 7.4 %
    assert(stopReached(48.99f, kStart, kStop));
    assert(stopReached(49.0f, kStart, kStop));      // threshold itself stops (<=)
    // 2026-11-16 11:45: about 49.6 V corrected -> keep discharging
    assert(!stopReached(49.6f, kStart, kStop));
    assert(!stopReached(52.5f, kStart, kStop));
    std::cout << "✓ PASSED" << std::endl;
}

void testRestartNeedsVoltage() {
    std::cout << "Testing: restart needs the voltage start threshold" << std::endl;
    // SoC >= 10 % but voltage only partly recovered -> stay stopped
    assert(!startAllowed(49.6f, kStart, kStop));
    // 2026-11-16 13:15: 50.1 V while charging -> voltage part allows the start
    assert(startAllowed(50.1f, kStart, kStop));
    assert(startAllowed(50.0f, kStart, kStop));     // threshold itself allows (>=)
    std::cout << "✓ PASSED" << std::endl;
}

void testUnknownVoltage() {
    std::cout << "Testing: unknown voltage leaves the decision to the SoC" << std::endl;
    assert(!stopReached(0.0f, kStart, kStop));
    assert(!stopReached(-1.0f, kStart, kStop));
    assert(startAllowed(0.0f, kStart, kStop));
    std::cout << "✓ PASSED" << std::endl;
}

void testEvGuardSentinel() {
    std::cout << "Testing: dpl-ev-guard lock values disable the floor" << std::endl;
    // wallbox charging: 60.0 / 59.9 V -> floor off, battery may buffer down to SoC 90 %
    assert(!isActive(60.0f, 59.9f));
    assert(!stopReached(52.0f, 60.0f, 59.9f));
    assert(startAllowed(52.0f, 60.0f, 59.9f));
    // just below 16 x 3.65 V the floor still works
    assert(isActive(58.3f, 58.0f));
    assert(!isActive(58.4f, 58.0f));
    std::cout << "✓ PASSED" << std::endl;
}

void testDisabledThresholds() {
    std::cout << "Testing: zero thresholds disable the floor" << std::endl;
    assert(!isActive(kStart, 0.0f));
    assert(!isActive(0.0f, kStop));
    assert(!stopReached(40.0f, kStart, 0.0f));
    assert(startAllowed(40.0f, 0.0f, kStop));
    std::cout << "✓ PASSED" << std::endl;
}

int main() {
    testStopOnVoltage();
    testRestartNeedsVoltage();
    testUnknownVoltage();
    testEvGuardSentinel();
    testDisabledThresholds();
    std::cout << "All voltage floor tests passed" << std::endl;
    return 0;
}
