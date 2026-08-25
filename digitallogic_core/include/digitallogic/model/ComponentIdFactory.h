/**
 * @file ComponentIdFactory.h
 * @brief Declares helpers that allocate and reseeds unique ComponentId values.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/ComponentId.h"

#include <cstdint>

namespace digitallogic {

/**
 * @brief Allocates a new unique component identifier.
 * @return Monotonically increasing component id.
 */
[[nodiscard]] ComponentId makeComponentId();

/**
 * @brief Resets the component id generator (used after loading a saved circuit).
 * @param nextValue Next id to assign.
 */
void seedComponentIdGenerator(std::uint64_t nextValue);

} // namespace digitallogic
