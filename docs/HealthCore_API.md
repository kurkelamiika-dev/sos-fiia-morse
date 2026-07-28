# HealthCore API

Version : 0.1

Status : Draft

Approved :

---

# Purpose

This document defines the public API of HealthCore.

It defines only the interface.

No implementation is included.

---

# Public Types

## Priority

LOW

NORMAL

HIGH

CRITICAL

---

## Freshness

UNKNOWN

FRESH

OLD

---

## ParameterHandle

Opaque handle identifying a registered parameter.

Application code stores the handle and uses it in future updates. Its current
implementation contains an index into HealthCore's fixed-size internal parameter table,
but application code must not depend on this representation.

A reserved invalid handle value means "no parameter" and is returned when registration
fails.

---

## HealthRecord

Contains:

- id
- description
- value
- unit
- source
- lastUpdate
- timeout
- priority
- hasBeenUpdated (internal; false until the parameter's first successful report(), used
  to represent the UNKNOWN freshness case)

All text fields (id, description, value, unit, source) are fixed-size character buffers
sized by compile-time constants (see Compile-Time Constants below). Value is always
stored as text.

---

# Public API

## registerParameter()

Registers a new health parameter.

Returns ParameterHandle. Returns the invalid handle if the parameter table is full, or
if any supplied text does not fit its fixed buffer.

---

## unregisterParameter()

Removes a parameter.

Returns success/failure. Fails if the handle is invalid or already unregistered.

---

## report()

Updates parameter value.

HealthCore does not read any clock, timer, or platform-specific time source. The
timestamp recorded alongside the update originates from HealthManager.

Returns success/failure. Fails, leaving the record unchanged, if the handle is invalid
or the supplied value does not fit its fixed buffer. Values are never silently
truncated.

---

## get()

Returns one HealthRecord, together with success/failure indicating whether the handle
was valid.

---

## count()

Returns number of registered parameters.

---

## clear()

Removes all registered parameters.

---

# Compile-Time Constants

HealthCore's capacity and buffer sizes are fixed at compile time:

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

# Out of Scope

This API does NOT contain:

- Morse
- Serial
- OLED
- WiFi
- MQTT
- Snapshot generation

Those belong to higher-level libraries.

---

# Clarification Log

The following clarifications were added on 2026-07-28, prior to first implementation,
based on decisions made with the project owner (see HealthCore_Specification.md, Section
18 for the full rationale):

- Fixed capacity and buffer size constants (Compile-Time Constants).
- ParameterHandle described as opaque, with its current implementation noted separately
  from its contract.
- hasBeenUpdated added to HealthRecord to represent the UNKNOWN freshness case.
- report()/get()/registerParameter() failure behavior on invalid handles and oversized
  strings made explicit; overflow is rejected, never truncated.
- Clarified that HealthCore never reads a clock — timestamps originate from
  HealthManager.
