#include "digitallogic/model/Circuit.h"
#include "digitallogic/model/SimulationEngine.h"

#include <QTest>

using namespace digitallogic;

class SimulationEngineTests final : public QObject {
    Q_OBJECT

private slots:
    void simpleAndCircuit_propagatesSourceValues();
    void notGate_invertsConnectedSource();
};

void SimulationEngineTests::simpleAndCircuit_propagatesSourceValues()
{
    Circuit circuit;
    const ComponentId sourceA = circuit.addSource(QPointF(0.0, 0.0), SignalValue::True);
    const ComponentId sourceB = circuit.addSource(QPointF(0.0, 0.0), SignalValue::False);
    const std::optional<ComponentId> andGateId = circuit.addGate(GateKind::And, QPointF(100.0, 0.0));
    QVERIFY(andGateId.has_value());

    const PinId sourceAOut{sourceA, 0};
    const PinId sourceBOut{sourceB, 0};
    const PinId andInput0{andGateId.value(), 0};
    const PinId andInput1{andGateId.value(), 1};
    const PinId andOutput{andGateId.value(), 0};

    QVERIFY(circuit.addWire(sourceAOut, andInput0));
    QVERIFY(circuit.addWire(sourceBOut, andInput1));

    const std::optional<QVector<PinSignal>> results = SimulationEngine::run(circuit);
    QVERIFY(results.has_value());

    QHash<PinId, SignalValue> values;
    for (const PinSignal& pinSignal : results.value()) {
        values.insert(pinSignal.pin, pinSignal.value);
    }

    QCOMPARE(values.value(sourceAOut), SignalValue::True);
    QCOMPARE(values.value(sourceBOut), SignalValue::False);
    QCOMPARE(values.value(andOutput), SignalValue::False);
}

void SimulationEngineTests::notGate_invertsConnectedSource()
{
    Circuit circuit;
    const ComponentId source = circuit.addSource(QPointF(0.0, 0.0), SignalValue::True);
    const std::optional<ComponentId> notGateId = circuit.addGate(GateKind::Not, QPointF(100.0, 0.0));
    QVERIFY(notGateId.has_value());

    const PinId sourceOut{source, 0};
    const PinId notInput{notGateId.value(), 0};
    const PinId notOutput{notGateId.value(), 0};

    QVERIFY(circuit.addWire(sourceOut, notInput));

    const std::optional<QVector<PinSignal>> results = SimulationEngine::run(circuit);
    QVERIFY(results.has_value());

    QHash<PinId, SignalValue> values;
    for (const PinSignal& pinSignal : results.value()) {
        values.insert(pinSignal.pin, pinSignal.value);
    }

    QCOMPARE(values.value(notOutput), SignalValue::False);
}

QTEST_APPLESS_MAIN(SimulationEngineTests)
#include "SimulationEngineTests.moc"
