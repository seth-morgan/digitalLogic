#include "digitallogic/model/SimulationEngine.h"

#include <QTest>

// Unit tests for SimulationEngine static gate evaluators (truth tables, not circuit wiring).

using namespace digitallogic;

class GateTests final : public QObject {
    Q_OBJECT

private slots:
    void andGate_allTrue_returnsTrue();
    void andGate_anyFalse_returnsFalse();
    void orGate_anyTrue_returnsTrue();
    void orGate_allFalse_returnsFalse();
    void notGate_invertsValue();
    void nandGate_allTrue_returnsFalse();
    void nandGate_anyFalse_returnsTrue();
    void xorGate_inputsDiffer_returnsTrue();
    void xorGate_inputsMatch_returnsFalse();
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

void GateTests::nandGate_allTrue_returnsFalse()
{
    QCOMPARE(SimulationEngine::evaluateNand({SignalValue::True, SignalValue::True}), SignalValue::False);
}

void GateTests::nandGate_anyFalse_returnsTrue()
{
    QCOMPARE(SimulationEngine::evaluateNand({SignalValue::True, SignalValue::False}), SignalValue::True);
}

void GateTests::xorGate_inputsDiffer_returnsTrue()
{
    QCOMPARE(SimulationEngine::evaluateXor({SignalValue::True, SignalValue::False}), SignalValue::True);
}

void GateTests::xorGate_inputsMatch_returnsFalse()
{
    QCOMPARE(SimulationEngine::evaluateXor({SignalValue::True, SignalValue::True}), SignalValue::False);
}

QTEST_APPLESS_MAIN(GateTests)
#include "GateTests.moc"
