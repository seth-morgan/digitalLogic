#include "digitallogic/challenge/ChallengeCatalog.h"
#include "digitallogic/challenge/ChallengeValidator.h"
#include "digitallogic/model/Circuit.h"
#include "digitallogic/model/PinIndices.h"

#include <QtTest>

using namespace digitallogic;

namespace {

// Level 1 in the catalog ("AND Gate"); keep in sync with ChallengeCatalog ordering.
constexpr int kAndLevelIndex = 0;

[[nodiscard]] const ChallengeLevel* andLevel()
{
    return ChallengeCatalog::levelAt(kAndLevelIndex);
}

void buildAndLevelCircuit(Circuit& circuit, QHash<QString, ComponentId>& sourceIds, ComponentId& targetId)
{
    const ChallengeLevel* level = andLevel();
    QVERIFY(level != nullptr);

    sourceIds.clear();
    for (const ChallengeSourceSpec& sourceSpec : level->sources) {
        const ComponentId sourceId = circuit.addSource(sourceSpec.position, SignalValue::False);
        sourceIds.insert(sourceSpec.label, sourceId);
    }
    targetId = circuit.addTarget(level->targetPosition);
}

} // namespace

class ChallengeValidatorTests final : public QObject {
    Q_OBJECT

private slots:
    void andLevelPassesWithCorrectWiring();
    void rejectsMissingTargetWire();
    void rejectsWrongGateCount();
};

void ChallengeValidatorTests::andLevelPassesWithCorrectWiring()
{
    const ChallengeLevel* level = andLevel();
    QVERIFY(level != nullptr);

    Circuit circuit;
    QHash<QString, ComponentId> sourceIds;
    ComponentId targetId{};
    buildAndLevelCircuit(circuit, sourceIds, targetId);

    const std::optional<ComponentId> andGateId = circuit.addGate(GateKind::And, QPointF(400.0, 180.0));
    QVERIFY(andGateId.has_value());

    const PinId sourceA{sourceIds.value(QStringLiteral("A")), sourceOutputPinIndex()};
    const PinId sourceB{sourceIds.value(QStringLiteral("B")), sourceOutputPinIndex()};
    const PinId andOut{andGateId.value(), gateOutputPinIndex(2)};
    const PinId andIn0{andGateId.value(), gateInputPinIndex(0)};
    const PinId andIn1{andGateId.value(), gateInputPinIndex(1)};
    const PinId targetIn{targetId, targetInputPinIndex()};

    QVERIFY(circuit.addWire(sourceA, andIn0));
    QVERIFY(circuit.addWire(sourceB, andIn1));
    QVERIFY(circuit.addWire(andOut, targetIn));

    const ChallengeValidationResult result = ChallengeValidator::validate(circuit, *level, sourceIds, targetId);
    QVERIFY(result.success);
}

void ChallengeValidatorTests::rejectsMissingTargetWire()
{
    const ChallengeLevel* level = andLevel();
    QVERIFY(level != nullptr);

    Circuit circuit;
    QHash<QString, ComponentId> sourceIds;
    ComponentId targetId{};
    buildAndLevelCircuit(circuit, sourceIds, targetId);
    // Gate placed but not wired to the challenge OUT target.
    (void)circuit.addGate(GateKind::And, QPointF(400.0, 180.0));

    const ChallengeValidationResult result = ChallengeValidator::validate(circuit, *level, sourceIds, targetId);
    QVERIFY(!result.success);
    QCOMPARE(result.message, QStringLiteral("Connect your circuit to the output target."));
}

void ChallengeValidatorTests::rejectsWrongGateCount()
{
    const ChallengeLevel* level = andLevel();
    QVERIFY(level != nullptr);

    Circuit circuit;
    QHash<QString, ComponentId> sourceIds;
    ComponentId targetId{};
    buildAndLevelCircuit(circuit, sourceIds, targetId);
    // Wrong gate kind for the AND level budget.
    (void)circuit.addGate(GateKind::Or, QPointF(400.0, 180.0));

    const ChallengeValidationResult result = ChallengeValidator::validate(circuit, *level, sourceIds, targetId);
    QVERIFY(!result.success);
    QCOMPARE(result.message, QStringLiteral("Only the allowed gate types may be used for this level."));
}

QTEST_MAIN(ChallengeValidatorTests)
#include "ChallengeValidatorTests.moc"
