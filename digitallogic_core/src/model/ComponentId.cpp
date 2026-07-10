#include "digitallogic/model/ComponentId.h"

#include <atomic>

namespace digitallogic {

namespace {

std::atomic<std::uint64_t> g_nextComponentId{1};

} // namespace

ComponentId makeComponentId()
{
    return ComponentId{g_nextComponentId.fetch_add(1, std::memory_order_relaxed)};
}

void seedComponentIdGenerator(const std::uint64_t nextValue)
{
    g_nextComponentId.store(nextValue, std::memory_order_relaxed);
}

} // namespace digitallogic
