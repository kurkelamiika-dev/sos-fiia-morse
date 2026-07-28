# HealthCore Specification v0.1

## 1. Purpose

HealthCore is a reusable embedded library that defines the common data model used by the Health Framework.

HealthCore contains **no application logic**.

Its only responsibility is to define the data structures shared by all other modules.

HealthCore must be reusable in any ESP32 project without modification.

---

## 2. Responsibilities

HealthCore shall define:

- Common data types
- HealthRecord structure
- Priority definitions
- Freshness definitions
- Common constants
- Parameter identifiers
- Public type definitions

HealthCore shall **not**:

- Start tasks
- Calculate timeouts
- Blink LEDs
- Print to Serial
- Communicate over WiFi
- Allocate background resources

---

## 3. Design Goals

- Small memory footprint
- Easy to understand
- Thread-safe by design (no global mutable state)
- Independent of application logic
- Portable to Arduino and ESP-IDF
- Easy to extend without breaking compatibility

---

## 4. HealthRecord

Each monitored parameter is represented by one HealthRecord.

A HealthRecord contains:

- Parameter ID
- Description
- Current Value
- Unit
- Source
- Last Update Time
- Timeout
- Priority
- Has Been Updated (internal flag; false until the first successful report(), see Section 16)

All text fields (Parameter ID, Description, Value, Unit, Source) are stored in fixed-size
character arrays sized by the compile-time constants in Section 15. There is no dynamic
allocation and no use of `String`. Value is always stored as text, regardless of whether
the underlying reading is numeric or symbolic (see Section 7).

Example:

```
ID              : S1
Description     : Engine Temperature
Value           : 82.4
Unit            : °C
Source          : CAN
Last Update     : 123456789 ms
Timeout         : 8000 ms
Priority        : HIGH
Has Been Updated: true
```

---

## 5. Freshness

Freshness is **never stored**.

It is always calculated by HealthManager.

Possible values:

- UNKNOWN
- FRESH
- OLD

Rules:

- UNKNOWN = Parameter has never been updated (determined from the record's internal
  Has Been Updated flag, not by inspecting Last Update Time).
- FRESH = Last update age ≤ timeout.
- OLD = Last update age > timeout.

---

## 6. Priority

Four reporting priorities:

- LOW
- NORMAL
- HIGH
- CRITICAL

Priority affects presentation order only.

---

## 7. Value

HealthCore does not interpret parameter values.

Examples:

- OK
- ERROR
- ONLINE
- OFFLINE
- FIX3D
- 23.5
- 82.4
- 12.8
- 75%

The meaning belongs to the application.

---

## 8. Parameter ID

Every parameter has a short identifier suitable for Morse transmission.

Examples:

- W
- MQTT
- GPS
- RTC
- S1
- S2
- TEMP
- BAT

---

## 9. Description

Human-readable parameter name.

Examples:

- WiFi Connection
- Engine Temperature
- Battery Voltage
- GPS Receiver

---

## 10. Source

Optional information describing where the data originates.

Examples:

- WiFi
- MQTT
- CAN
- NMEA2000
- BME280
- ADC

---

## 11. Unit

Optional engineering unit.

Examples:

- °C
- V
- A
- rpm
- %
- kn
- m

---

## 12. Dependencies

HealthCore shall not depend on:

- HealthManager
- MorseOutput
- SerialOutput
- OLED
- WiFi
- MQTT
- FreeRTOS tasks

HealthCore shall not read time from any clock, timer, `millis()`, RTC, or FreeRTOS tick
count. It has no notion of "now." All timestamps stored in Last Update Time are supplied
explicitly by the caller (HealthManager) — see Section 17.

---

## 13. Future Extensions

Reserved for:

- History
- Logging
- JSON export
- MQTT export
- OLED output
- Web UI
- SD card storage

---

## 14. Memory Policy

- Avoid unnecessary dynamic memory allocation.
- Avoid use of `new` and `delete`.
- Avoid mutable global objects.
- Prefer deterministic memory usage.
- Design for long-term reliable operation without memory fragmentation.

---

## 15. Implementation Constants (v0.1)

HealthCore uses a fixed-capacity table of parameters, sized entirely at compile time:

```
MAX_PARAMETERS         = 64
MAX_ID_LENGTH          = 8
MAX_DESCRIPTION_LENGTH = 32
MAX_VALUE_LENGTH       = 24
MAX_UNIT_LENGTH        = 8
MAX_SOURCE_LENGTH      = 16
```

All length constants include the terminating null character.

---

## 16. Handle Validity, Failure Behavior & Enumeration

- ParameterHandle is an opaque handle. Its current implementation contains an index into
  the fixed-size internal parameter table (capacity MAX_PARAMETERS); application code
  must not depend on this representation.
