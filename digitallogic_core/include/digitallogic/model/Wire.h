/**
 * @file Wire.h
 * @brief Declares a directed connection between two circuit pins.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/PinId.h"

namespace digitallogic {

/**
 * @brief Directed connection from an output pin to an input pin.
 */
struct Wire final {
    PinId from; // Output pin (source output or gate output)
    PinId to;   // Input pin (gate or challenge target input)
};

} // namespace digitallogic
