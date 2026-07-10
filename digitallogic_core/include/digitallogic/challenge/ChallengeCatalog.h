#pragma once

#include "digitallogic/challenge/ChallengeLevel.h"

#include <QVector>

namespace digitallogic {

/**
 * @brief Built-in challenge levels shipped with the simulator.
 */
class ChallengeCatalog final {
public:
  [[nodiscard]] static const QVector<ChallengeLevel>& levels();
  [[nodiscard]] static int levelCount() noexcept;
  [[nodiscard]] static const ChallengeLevel* levelAt(int index);
};

} // namespace digitallogic
