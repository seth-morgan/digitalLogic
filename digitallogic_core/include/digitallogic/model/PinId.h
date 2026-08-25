/**
 * @file PinId.h
 * @brief Identifies a specific pin on a component (used as wire endpoints).
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/ComponentId.h"

namespace digitallogic {

/**
 * @brief Identifies a specific pin on a circuit component.
 */
struct PinId final {
    ComponentId componentId{};
    int pinIndex{0}; // Zero-based pin index on the component

    [[nodiscard]] bool operator==(const PinId& other) const noexcept
    {
        return componentId == other.componentId && pinIndex == other.pinIndex;
    }

    [[nodiscard]] bool operator!=(const PinId& other) const noexcept
    {
        return !(*this == other);
    }
};

} // namespace digitallogic

namespace std {

// Combine component hash with pin index for unordered-map lookup.
template <>
struct hash<digitallogic::PinId> {
    std::size_t operator()(const digitallogic::PinId& pin) const noexcept
    {
        const std::size_t componentHash = std::hash<digitallogic::ComponentId>{}(pin.componentId);
        // Boost-style hash combine so pinIndex collisions are less likely.
        return componentHash ^ (static_cast<std::size_t>(pin.pinIndex) + 0x9e3779b9U + (componentHash << 6) + (componentHash >> 2));
    }
};

} // namespace std
