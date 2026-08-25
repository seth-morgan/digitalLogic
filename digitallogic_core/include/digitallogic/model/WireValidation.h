/**
 * @file WireValidation.h
 * @brief Declares wire-connection validation results and user-facing messages.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

namespace digitallogic {

/**
 * @brief Result of validating a proposed wire connection.
 */
enum class WireValidationResult {
    Ok,
    SameComponent,           // From and to belong to the same component
    InvalidDestination,      // Destination is not a valid input pin
    InvalidSource,           // Source is not a valid output pin
    InputAlreadyConnected    // Destination input already has a wire
};

/**
 * @brief Human-readable description of a wire validation result.
 * @param result Validation outcome.
 * @return Short message suitable for UI display.
 */
[[nodiscard]] const char* wireValidationMessage(WireValidationResult result);

} // namespace digitallogic
