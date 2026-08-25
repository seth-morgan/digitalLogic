/**
 * @file ChallengeCatalog.h
 * @brief Declares the built-in catalog of challenge levels.
 * @author Seth Morgan
 * @date 2026-08-25
 */
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
