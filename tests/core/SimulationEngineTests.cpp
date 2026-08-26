/**
 * @file SimulationEngineTests.cpp
 * @brief Unit tests for end-to-end SimulationEngine circuit evaluation.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/model/Circuit.h"
#include "digitallogic/model/PinIndices.h"
#include "digitallogic/model/SimulationEngine.h"

#include <QHash>
#include <QTest>

using namespace digitallogic;

namespace {

QHash<PinId, SignalValue> pinSignalsToMap(const QVector<PinSignal>& results)
{
    QHash<PinId, SignalValue> values;
    for (const PinSignal& pinSignal : results) {
        values.insert(pinSignal.pin, pinSignal.value);
    }
    return values;
}

} // namespace

class SimulationEngineTests final : public QObject {
    Q_OBJECT

private slots:
    void simpleAndCircuit_propagatesSourceValues();
    void notGate_invertsConnectedSource();
};

/**
 * @brief Builds a two-input AND circuit and checks pin values after simulation.
 */
void SimulationEngineTests::simpleAndCircuit_propagatesSourceValues()
{
    Circuit circuit;
    const ComponentId sourceA = circuit.addSource(QPointF(0.0, 0.0), SignalValue::True);
    const ComponentId sourceB = circuit.addSource(QPointF(0.0, 0.0), SignalValue::False);
    const std::optional<ComponentId> andGateId = circuit.addGate(GateKind::And, QPointF(100.0, 0.0));
    QVERIFY(andGateId.has_value());

    const PinId sourceAOut{sourceA, sourceOutputPinIndex()};
    const PinId sourceBOut{sourceB, sourceOutputPinIndex()};
    const PinId andInput0{andGateId.value(), gateInputPinIndex(0)};
    const PinId andInput1{andGateId.value(), gateInputPinIndex(1)};
    // AND has two inputs; output pin index equals the input count (2).
    const PinId andOutput{andGateId.value(), gateOutputPinIndex(2)};

    QVERIFY(circuit.addWire(sourceAOut, andInput0));
    QVERIFY(circuit.addWire(sourceBOut, andInput1));

    const std::optional<QVector<PinSignal>> results = SimulationEngine::run(circuit);
    QVERIFY(results.has_value());

    const QHash<PinId, SignalValue> values = pinSignalsToMap(results.value());

    QCOMPARE(values.value(sourceAOut), SignalValue::True);
    QCOMPARE(values.value(sourceBOut), SignalValue::False);
    // True AND False => False at the gate output.
    QCOMPARE(values.value(andOutput), SignalValue::False);
}

/**
 * @brief Builds a NOT circuit and checks the inverted output after simulation.
 */
void SimulationEngineTests::notGate_invertsConnectedSource()
{
    Circuit circuit;
    const ComponentId source = circuit.addSource(QPointF(0.0, 0.0), SignalValue::True);
    const std::optional<ComponentId> notGateId = circuit.addGate(GateKind::Not, QPointF(100.0, 0.0));
    QVERIFY(notGateId.has_value());

    const PinId sourceOut{source, sourceOutputPinIndex()};
    const PinId notInput{notGateId.value(), gateInputPinIndex(0)};
    // NOT has one input; output pin index is 1.
    const PinId notOutput{notGateId.value(), gateOutputPinIndex(1)};

    QVERIFY(circuit.addWire(sourceOut, notInput));

    const std::optional<QVector<PinSignal>> results = SimulationEngine::run(circuit);
    QVERIFY(results.has_value());

    const QHash<PinId, SignalValue> values = pinSignalsToMap(results.value());

    QCOMPARE(values.value(notOutput), SignalValue::False);
}

QTEST_APPLESS_MAIN(SimulationEngineTests)
#include "SimulationEngineTests.moc"
