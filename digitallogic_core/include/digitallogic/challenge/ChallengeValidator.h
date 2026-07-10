#pragma once

#include "digitallogic/challenge/ChallengeLevel.h"
#include "digitallogic/model/Circuit.h"
#include "digitallogic/model/ComponentId.h"

#include <QString>

namespace digitallogic {

/**
 * @brief Outcome of validating a challenge solution.
 */
struct ChallengeValidationResult final {
    bool success{false};
    QString message;
};

/**
 * @brief Checks whether a circuit satisfies a challenge level.
 */
class ChallengeValidator final {
public:
    /**
     * @brief Validates gate usage, wiring, and truth-table expectations.
     * @param circuit Circuit built by the player.
     * @param level Challenge definition.
     * @param sourceIdsByLabel Maps source labels (e.g. "A") to component ids.
     * @param targetId Output target component id.
     */
    [[nodiscard]] static ChallengeValidationResult validate(const Circuit& circuit, const ChallengeLevel& level,
                                                          const QHash<QString, ComponentId>& sourceIdsByLabel,
                                                          ComponentId targetId);
};

} // namespace digitallogic
