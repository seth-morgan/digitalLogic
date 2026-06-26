#pragma once

namespace digitallogic {

/**
 * @brief Digital signal value used throughout the simulation engine.
 */
enum class SignalValue {
    False = 0,
    True = 1,
    Unknown = 2
};

/**
 * @brief Converts a signal value to a human-readable label.
 * @param value Signal to format.
 * @return "0", "1", or "?".
 */
[[nodiscard]] const char* toString(SignalValue value);

} // namespace digitallogic
