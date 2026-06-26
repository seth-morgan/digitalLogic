#pragma once

#include <cstdint>
#include <functional>

namespace digitallogic {

/**
 * @brief Stable identifier for a circuit component (source or gate).
 */
struct ComponentId final {
    std::uint64_t value{0};

    [[nodiscard]] bool operator==(const ComponentId& other) const noexcept
    {
        return value == other.value;
    }

    [[nodiscard]] bool operator!=(const ComponentId& other) const noexcept
    {
        return !(*this == other);
    }
};

} // namespace digitallogic

namespace std {

template <>
struct hash<digitallogic::ComponentId> {
    std::size_t operator()(const digitallogic::ComponentId& id) const noexcept
    {
        return std::hash<std::uint64_t>{}(id.value);
    }
};

} // namespace std
