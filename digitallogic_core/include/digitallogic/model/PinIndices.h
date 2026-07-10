#pragma once

namespace digitallogic {

/**
 * @brief Pin index for a source output (sources have a single output on pin 0).
 */
[[nodiscard]] inline constexpr int sourceOutputPinIndex() noexcept
{
    return 0;
}

/**
 * @brief Pin index for a gate input in the range [0, inputCount).
 */
[[nodiscard]] inline constexpr int gateInputPinIndex(const int inputIndex) noexcept
{
    return inputIndex;
}

/**
 * @brief Pin index for a gate output (distinct from all input indices).
 * @param inputCount Number of inputs on the gate.
 */
[[nodiscard]] inline constexpr int gateOutputPinIndex(const int inputCount) noexcept
{
    return inputCount;
}

/**
 * @brief Returns true when the pin index refers to a gate output pin.
 */
[[nodiscard]] inline constexpr bool isGateOutputPin(const int pinIndex, const int inputCount) noexcept
{
    return pinIndex == gateOutputPinIndex(inputCount);
}

} // namespace digitallogic
