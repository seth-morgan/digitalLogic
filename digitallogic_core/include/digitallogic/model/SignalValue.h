/**
 * @file SignalValue.h
 * @brief Defines the three-state digital signal used across the simulator.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

namespace digitallogic {

/**
 * @brief Digital signal value used throughout the simulation engine.
 */
enum class SignalValue {
    False = 0,   // Logic low
    True = 1,    // Logic high
    Unknown = 2  // Unconnected or unresolved wire
};

/**
 * @brief Converts a signal value to a human-readable label.
 * @param value Signal to format.
 * @return "0", "1", or "?".
 */
[[nodiscard]] const char* toString(SignalValue value);

} // namespace digitallogic
