/**
 * @file ChallengeValidatorTests.cpp
 * @brief Unit tests for ChallengeValidator against catalog level 0 (AND).
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/challenge/ChallengeCatalog.h"
#include "digitallogic/challenge/ChallengeValidator.h"
#include "digitallogic/model/Circuit.h"
#include "digitallogic/model/PinIndices.h"

#include <QtTest>

using namespace digitallogic;

class ChallengeValidatorTests final : public QObject {
    Q_OBJECT

private slots:
    void andLevelPassesWithCorrectWiring();
    void rejectsMissingTargetWire();
    void rejectsWrongGateCount();
};

/**
 * @brief Builds a correctly wired AND solution for level 0 and expects success.
 */
void ChallengeValidatorTests::andLevelPassesWithCorrectWiring()
{
    const ChallengeLevel* level = ChallengeCatalog::levelAt(0);
    QVERIFY(level != nullptr);

    Circuit circuit;
    QHash<QString, ComponentId> sourceIds;
    // Place sources using the catalog labels so the validator can map A/B.
    for (const ChallengeSourceSpec& sourceSpec : level->sources) {
        const ComponentId sourceId = circuit.addSource(sourceSpec.position, SignalValue::False);
        sourceIds.insert(sourceSpec.label, sourceId);
    }
    const ComponentId targetId = circuit.addTarget(level->targetPosition);

    const std::optional<ComponentId> andGateId = circuit.addGate(GateKind::And, QPointF(400.0, 180.0));
    QVERIFY(andGateId.has_value());

    const PinId sourceA{sourceIds.value(QStringLiteral("A")), sourceOutputPinIndex()};
    const PinId sourceB{sourceIds.value(QStringLiteral("B")), sourceOutputPinIndex()};
    const PinId andOut{andGateId.value(), gateOutputPinIndex(2)};
    // Raw pin indices 0/1 are the AND gate inputs (same as gateInputPinIndex).
    const PinId andIn0{andGateId.value(), 0};
    const PinId andIn1{andGateId.value(), 1};
    const PinId targetIn{targetId, targetInputPinIndex()};

    QVERIFY(circuit.addWire(sourceA, andIn0));
    QVERIFY(circuit.addWire(sourceB, andIn1));
    QVERIFY(circuit.addWire(andOut, targetIn));

    const ChallengeValidationResult result = ChallengeValidator::validate(circuit, *level, sourceIds, targetId);
    QVERIFY(result.success);
}

/**
 * @brief Expects failure when the AND output is never wired to the target.
 */
void ChallengeValidatorTests::rejectsMissingTargetWire()
{
    const ChallengeLevel* level = ChallengeCatalog::levelAt(0);
    QVERIFY(level != nullptr);

    Circuit circuit;
    QHash<QString, ComponentId> sourceIds;
    for (const ChallengeSourceSpec& sourceSpec : level->sources) {
        const ComponentId sourceId = circuit.addSource(sourceSpec.position, SignalValue::False);
        sourceIds.insert(sourceSpec.label, sourceId);
    }
    const ComponentId targetId = circuit.addTarget(level->targetPosition);
    // Gate is present but no wires connect it to sources or the target.
    (void)circuit.addGate(GateKind::And, QPointF(400.0, 180.0));

    const ChallengeValidationResult result = ChallengeValidator::validate(circuit, *level, sourceIds, targetId);
    QVERIFY(!result.success);
}

/**
 * @brief Expects failure when the placed gate kind does not match the level.
 */
void ChallengeValidatorTests::rejectsWrongGateCount()
{
    const ChallengeLevel* level = ChallengeCatalog::levelAt(0);
    QVERIFY(level != nullptr);

    Circuit circuit;
    QHash<QString, ComponentId> sourceIds;
    for (const ChallengeSourceSpec& sourceSpec : level->sources) {
        const ComponentId sourceId = circuit.addSource(sourceSpec.position, SignalValue::False);
        sourceIds.insert(sourceSpec.label, sourceId);
    }
    const ComponentId targetId = circuit.addTarget(level->targetPosition);
    // Level 0 expects an AND; an OR alone fails the required-gate check.
    (void)circuit.addGate(GateKind::Or, QPointF(400.0, 180.0));

    const ChallengeValidationResult result = ChallengeValidator::validate(circuit, *level, sourceIds, targetId);
    QVERIFY(!result.success);
}

QTEST_MAIN(ChallengeValidatorTests)
#include "ChallengeValidatorTests.moc"
