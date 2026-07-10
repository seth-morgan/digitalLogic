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

void ChallengeValidatorTests::andLevelPassesWithCorrectWiring()
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

    const std::optional<ComponentId> andGateId = circuit.addGate(GateKind::And, QPointF(400.0, 180.0));
    QVERIFY(andGateId.has_value());

    const PinId sourceA{sourceIds.value(QStringLiteral("A")), sourceOutputPinIndex()};
    const PinId sourceB{sourceIds.value(QStringLiteral("B")), sourceOutputPinIndex()};
    const PinId andOut{andGateId.value(), gateOutputPinIndex(2)};
    const PinId andIn0{andGateId.value(), 0};
    const PinId andIn1{andGateId.value(), 1};
    const PinId targetIn{targetId, targetInputPinIndex()};

    QVERIFY(circuit.addWire(sourceA, andIn0));
    QVERIFY(circuit.addWire(sourceB, andIn1));
    QVERIFY(circuit.addWire(andOut, targetIn));

    const ChallengeValidationResult result = ChallengeValidator::validate(circuit, *level, sourceIds, targetId);
    QVERIFY(result.success);
}

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
    (void)circuit.addGate(GateKind::And, QPointF(400.0, 180.0));

    const ChallengeValidationResult result = ChallengeValidator::validate(circuit, *level, sourceIds, targetId);
    QVERIFY(!result.success);
}

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
    (void)circuit.addGate(GateKind::Or, QPointF(400.0, 180.0));

    const ChallengeValidationResult result = ChallengeValidator::validate(circuit, *level, sourceIds, targetId);
    QVERIFY(!result.success);
}

QTEST_MAIN(ChallengeValidatorTests)
#include "ChallengeValidatorTests.moc"
