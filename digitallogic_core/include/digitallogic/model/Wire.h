#pragma once

#include "digitallogic/model/PinId.h"

namespace digitallogic {

/**
 * @brief Directed connection from an output pin to an input pin.
 */
struct Wire final {
    PinId from; ///< Output pin (source output or gate output).
    PinId to;   ///< Input pin (gate input only in baseline).
};

} // namespace digitallogic
