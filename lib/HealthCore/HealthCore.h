#ifndef HEALTHCORE_H
#define HEALTHCORE_H

#include <cstddef>
#include <cstdint>

// Compile-time capacity and buffer size constants.
// See docs/HealthCore_Specification.md, Section 15.
constexpr size_t MAX_PARAMETERS = 64;
constexpr size_t MAX_ID_LENGTH = 8;
constexpr size_t MAX_DESCRIPTION_LENGTH = 32;
constexpr size_t MAX_VALUE_LENGTH = 24;
constexpr size_t MAX_UNIT_LENGTH = 8;
constexpr size_t MAX_SOURCE_LENGTH = 16;

// Reporting priority. Affects presentation order only; HealthCore assigns no
// other meaning to it.
//
// HIGH/LOW are shielded from Arduino's <Arduino.h> (esp32-hal-gpio.h), which
// #defines HIGH/LOW as digital pin-state macros. Without this, the token
// substitution would corrupt this enum's declaration in any translation unit
// that also includes Arduino.h. push_macro/pop_macro suppresses the macros
// only for this declaration and restores them immediately after, so Arduino
// code elsewhere in the same file (e.g. digitalWrite(pin, HIGH)) keeps
// working.
#pragma push_macro("HIGH")
#pragma push_macro("LOW")
#undef HIGH
#undef LOW
enum class Priority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};
#pragma pop_macro("LOW")
#pragma pop_macro("HIGH")

// Freshness is never stored by HealthCore. This type is defined here because
// it is part of the shared data model, but computing it (comparing a
// caller-supplied "now" against lastUpdate/timeout) is HealthManager's
// responsibility.
enum class Freshness {
    UNKNOWN,
    FRESH,
    OLD
};

// Opaque handle identifying a registered parameter.
//
// Its current implementation contains an index into HealthCore's fixed-size
// internal parameter table. Application code must not depend on this
// representation; treat it as opaque and compare only against
// INVALID_PARAMETER_HANDLE or other handles returned by HealthCore.
struct ParameterHandle {
    int index;
};

// Reserved handle value meaning "no parameter". Returned by
// registerParameter() when registration fails.
constexpr ParameterHandle INVALID_PARAMETER_HANDLE{-1};

inline bool operator==(const ParameterHandle& lhs, const ParameterHandle& rhs) {
    return lhs.index == rhs.index;
}

inline bool operator!=(const ParameterHandle& lhs, const ParameterHandle& rhs) {
    return !(lhs == rhs);
}

// One monitored parameter.
//
// All text fields are fixed-size character buffers; there is no dynamic
// allocation and no use of String. Value is always stored as text,
// regardless of whether the underlying reading is numeric or symbolic.
//
// hasBeenUpdated is false until the parameter's first successful report()
// and represents the UNKNOWN freshness case (see
// docs/HealthCore_Specification.md, Sections 4 and 5).
struct HealthRecord {
    char id[MAX_ID_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    char value[MAX_VALUE_LENGTH];
    char unit[MAX_UNIT_LENGTH];
    char source[MAX_SOURCE_LENGTH];
    uint32_t lastUpdate;
    uint32_t timeout;
    Priority priority;
    bool hasBeenUpdated;
};

// HealthCore defines the shared data model for the Health Framework: a
// fixed-capacity table of HealthRecord entries and the operations to
// register, update, read, and remove them.
//
// HealthCore contains no application logic. It never reads a clock, timer,
// RTC, or platform tick count and has no notion of "now" — every timestamp
// stored in a HealthRecord is supplied explicitly by the caller (see
// docs/HealthCore_Specification.md, Sections 12 and 17).
class HealthCore {
public:
    HealthCore();

    // Registers a new parameter. Returns INVALID_PARAMETER_HANDLE if the
    // table is full (count() == MAX_PARAMETERS) or if any supplied string
    // does not fit its fixed buffer, including the null terminator. On
    // success, the record's value starts empty and hasBeenUpdated is false.
    ParameterHandle registerParameter(const char* id,
                                       const char* description,
                                       const char* unit,
                                       const char* source,
                                       uint32_t timeout,
                                       Priority priority);

    // Removes a parameter. Returns false if the handle is invalid or has
    // already been unregistered.
    bool unregisterParameter(ParameterHandle handle);

    // Updates a parameter's value and last-update timestamp. timestamp is
    // supplied by the caller; HealthCore never reads a clock. Returns false,
    // leaving the record unchanged, if the handle is invalid or the value
    // does not fit MAX_VALUE_LENGTH, including the null terminator.
    bool report(ParameterHandle handle, const char* value, uint32_t timestamp);

    // Retrieves the record for handle into outRecord. Returns false if the
    // handle is invalid; outRecord is left unchanged in that case.
    bool get(ParameterHandle handle, HealthRecord& outRecord) const;

    // Number of currently registered parameters.
    size_t count() const;

    // Removes all registered parameters.
    void clear();

private:
    bool isValidHandle(ParameterHandle handle) const;

    HealthRecord records_[MAX_PARAMETERS];
    bool occupied_[MAX_PARAMETERS];
    size_t count_;
};

#endif
