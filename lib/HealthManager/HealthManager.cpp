#include "HealthManager.h"

namespace {

// now - record.lastUpdate relies on uint32_t wraparound to stay correct
// across a millis()-style rollover, as long as the true elapsed time is
// under 2^32 units.
Freshness computeFreshness(const HealthRecord& record, uint32_t now) {
    if (!record.hasBeenUpdated) {
        return Freshness::UNKNOWN;
    }

    const uint32_t elapsed = now - record.lastUpdate;
    return (elapsed <= record.timeout) ? Freshness::FRESH : Freshness::OLD;
}

} // namespace

HealthManager::HealthManager(TimeSource timeSource) : timeSource_(timeSource) {
}

ParameterHandle HealthManager::registerParameter(const char* id,
                                                  const char* description,
                                                  const char* unit,
                                                  const char* source,
                                                  uint32_t timeout,
                                                  Priority priority) {
    return core_.registerParameter(id, description, unit, source, timeout, priority);
}

bool HealthManager::unregisterParameter(ParameterHandle handle) {
    return core_.unregisterParameter(handle);
}

bool HealthManager::report(ParameterHandle handle, const char* value) {
    return core_.report(handle, value, timeSource_());
}

bool HealthManager::get(ParameterHandle handle, HealthStatus& outStatus) const {
    if (!core_.get(handle, outStatus.record)) {
        return false;
    }

    outStatus.freshness = computeFreshness(outStatus.record, timeSource_());
    return true;
}

bool HealthManager::getAt(size_t position, ParameterHandle& outHandle, HealthStatus& outStatus) const {
    if (!core_.getAt(position, outHandle, outStatus.record)) {
        return false;
    }

    outStatus.freshness = computeFreshness(outStatus.record, timeSource_());
    return true;
}

size_t HealthManager::count() const {
    return core_.count();
}

void HealthManager::clear() {
    core_.clear();
}
