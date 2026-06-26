#include "digitallogic/model/SimulationEngine.h"

#include <QTest>

using namespace digitallogic;

class GateTests final : public QObject {
    Q_OBJECT

private slots:
    void andGate_allTrue_returnsTrue();
    void andGate_anyFalse_returnsFalse();
    void orGate_anyTrue_returnsTrue();
    void orGate_allFalse_returnsFalse();
    void notGate_invertsValue();
};

void GateTests::andGate_allTrue_returnsTrue()
{
    QCOMPARE(SimulationEngine::evaluateAnd({SignalValue::True, SignalValue::True}), SignalValue::True);
}

void GateTests::andGate_anyFalse_returnsFalse()
{
    QCOMPARE(SimulationEngine::evaluateAnd({SignalValue::True, SignalValue::False}), SignalValue::False);
}

void GateTests::orGate_anyTrue_returnsTrue()
{
    QCOMPARE(SimulationEngine::evaluateOr({SignalValue::False, SignalValue::True}), SignalValue::True);
}

void GateTests::orGate_allFalse_returnsFalse()
{
    QCOMPARE(SimulationEngine::evaluateOr({SignalValue::False, SignalValue::False}), SignalValue::False);
}

void GateTests::notGate_invertsValue()
{
    QCOMPARE(SimulationEngine::evaluateNot(SignalValue::True), SignalValue::False);
    QCOMPARE(SimulationEngine::evaluateNot(SignalValue::False), SignalValue::True);
}

QTEST_APPLESS_MAIN(GateTests)
#include "GateTests.moc"
