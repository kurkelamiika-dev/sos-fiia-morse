#include "HealthCore.h"

#include <cstring>

namespace {

// Copies the null-terminated string at source into destination if it fits
// (including the terminator). Returns false, leaving destination untouched,
// otherwise. Never truncates.
bool copyToBuffer(char* destination, size_t destinationSize, const char* source) {
    size_t length = 0;

    while (source[length] != '\0') {
        ++length;

        if (length >= destinationSize) {
            return false;
        }
    }

    memcpy(destination, source, length + 1);
    return true;
}

} // namespace

HealthCore::HealthCore() : count_(0) {
    clear();
}

ParameterHandle HealthCore::registerParameter(const char* id,
                                               const char* description,
                                               const char* unit,
                                               const char* source,
                                               uint32_t timeout,
                                               Priority priority) {
    if (count_ >= MAX_PARAMETERS) {
        return INVALID_PARAMETER_HANDLE;
    }

    int slot = -1;

    for (int i = 0; i < static_cast<int>(MAX_PARAMETERS); ++i) {
        if (!occupied_[i]) {
            slot = i;
            break;
        }
    }

    if (slot < 0) {
        return INVALID_PARAMETER_HANDLE;
    }

    HealthRecord candidate{};

    if (!copyToBuffer(candidate.id, MAX_ID_LENGTH, id) ||
        !copyToBuffer(candidate.description, MAX_DESCRIPTION_LENGTH, description) ||
        !copyToBuffer(candidate.unit, MAX_UNIT_LENGTH, unit) ||
        !copyToBuffer(candidate.source, MAX_SOURCE_LENGTH, source)) {
        return INVALID_PARAMETER_HANDLE;
    }

    candidate.value[0] = '\0';
    candidate.timeout = timeout;
    candidate.priority = priority;
    candidate.lastUpdate = 0;
    candidate.hasBeenUpdated = false;

    records_[slot] = candidate;
    occupied_[slot] = true;
    ++count_;

    return ParameterHandle{slot};
}

bool HealthCore::unregisterParameter(ParameterHandle handle) {
    if (!isValidHandle(handle)) {
        return false;
    }

    occupied_[handle.index] = false;
    --count_;
    return true;
}

bool HealthCore::report(ParameterHandle handle, const char* value, uint32_t timestamp) {
    if (!isValidHandle(handle)) {
        return false;
    }

    char candidateValue[MAX_VALUE_LENGTH] = {};

    if (!copyToBuffer(candidateValue, MAX_VALUE_LENGTH, value)) {
        return false;
    }

    HealthRecord& record = records_[handle.index];
    memcpy(record.value, candidateValue, sizeof(candidateValue));
    record.lastUpdate = timestamp;
    record.hasBeenUpdated = true;
    return true;
}

bool HealthCore::get(ParameterHandle handle, HealthRecord& outRecord) const {
    if (!isValidHandle(handle)) {
        return false;
    }

    outRecord = records_[handle.index];
    return true;
}

bool HealthCore::getAt(size_t position, ParameterHandle& outHandle, HealthRecord& outRecord) const {
    if (position >= count_) {
        return false;
    }

    size_t seen = 0;

    for (size_t i = 0; i < MAX_PARAMETERS; ++i) {
        if (!occupied_[i]) {
            continue;
        }

        if (seen == position) {
            outHandle = ParameterHandle{static_cast<int>(i)};
            outRecord = records_[i];
            return true;
        }

        ++seen;
    }

    return false;
}

size_t HealthCore::count() const {
    return count_;
}

void HealthCore::clear() {
    for (size_t i = 0; i < MAX_PARAMETERS; ++i) {
        occupied_[i] = false;
    }

    count_ = 0;
}

bool HealthCore::isValidHandle(ParameterHandle handle) const {
    if (handle.index < 0 || handle.index >= static_cast<int>(MAX_PARAMETERS)) {
        return false;
    }

    return occupied_[handle.index];
}
