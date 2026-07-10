#pragma once

#include "digitallogic/model/Gate.h"
#include "digitallogic/model/SignalValue.h"

#include <QHash>
#include <QPointF>
#include <QString>
#include <QVector>

namespace digitallogic {

/**
 * @brief Pre-placed source specification for a challenge level.
 */
struct ChallengeSourceSpec final {
    QString label;
    QPointF position;
};

/**
 * @brief Allowed gate count for a challenge level.
 */
struct ChallengeGateAllowance final {
    GateKind kind;
    int maxCount;
};

/**
 * @brief One input combination and the expected target value.
 */
struct ChallengeTestCase final {
    QHash<QString, SignalValue> sourceValues;
    SignalValue expectedTargetValue;
};

/**
 * @brief Full definition of a single challenge puzzle.
 */
struct ChallengeLevel final {
    int index{0};
    QString title;
    QString description;
    QVector<ChallengeSourceSpec> sources;
    QPointF targetPosition;
    QVector<ChallengeGateAllowance> gateAllowances;
    QVector<ChallengeTestCase> testCases;
};

} // namespace digitallogic
