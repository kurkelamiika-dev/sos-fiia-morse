# HealthSnapshot Specification v0.1

## 1. Purpose

HealthSnapshot is a reusable embedded library that captures a consistent, point-in-time,
read-only copy of every parameter currently registered in a HealthManager.

HealthSnapshot contains **no application logic** and interprets no parameter values.

Its only responsibility is to decouple output modules from HealthManager, so that
Serial, Morse, OLED, MQTT, and future output modules can share one consistent view of the
current health state without each querying HealthManager directly.

HealthSnapshot must be reusable in any ESP32 project without modification.

---

## 2. Responsibilities

HealthSnapshot shall:

- Capture a point-in-time copy of every parameter registered in a HealthManager
- Preserve the handle, record, and freshness of each captured parameter
- Preserve HealthManager's enumeration order exactly as captured

HealthSnapshot shall **not**:

- Store parameters of its own; HealthCore remains the only storage backend
- Compute Freshness; HealthManager remains the only source of Freshness
- Read any clock, timer, `millis()`, RTC, or platform tick count
- Sort, filter, or otherwise reorder captured entries
- Format output for Serial, Morse, OLED, MQTT, WiFi, or any other output medium
- Allocate memory dynamically

---

## 3. Design Goals

- Small, fixed memory footprint
- Easy to understand
- No global mutable state
- Independent of output format
- Portable to Arduino and ESP-IDF
- Easy to extend without breaking compatibility
- Deterministic: `capture()` always visits exactly `manager.count()` entries, never a
  partial capture

---

## 4. HealthSnapshotEntry

Each captured parameter is represented by one HealthSnapshotEntry.

A HealthSnapshotEntry contains:

- ParameterHandle (the handle the parameter was captured from)
- HealthStatus (HealthRecord and Freshness, as computed by HealthManager at capture time)

HealthSnapshotEntry stores no data beyond these two fields. HealthRecord, HealthStatus,
and Freshness are defined by HealthCore and HealthManager respectively; HealthSnapshot
does not redefine or reinterpret them (see HealthCore_Specification.md, Sections 4 and
5).

The handle in a HealthSnapshotEntry is opaque metadata, exactly as ParameterHandle is
defined in HealthCore_Specification.md, Section 16. It may be used by a consumer to refer
back to the originating parameter through HealthManager, but a consumer must never
construct or modify a ParameterHandle itself. HealthSnapshot itself does not use the
handle for anything; it only carries it forward from HealthManager.

---

## 5. Capture Semantics

A HealthSnapshot is populated by calling `capture(manager)`.

Rules:

- `capture()` replaces all of a HealthSnapshot's prior contents; it does not merge or
  append.
- `capture()` visits every parameter currently registered in `manager`, in the exact
  order `HealthManager::getAt()` yields them. It performs no sorting and no filtering.
- If a `capture()` call yields fewer entries than a previous one, positions at or beyond
  the new `count()` are not cleared; they simply become unreachable through `getAt()`,
  consistent with `count()` being the sole gate on validity.
- After `capture()` returns, the HealthSnapshot is fully decoupled from `manager`: later
  calls to `report()`, `registerParameter()`, `unregisterParameter()`, or `clear()` on
  `manager` do not affect a snapshot already captured.
- A HealthSnapshot is a reusable, passive data container, not a one-shot object. The same
  instance may be captured repeatedly over its lifetime; each `capture()` call fully
  replaces the previous contents.
- Presentation ordering (for example, by Priority) is not decided by HealthSnapshot.
  Different output modules may require different orderings for their own medium;
  HealthSnapshot leaves that decision to its consumers.

---

## 6. Dependencies

HealthSnapshot's only behavioral dependency is HealthManager: every method HealthSnapshot
calls to obtain parameter data is a HealthManager method (`count()`, `getAt()`).
HealthSnapshot never instantiates a HealthCore object and never calls a HealthCore method
directly.

HealthSnapshot does reuse HealthCore's public types directly — ParameterHandle,
HealthRecord, Freshness, and the MAX_PARAMETERS constant that sizes its internal array —
since these are the shared data model defined in HealthCore_Specification.md, Sections 4
through 6 and 15. Reusing these types is not a storage dependency: HealthCore remains the
only storage backend, and it is reached only through HealthManager.

HealthSnapshot shall not depend on:

- MorseOutput
- SerialOutput
- OLEDOutput
- MQTTOutput
- WiFi
- FreeRTOS tasks

HealthSnapshot shall not read time from any clock, timer, `millis()`, RTC, or FreeRTOS
tick count. Every timestamp visible in a captured entry is the Last Update Time value
HealthManager had already recorded at capture time.

---

## 7. Memory Policy

- No dynamic memory allocation.
- No use of `new` or `delete`.
- No mutable global objects.
- Each HealthSnapshot holds a fixed-size internal array of HealthSnapshotEntry, sized
  MAX_PARAMETERS (see Section 8); `capture()` only ever writes within this fixed
  capacity.
- Design for long-term reliable operation without memory fragmentation.

---

## 8. Capacity

HealthSnapshot defines no compile-time constants of its own. Its internal capacity is
HealthCore's own MAX_PARAMETERS constant (see HealthCore_Specification.md, Section 15):

```
MAX_PARAMETERS = 64
```

A HealthSnapshot can therefore never hold more entries than HealthCore can ever have
registered at once. Since `capture()` only ever visits `manager.count()` parameters, and
HealthManager's `count()` can never exceed MAX_PARAMETERS, `capture()` can never write
past the end of this array.

---

## 9. Failure Behavior

- `getAt(position, outEntry)` returns false, leaving `outEntry` unchanged, if
  `position >= count()`.
- `capture()` cannot itself fail: it always succeeds, and always results in `count()`
  equal to `manager`'s `count()` at the moment of the call.
- HealthSnapshot performs no validation of parameter content. The HealthRecord and
  ParameterHandle inside each entry are copied verbatim from HealthManager, which is
  itself responsible for their validity.

---

## 10. Timestamps and Freshness

HealthSnapshot never reads any clock, and never computes Freshness. Every Last Update
Time and every Freshness value visible in a captured entry was already computed by
HealthManager before `capture()` was called (see HealthCore_Specification.md, Sections 5
and 17).

Freshness is not recalculated as time passes after capture. A HealthSnapshot reflects the
health state exactly as it stood at the moment of `capture()`, not the current state of
`manager`.

---

## 11. Clarification Log

The following clarifications were made with the project owner on 2026-07-28, while
designing HealthSnapshot, prior to first implementation:

- HealthSnapshot is a reusable, passive data container rather than a one-shot object: it
  is default-constructed empty and repopulated by calling `capture()`, rather than
  requiring reconstruction for every fresh view (Section 5).
- Each HealthSnapshotEntry carries both the originating ParameterHandle and the
  HealthStatus, rather than HealthStatus alone, so a future consumer can trace an entry
  back to its live parameter through HealthManager. The handle remains opaque metadata;
  HealthSnapshot does not use it for anything itself (Section 4).
- `capture()` preserves HealthManager's raw enumeration order and performs no sorting or
  filtering. Presentation ordering is left entirely to output modules, since different
  output media may require different orderings (Section 5).
- "Thread-safe by design" was replaced with "No global mutable state" (Section 3). The
  latter is what HealthSnapshot actually guarantees; the former would additionally claim
  synchronized concurrent access to a single instance (for example concurrent
  `capture()`/`getAt()` calls), which HealthSnapshot does not provide.
