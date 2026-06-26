#pragma once

#include "digitallogic/model/ComponentId.h"

namespace digitallogic {

/**
 * @brief Allocates a new unique component identifier.
 * @return Monotonically increasing component id.
 */
[[nodiscard]] ComponentId makeComponentId();

} // namespace digitallogic
