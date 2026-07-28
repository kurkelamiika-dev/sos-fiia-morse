#ifndef HEALTHMANAGER_H
#define HEALTHMANAGER_H

#include <cstddef>
#include <cstdint>

#include "HealthCore.h"

// HealthManager is the layer that owns time and calculates Freshness, per
// HealthCore_Specification.md Sections 5, 12, and 17: HealthCore itself
// never reads a clock, timer, RTC, or platform tick count, and every
// timestamp it stores is supplied explicitly by its caller. HealthManager is
// that caller.
//
// HealthCore is HealthManager's only storage backend — HealthManager keeps
// no parameter data of its own, including no shadow list of handles.
// ParameterHandle, HealthRecord, Priority, and the compile-time buffer
// constants all come directly from HealthCore.h; HealthManager reuses them
// rather than redefining or wrapping them. Enumeration is likewise backed by
// HealthCore::getAt() — HealthManager never constructs a ParameterHandle
// itself, it only ever forwards handles obtained from HealthCore.

// A HealthRecord together with the Freshness computed for it by
// HealthManager at the moment of the call. Freshness is never stored (see
// HealthCore_Specification.md, Section 5); it is derived here from the
// record's timeout, lastUpdate, and hasBeenUpdated fields against
// HealthManager's current time.
struct HealthStatus {
    HealthRecord record;
    Freshness freshness;
};

// Supplies the current time, in the same units as the timeout values passed
// to registerParameter() (e.g. Arduino's millis()). Injected at construction
// so HealthManager contains no direct dependency on any platform-specific
// time API and can be reused against a fake clock.
using TimeSource = uint32_t (*)();

// Data flow:
//
//   Application -> HealthManager::registerParameter()
//               -> HealthCore::registerParameter()
//
//   Application -> HealthManager::report(handle, value)
//               -> timeSource() for "now"
//               -> HealthCore::report(handle, value, now)
//
//   Application -> HealthManager::get(handle)
//               -> HealthCore::get(handle, record)
//               -> Freshness computed from timeSource(), record.timeout,
//                  record.lastUpdate, record.hasBeenUpdated
//               -> HealthStatus{record, freshness} returned to caller
//
//   Output module (e.g. future HealthSnapshot) -> HealthManager::getAt(position)
//               -> HealthCore::getAt(position, handle, record)
//               -> Freshness computed the same way as get()
//               -> handle and HealthStatus{record, freshness} returned to
//                  caller, so all registered parameters can be walked via
//                  position in [0, count()) without the caller needing to
//                  already hold any handle
//
//   Application -> HealthManager::unregisterParameter()/count()/clear()
//               -> forwarded directly to HealthCore
//
// HealthManager contains no Morse, Serial, OLED, WiFi, or MQTT logic; those
// belong to the layers below it in the architecture (HealthSnapshot,
// MorseOutput, SerialOutput).
class HealthManager {
public:
    explicit HealthManager(TimeSource timeSource);

    // Registers a new parameter. Forwards directly to
    // HealthCore::registerParameter(); see HealthCore.h for failure
    // conditions.
    ParameterHandle registerParameter(const char* id,
                                       const char* description,
                                       const char* unit,
                                       const char* source,
                                       uint32_t timeout,
                                       Priority priority);

    // Removes a parameter. Forwards directly to
    // HealthCore::unregisterParameter().
    bool unregisterParameter(ParameterHandle handle);

    // Updates a parameter's value using HealthManager's current time as the
    // timestamp. Returns false, leaving the record unchanged, under the same
    // conditions as HealthCore::report().
    bool report(ParameterHandle handle, const char* value);

    // Retrieves the record for handle together with its freshness computed
    // against HealthManager's current time. Returns false if the handle is
    // invalid; outStatus is left unchanged in that case.
    bool get(ParameterHandle handle, HealthStatus& outStatus) const;

    // Enumerates registered parameters by position, mirroring
    // HealthCore::getAt(), with freshness added the same way get() adds it.
    // position ranges over [0, count()). Returns false, leaving
    // outHandle/outStatus unchanged, if position >= count().
    bool getAt(size_t position, ParameterHandle& outHandle, HealthStatus& outStatus) const;

    // Number of currently registered parameters. Forwards directly to
    // HealthCore::count().
    size_t count() const;

    // Removes all registered parameters. Forwards directly to
    // HealthCore::clear().
    void clear();

private:
    HealthCore core_;
    TimeSource timeSource_;
};

#endif
