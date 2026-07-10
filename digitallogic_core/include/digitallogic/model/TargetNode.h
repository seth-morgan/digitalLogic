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
