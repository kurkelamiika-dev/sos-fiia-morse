# HealthSnapshot API

Version : 0.1

Status : Draft

Approved :

---

# Purpose

This document defines the public API of HealthSnapshot.

It defines only the interface.

No implementation is included.

---

# Public Types

## HealthSnapshotEntry

Contains:

- handle (ParameterHandle; opaque metadata, see HealthCore_API.md)
- status (HealthStatus; HealthRecord and Freshness, as computed by HealthManager)

HealthSnapshotEntry stores no data beyond these two fields.

---

# Public API

## HealthSnapshot()

Constructs an empty HealthSnapshot.

count() is 0 until the first capture().

---

## capture()

Replaces the HealthSnapshot's contents with a fresh, point-in-time copy of every
parameter currently registered in the supplied HealthManager.

Visits parameters in the exact order HealthManager::getAt() yields them; performs no
sorting or filtering. Always succeeds, and always leaves count() equal to the
HealthManager's count() at the moment of the call.

Once capture() returns, the HealthSnapshot is fully decoupled from the HealthManager it
was captured from; later changes to that HealthManager do not affect this HealthSnapshot
until capture() is called again.

---

## count()

Returns the number of entries captured by the most recent capture(). 0 before the first
capture().

---

## getAt()

Retrieves the entry at position from the most recent capture().

Returns success/failure indicating whether position was in range. On success, fills the
supplied HealthSnapshotEntry output parameter; on failure, it is left unchanged.

position ranges over [0, count()).

---

# Capacity

HealthSnapshot defines no compile-time constants of its own. Its internal capacity is
HealthCore's MAX_PARAMETERS (see HealthCore_API.md, Compile-Time Constants).

---

# Out of Scope

This API does NOT contain:

- Morse
- Serial
- OLED
- MQTT
- WiFi
- Sorting or filtering of captured entries
- Freshness calculation
- Parameter storage

Those belong to HealthCore, HealthManager, or higher-level output libraries.

---

# Clarification Log

The following clarifications were added on 2026-07-28, prior to first implementation,
based on decisions made with the project owner (see HealthSnapshot_Specification.md,
Section 11 for the full rationale):

- HealthSnapshot is default-constructible and reusable via capture(), rather than a
  one-shot constructor-only object.
- HealthSnapshotEntry carries both ParameterHandle and HealthStatus.
- capture() preserves HealthManager's raw enumeration order; no sorting or filtering is
  performed.
