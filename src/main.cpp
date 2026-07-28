#include <Arduino.h>

#include "HealthManager.h"
#include "HealthSnapshot.h"
#include "SerialOutput.h"

// Supplies HealthManager's "now", in the same units (milliseconds) as the
// timeout values passed to registerParameter() below. This is the only
// place in this file that reads a clock; HealthCore and HealthManager
// never do.
uint32_t currentTimeMs() {
    return static_cast<uint32_t>(millis());
}

HealthManager manager(currentTimeMs);
HealthSnapshot snapshot;
SerialOutput serialOutput(Serial);

ParameterHandle tempHandle = INVALID_PARAMETER_HANDLE;
ParameterHandle batteryHandle = INVALID_PARAMETER_HANDLE;

void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("HEALTH FRAMEWORK INTEGRATION TEST");
    Serial.println("HealthCore -> HealthManager -> HealthSnapshot -> SerialOutput");

    // Priority::NORMAL and Priority::CRITICAL are used here rather than
    // Priority::HIGH/Priority::LOW. HIGH/LOW's enumerator names collide
    // with Arduino's own HIGH/LOW pin-state macros in any file that
    // includes both Arduino.h and Priority; that is a library-level issue
    // to be resolved separately, not something this example works around.
    tempHandle = manager.registerParameter("TEMP", "Engine Temperature", "C", "CAN", 5000, Priority::NORMAL);
    batteryHandle = manager.registerParameter("BAT", "Battery Voltage", "V", "ADC", 5000, Priority::CRITICAL);

    // Reported once: BAT falls OLD after its 5000 ms timeout and stays OLD,
    // since nothing reports it again below. This is a deliberate contrast
    // with TEMP, which is re-reported every loop() and so stays FRESH for
    // the whole run — a simple way to see both Freshness outcomes on real
    // hardware in a single snapshot.
    manager.report(batteryHandle, "12.8");
}

void loop() {
    manager.report(tempHandle, "82.4");

    snapshot.capture(manager);

    Serial.println();
    Serial.println("---- Health Snapshot ----");
    serialOutput.print(snapshot);

    delay(2000);
}
