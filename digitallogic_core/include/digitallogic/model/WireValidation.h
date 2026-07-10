#pragma once

namespace digitallogic {

/**
 * @brief Result of validating a proposed wire connection.
 */
enum class WireValidationResult {
    Ok,
    SameComponent,
    InvalidDestination,
    InvalidSource,
    InputAlreadyConnected
};

/**
 * @brief Human-readable description of a wire validation result.
 * @param result Validation outcome.
 * @return Short message suitable for UI display.
 */
[[nodiscard]] const char* wireValidationMessage(WireValidationResult result);

} // namespace digitallogic
