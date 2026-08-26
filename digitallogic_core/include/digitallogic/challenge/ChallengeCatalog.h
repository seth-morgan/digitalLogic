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
    /**
     * @brief Returns the full list of built-in challenge levels.
     */
    [[nodiscard]] static const QVector<ChallengeLevel>& levels();

    /**
     * @brief Returns how many challenge levels are in the catalog.
     */
    [[nodiscard]] static int levelCount() noexcept;

    /**
     * @brief Returns the challenge level at the given index, or nullptr if out of range.
     * @param index Zero-based level index.
     */
    [[nodiscard]] static const ChallengeLevel* levelAt(int index);
};

} // namespace digitallogic
