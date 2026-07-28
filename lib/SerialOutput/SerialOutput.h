#ifndef SERIALOUTPUT_H
#define SERIALOUTPUT_H

#include "HealthSnapshot.h"

// Print is Arduino's abstract base class for anything printable-to
// (HardwareSerial / the global Serial object derives from it, via Stream).
// Forward-declared only, so this header stays compilable with a plain C++
// toolchain; the full definition is only needed in SerialOutput.cpp, where
// stream_'s methods are actually called.
class Print;

// SerialOutput is the Serial output module for the Health Framework: it
// renders an already-captured HealthSnapshot as human-readable text on a
// caller-supplied stream.
//
// SerialOutput depends only on HealthSnapshot. It never touches
// HealthManager or HealthCore: it does not capture a snapshot itself, and
// its public API never mentions either type. It has no notion of "now" and
// interprets no parameter values beyond formatting them for display —
// Freshness and every field's contents come entirely from the
// HealthSnapshot it is given.
//
// SerialOutput does not manage the stream's lifecycle. Opening or
// configuring it (for example Serial.begin()) remains the caller's
// responsibility, both before constructing a SerialOutput and for as long
// as it is used.
//
// Data flow:
//
//   Application -> HealthManager::registerParameter()/report() (elsewhere)
//   Application -> HealthSnapshot::capture(manager)             (elsewhere)
//   Application -> SerialOutput::print(snapshot)
//                -> for position in [0, snapshot.count()):
//                     HealthSnapshot::getAt(position, entry)
//                     entry.handle / entry.status formatted and written to
//                     the stream supplied at construction
//
// SerialOutput never receives a HealthManager or HealthCore reference
// anywhere in its API; it only ever renders a HealthSnapshot it is given.
class SerialOutput {
public:
    // stream is where every print() call writes its output. stream must
    // remain valid for the lifetime of this SerialOutput; opening or
    // configuring it (for example Serial.begin()) is the caller's
    // responsibility.
    explicit SerialOutput(Print& stream);

    // Writes a human-readable representation of every entry in snapshot to
    // the stream supplied at construction.
    void print(const HealthSnapshot& snapshot);

private:
    Print& stream_;
};

#endif
