/**
 * @file TargetNode.h
 * @brief Declares a challenge-mode output target with a single input pin.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/ComponentId.h"

namespace digitallogic {

/**
 * @brief Fixed output target with a single input pin in challenge levels.
 */
class TargetNode final {
public:
    explicit TargetNode(ComponentId id);

    [[nodiscard]] ComponentId id() const noexcept { return m_id; }

private:
    ComponentId m_id;
};

} // namespace digitallogic
