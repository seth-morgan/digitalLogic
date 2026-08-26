/**
 * @file CircuitTests.cpp
 * @brief Unit tests for Circuit wire validation and component removal.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/model/Circuit.h"
#include "digitallogic/model/PinIndices.h"
#include "digitallogic/model/WireValidation.h"

#include <QTest>

using namespace digitallogic;

class CircuitTests final : public QObject {
    Q_OBJECT

private slots:
    void validateWire_rejectsSameComponent();
    void validateWire_rejectsDuplicateInput();
    void removeGate_removesAttachedWires();
    void removeWire_removesOnlyMatchingWire();
};

/**
 * @brief Ensures a wire cannot connect two pins on the same component.
 */
void CircuitTests::validateWire_rejectsSameComponent()
{
    Circuit circuit;
    const std::optional<ComponentId> gateIdOpt = circuit.addGate(GateKind::And, QPointF(0.0, 0.0));
    QVERIFY(gateIdOpt.has_value());
    const ComponentId gateId = gateIdOpt.value();
    // Cannot wire a gate output back to an input on the same component.
    // Output pin index 2 is the AND gate's single output (two inputs occupy 0 and 1).
    const PinId output{gateId, gateOutputPinIndex(2)};
    const PinId input{gateId, gateInputPinIndex(0)};

    QCOMPARE(circuit.validateWire(output, input), WireValidationResult::SameComponent);
}

/**
 * @brief Ensures a second wire into an already-driven input is rejected.
 */
void CircuitTests::validateWire_rejectsDuplicateInput()
{
    Circuit circuit;
    const ComponentId source = circuit.addSource(QPointF(0.0, 0.0), SignalValue::True);
    const std::optional<ComponentId> gateIdOpt = circuit.addGate(GateKind::And, QPointF(100.0, 0.0));
    QVERIFY(gateIdOpt.has_value());
    const ComponentId gateId = gateIdOpt.value();

    const PinId sourceOut{source, sourceOutputPinIndex()};
    const PinId gateInput{gateId, gateInputPinIndex(0)};

    QVERIFY(circuit.addWire(sourceOut, gateInput));
    QCOMPARE(circuit.validateWire(sourceOut, gateInput), WireValidationResult::InputAlreadyConnected);
}

/**
 * @brief Ensures removing a gate also deletes wires attached to its pins.
 */
void CircuitTests::removeGate_removesAttachedWires()
{
    Circuit circuit;
    const ComponentId source = circuit.addSource(QPointF(0.0, 0.0), SignalValue::True);
    const std::optional<ComponentId> gateIdOpt = circuit.addGate(GateKind::And, QPointF(100.0, 0.0));
    QVERIFY(gateIdOpt.has_value());
    const ComponentId gateId = gateIdOpt.value();

    const PinId sourceOut{source, sourceOutputPinIndex()};
    const PinId gateInput{gateId, gateInputPinIndex(0)};
    QVERIFY(circuit.addWire(sourceOut, gateInput));
    QCOMPARE(circuit.wires().size(), 1);

    QVERIFY(circuit.removeGate(gateId));
    QCOMPARE(circuit.wires().size(), 0);
    QCOMPARE(static_cast<int>(circuit.gates().size()), 0);
}

/**
 * @brief Ensures removeWire deletes only the matching connection.
 */
void CircuitTests::removeWire_removesOnlyMatchingWire()
{
    Circuit circuit;
    const ComponentId sourceA = circuit.addSource(QPointF(0.0, 0.0), SignalValue::True);
    const ComponentId sourceB = circuit.addSource(QPointF(0.0, 0.0), SignalValue::False);
    const std::optional<ComponentId> gateIdOpt = circuit.addGate(GateKind::And, QPointF(100.0, 0.0));
    QVERIFY(gateIdOpt.has_value());
    const ComponentId gateId = gateIdOpt.value();

    const PinId sourceAOut{sourceA, sourceOutputPinIndex()};
    const PinId sourceBOut{sourceB, sourceOutputPinIndex()};
    const PinId input0{gateId, gateInputPinIndex(0)};
    const PinId input1{gateId, gateInputPinIndex(1)};

    QVERIFY(circuit.addWire(sourceAOut, input0));
    QVERIFY(circuit.addWire(sourceBOut, input1));
    QCOMPARE(circuit.wires().size(), 2);

    QVERIFY(circuit.removeWire(sourceAOut, input0));
    QCOMPARE(circuit.wires().size(), 1);
}

QTEST_APPLESS_MAIN(CircuitTests)
#include "CircuitTests.moc"