- A reserved invalid handle value represents "no parameter" and is returned when
  registration fails.
- Registration fails (returns the invalid handle) if:
  - the table is full (count() == MAX_PARAMETERS), or
  - any supplied string does not fit its fixed buffer, including the null terminator.
- Reporting a value fails (returns false, record left unchanged) if:
  - the handle is invalid or has been unregistered, or
  - the supplied value does not fit MAX_VALUE_LENGTH, including the null terminator.
- No string is ever silently truncated in v0.1. Overflow is rejected, not truncated.
- v0.1 has no handle generation counter. If a slot is freed by removing a parameter and
  later reused by a new registration, a handle obtained before the removal may silently
  address the new occupant of that slot. This is an accepted limitation for v0.1, not an
  oversight, and may be revisited in a future version.
- HealthCore provides `getAt(position, outHandle, outRecord)` so callers can enumerate
  every registered parameter without already holding its handle. No other v0.1 operation
  can discover an unknown handle, yet enumeration is required by any consumer that must
  act on the whole parameter table rather than one named parameter — including the
  Section 13 future extensions (History, Logging, JSON export, MQTT export, OLED output,
  Web UI, SD card storage) reserved for HealthCore itself, none of which are possible
  without it.
- `getAt()` exists to preserve handle opacity, not to work around it. Without it, a
  caller needing to enumerate parameters could be tempted to synthesize
  `ParameterHandle` values by walking an assumed index range from 0 upward, since the
  current representation happens to be a table index. That would violate "application
  code must not depend on this representation" above, and would break silently if a
  future version added a handle generation counter (previous bullet). `getAt()` keeps
  HealthCore as the only code that ever constructs a `ParameterHandle`; every caller, at
  every layer, only ever receives handles that HealthCore itself produced — from
  `registerParameter()` or from `getAt()`.
- `position` ranges over `[0, count())`. `getAt()` returns false, leaving `outHandle` and
  `outRecord` unchanged, if `position` is out of range.
- The mapping from `position` to a parameter is internal to HealthCore and is not
  guaranteed to match registration order. It is stable only until the next
  `registerParameter()`, `unregisterParameter()`, or `clear()` call.
- `getAt()` is purely additive: it does not change the behavior, signature, or failure
  conditions of `registerParameter()`, `unregisterParameter()`, `report()`, `get()`,
  `count()`, or `clear()`.

---

## 17. Timestamp Ownership

HealthCore never reads any clock, timer, RTC, `millis()`, or platform-specific time
source. All timestamps stored in HealthRecord originate from HealthManager.

This preserves the "portable to Arduino and ESP-IDF without modification" design goal
from Section 3, since no platform-specific time API is ever referenced inside HealthCore.

---

## 18. Clarification Log

The following clarifications were added to this v0.1 draft on 2026-07-28, prior to first
implementation, based on decisions made with the project owner. They are recorded here so
the additions are visible rather than silent:

- Fixed capacity of 64 parameters (Section 15).
- ParameterHandle described as an opaque handle whose current implementation is an index
  into the fixed-size parameter table, with an accepted lack of a generation counter in
  v0.1 (Section 16).
- Fixed-size character array buffers for all text fields, with concrete sizes
  (Section 15).
- Value is always stored as text.
- String overflow is rejected (call fails), never truncated (Section 16).
- HealthCore does not read any clock; it was clarified that HealthManager, not
  HealthCore, owns time (Section 12, Section 17). This corrects an earlier assumption
  that HealthCore would read a timestamp itself when a value is reported.
- HealthRecord gains an internal Has Been Updated flag to represent the UNKNOWN
  freshness case, since Last Update Time values are now opaque caller-supplied numbers
  with no reserved "never updated" sentinel (Section 4, Section 5).

### Addendum (2026-07-28, during HealthManager design)

While designing HealthManager on top of the frozen v0.1 API above, it became clear that
no existing operation lets any caller enumerate the registered parameter table — every
operation requires already holding a handle. This blocks not only HealthManager but also
every future consumer that must act on all registered parameters rather than one named
parameter, including the Section 13 future extensions reserved for HealthCore itself
(History, Logging, JSON export, MQTT export, OLED output, Web UI, SD card storage). This
addendum, made with the project owner, closes that gap without reopening the rest of the
frozen v0.1 API:

- Added `HealthCore::getAt(position, outHandle, outRecord)` (Section 16) as a purely
  additive enumeration primitive. It changes no existing method's signature, behavior,
  or failure conditions.
- `getAt()` was chosen specifically to preserve `ParameterHandle` opacity rather than
  work around it: it returns handles obtained from HealthCore's own table, the same way
  `registerParameter()` does, so no caller at any layer ever needs to construct a
  `ParameterHandle` itself from an assumed index representation (Section 16).
