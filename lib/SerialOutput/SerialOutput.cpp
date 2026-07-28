#include "SerialOutput.h"

#include <Print.h>

#include <cstddef>
#include <cstring>

namespace {

// Matches the label column width used in docs/HealthCore_Specification.md,
// Section 4's HealthRecord example ("Has Been Updated: " is 17 characters).
constexpr size_t LABEL_WIDTH = 17;

void printLabel(Print& stream, const char* label) {
    stream.print(label);

    for (size_t i = strlen(label); i < LABEL_WIDTH; ++i) {
        stream.print(' ');
    }

    stream.print(": ");
}

const char* priorityToString(Priority priority) {
    switch (priority) {
        case Priority::LOW:      return "LOW";
        case Priority::NORMAL:   return "NORMAL";
        case Priority::HIGH:     return "HIGH";
        case Priority::CRITICAL: return "CRITICAL";
    }

    return "UNKNOWN";
}

const char* freshnessToString(Freshness freshness) {
    switch (freshness) {
        case Freshness::UNKNOWN: return "UNKNOWN";
        case Freshness::FRESH:   return "FRESH";
        case Freshness::OLD:     return "OLD";
    }

    return "UNKNOWN";
}

void printEntry(Print& stream, const HealthSnapshotEntry& entry) {
    const HealthRecord& record = entry.status.record;

    printLabel(stream, "ID");
    stream.println(record.id);

    printLabel(stream, "Description");
    stream.println(record.description);

    printLabel(stream, "Value");
    stream.println(record.value);

    printLabel(stream, "Unit");
    stream.println(record.unit);

    printLabel(stream, "Source");
    stream.println(record.source);

    printLabel(stream, "Last Update");
    stream.print(record.lastUpdate);
    stream.println(" ms");

    printLabel(stream, "Timeout");
    stream.print(record.timeout);
    stream.println(" ms");

    printLabel(stream, "Priority");
    stream.println(priorityToString(record.priority));

    printLabel(stream, "Freshness");
    stream.println(freshnessToString(entry.status.freshness));
}

} // namespace

SerialOutput::SerialOutput(Print& stream) : stream_(stream) {
}

void SerialOutput::print(const HealthSnapshot& snapshot) {
    HealthSnapshotEntry entry;

    // getAt() cannot fail here: position ranges over [0, snapshot.count()).
    for (size_t position = 0; position < snapshot.count(); ++position) {
        snapshot.getAt(position, entry);
        printEntry(stream_, entry);
        stream_.println();
    }
}
