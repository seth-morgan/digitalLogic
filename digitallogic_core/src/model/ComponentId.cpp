/**
 * @file ComponentId.cpp
 * @brief Implements thread-safe allocation of unique ComponentId values.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/model/ComponentId.h"

#include <atomic>

namespace digitallogic {

namespace {

// Shared counter; starts at 1 so 0 can remain a sentinel "empty" id.
std::atomic<std::uint64_t> g_nextComponentId{1};

} // namespace

ComponentId makeComponentId()
{
    // Atomically claim the next id without requiring a mutex.
    return ComponentId{g_nextComponentId.fetch_add(1, std::memory_order_relaxed)};
}

void seedComponentIdGenerator(const std::uint64_t nextValue)
{
    // After deserialize, jump the counter past the highest loaded id.
    g_nextComponentId.store(nextValue, std::memory_order_relaxed);
}

} // namespace digitallogic
