#include "HealthSnapshot.h"

HealthSnapshot::HealthSnapshot() : count_(0) {
}

void HealthSnapshot::capture(const HealthManager& manager) {
    count_ = manager.count();

    // getAt() cannot fail here: position ranges over [0, count_), and
    // count_ was just read from this same manager.
    for (size_t position = 0; position < count_; ++position) {
        manager.getAt(position, entries_[position].handle, entries_[position].status);
    }
}

size_t HealthSnapshot::count() const {
    return count_;
}

bool HealthSnapshot::getAt(size_t position, HealthSnapshotEntry& outEntry) const {
    if (position >= count_) {
        return false;
    }

    outEntry = entries_[position];
    return true;
}
