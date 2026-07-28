#ifndef HEALTHSNAPSHOT_H
#define HEALTHSNAPSHOT_H

#include <cstddef>

#include "HealthCore.h"
#include "HealthManager.h"

// HealthSnapshot is a pure read-only presentation-data layer: a fixed-size,
// point-in-time copy of every parameter currently registered in a
// HealthManager, taken via HealthManager::getAt() (itself backed solely by
// HealthCore::getAt() — see docs/HealthCore_Specification.md, Section 16).
//
// HealthSnapshot owns no parameter storage beyond the snapshot it produces,
// reads no clock of its own (every timestamp a consumer sees is the
// lastUpdate already computed by HealthManager), and computes no Freshness
// itself — HealthManager remains the only source of Freshness, HealthCore
// remains the only storage backend. HealthSnapshot knows nothing about
// Serial, Morse, OLED, MQTT, WiFi, or any other output format; it exists so
// those output modules share one consistent, decoupled view of the current
// health state instead of each querying HealthManager directly.
//
// A captured snapshot is fully decoupled from HealthManager: once capture()
// returns, later changes to HealthManager (report(), registerParameter(),
// unregisterParameter(), clear()) do not affect a previously captured
// snapshot. capture() performs no sorting or filtering — entries appear in
// whatever order HealthManager::getAt() yields them. Presentation ordering
// (e.g. by Priority) is left to individual output modules, not decided here,
// since different output media may want different orders.

// One entry in a captured snapshot: the handle a parameter was captured
// from, together with its record and freshness at capture time.
//
// handle is opaque metadata only (see ParameterHandle in HealthCore.h):
// consumers may use it to refer back to the originating parameter through
// HealthManager (e.g. future interactive UI, diagnostics, acknowledgements),
// but must never construct or modify a ParameterHandle themselves.
struct HealthSnapshotEntry {
    ParameterHandle handle;
    HealthStatus status;
};

// Data flow:
//
//   Output module -> HealthSnapshot::capture(manager)
//                  -> for position in [0, manager.count()):
//                       HealthManager::getAt(position, handle, status)
//                       (handle from HealthCore, status = record + freshness
//                       computed by HealthManager)
//                  -> stored verbatim, in that order, as entries_[position]
//
//   Output module -> HealthSnapshot::count()
//                  -> number of entries captured
//
//   Output module -> HealthSnapshot::getAt(position, outEntry)
//                  -> outEntry = entries_[position], unchanged since capture()
//
// HealthSnapshot is a reusable, passive data container, not a one-shot
// object: the same instance is expected to be captured repeatedly over its
// lifetime (e.g. once per output cycle), with each capture() replacing all
// prior contents.
class HealthSnapshot {
public:
    // Starts empty (count() == 0) until the first capture().
    HealthSnapshot();

    // Replaces this snapshot's contents with a fresh, point-in-time copy of
    // every parameter currently registered in manager. Deterministic and
    // allocation-free: entries are written into a fixed-size internal array
    // sized MAX_PARAMETERS (HealthCore's own capacity constant), in the same
    // order HealthManager::getAt() yields them.
    void capture(const HealthManager& manager);

    // Number of entries captured by the most recent capture(). 0 before the
    // first capture().
    size_t count() const;

    // Retrieves the entry at position from the most recent capture().
    // Returns false, leaving outEntry unchanged, if position >= count().
    bool getAt(size_t position, HealthSnapshotEntry& outEntry) const;

private:
    HealthSnapshotEntry entries_[MAX_PARAMETERS];
    size_t count_;
};

#endif
