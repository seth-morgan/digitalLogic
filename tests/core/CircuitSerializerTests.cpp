#include "digitallogic/model/Circuit.h"
#include "digitallogic/model/CircuitSerializer.h"
#include "digitallogic/model/PinIndices.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QTest>

using namespace digitallogic;

class CircuitSerializerTests final : public QObject {
    Q_OBJECT

private slots:
    void roundTrip_preservesCircuitTopology();
    void fromJson_rejectsInvalidWire();
};

void CircuitSerializerTests::roundTrip_preservesCircuitTopology()
{
    Circuit circuit;
    const ComponentId sourceA = circuit.addSource(QPointF(10.0, 20.0), SignalValue::True);
    const ComponentId sourceB = circuit.addSource(QPointF(10.0, 80.0), SignalValue::False);
    const ComponentId andGate = circuit.addGate(GateKind::And, QPointF(200.0, 40.0)).value();

    const PinId sourceAOut{sourceA, sourceOutputPinIndex()};
    const PinId sourceBOut{sourceB, sourceOutputPinIndex()};
    const PinId andIn0{andGate, gateInputPinIndex(0)};
    const PinId andIn1{andGate, gateInputPinIndex(1)};
    QVERIFY(circuit.addWire(sourceAOut, andIn0));
    QVERIFY(circuit.addWire(sourceBOut, andIn1));

    QString error;
    const QJsonObject json = CircuitSerializer::toJson(circuit);
    const std::optional<Circuit> loaded = CircuitSerializer::fromJson(json, error);
    QVERIFY2(loaded.has_value(), qPrintable(error));

    QCOMPARE(loaded->sources().size(), 2);
    QCOMPARE(static_cast<int>(loaded->gates().size()), 1);
    QCOMPARE(loaded->wires().size(), 2);
    QCOMPARE(loaded->findSource(sourceA)->value(), SignalValue::True);
}

void CircuitSerializerTests::fromJson_rejectsInvalidWire()
{
    Circuit circuit;
    const ComponentId source = circuit.addSource(QPointF(0.0, 0.0), SignalValue::False);
    const ComponentId gate = circuit.addGate(GateKind::Not, QPointF(100.0, 0.0)).value();

    QJsonObject json = CircuitSerializer::toJson(circuit);
    QJsonArray wires;
    QJsonObject badWire;
    QJsonObject from;
    from.insert(QStringLiteral("componentId"), static_cast<qint64>(gate.value));
    from.insert(QStringLiteral("pinIndex"), gateOutputPinIndex(1));
    QJsonObject to;
    to.insert(QStringLiteral("componentId"), static_cast<qint64>(gate.value));
    to.insert(QStringLiteral("pinIndex"), gateInputPinIndex(0));
    badWire.insert(QStringLiteral("from"), from);
    badWire.insert(QStringLiteral("to"), to);
    wires.append(badWire);
    json.insert(QStringLiteral("wires"), wires);

    QString error;
    const std::optional<Circuit> loaded = CircuitSerializer::fromJson(json, error);
    QVERIFY(!loaded.has_value());
    QVERIFY(!error.isEmpty());
}

QTEST_APPLESS_MAIN(CircuitSerializerTests)
#include "CircuitSerializerTests.moc"
