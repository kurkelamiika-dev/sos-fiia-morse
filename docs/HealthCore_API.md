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

Opaque handle returned by HealthManager.

Application code stores the handle and uses it in future updates.

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

---

# Public API

## registerParameter()

Registers a new health parameter.

Returns ParameterHandle.

---

## unregisterParameter()

Removes a parameter.

Returns success/failure.

---

## report()

Updates parameter value.

Returns success/failure.

---

## get()

Returns one HealthRecord.

---

## count()

Returns number of registered parameters.

---

## clear()

Removes all registered parameters.

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
