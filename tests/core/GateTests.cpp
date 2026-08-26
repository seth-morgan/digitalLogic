/**
 * @file GateTests.cpp
 * @brief Unit tests for SimulationEngine gate evaluation helpers.
 * @author Seth Morgan
 * @date 2026-08-25
 */
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

/**
 * @brief Verifies AND returns True when every input is True.
 */
void GateTests::andGate_allTrue_returnsTrue()
{
    QCOMPARE(SimulationEngine::evaluateAnd({SignalValue::True, SignalValue::True}), SignalValue::True);
}

/**
 * @brief Verifies AND returns False when any input is False.
 */
void GateTests::andGate_anyFalse_returnsFalse()
{
    QCOMPARE(SimulationEngine::evaluateAnd({SignalValue::True, SignalValue::False}), SignalValue::False);
}

/**
 * @brief Verifies OR returns True when any input is True.
 */
void GateTests::orGate_anyTrue_returnsTrue()
{
    QCOMPARE(SimulationEngine::evaluateOr({SignalValue::False, SignalValue::True}), SignalValue::True);
}

/**
 * @brief Verifies OR returns False when every input is False.
 */
void GateTests::orGate_allFalse_returnsFalse()
{
    QCOMPARE(SimulationEngine::evaluateOr({SignalValue::False, SignalValue::False}), SignalValue::False);
}

/**
 * @brief Verifies NOT inverts both True and False inputs.
 */
void GateTests::notGate_invertsValue()
{
    QCOMPARE(SimulationEngine::evaluateNot(SignalValue::True), SignalValue::False);
    QCOMPARE(SimulationEngine::evaluateNot(SignalValue::False), SignalValue::True);
}

/**
 * @brief Verifies NAND returns False when every input is True.
 */
void GateTests::nandGate_allTrue_returnsFalse()
{
    QCOMPARE(SimulationEngine::evaluateNand({SignalValue::True, SignalValue::True}), SignalValue::False);
}

/**
 * @brief Verifies NAND returns True when any input is False.
 */
void GateTests::nandGate_anyFalse_returnsTrue()
{
    QCOMPARE(SimulationEngine::evaluateNand({SignalValue::True, SignalValue::False}), SignalValue::True);
}

/**
 * @brief Verifies XOR returns True when the two inputs differ.
 */
void GateTests::xorGate_inputsDiffer_returnsTrue()
{
    QCOMPARE(SimulationEngine::evaluateXor({SignalValue::True, SignalValue::False}), SignalValue::True);
}

/**
 * @brief Verifies XOR returns False when both inputs match.
 */
void GateTests::xorGate_inputsMatch_returnsFalse()
{
    QCOMPARE(SimulationEngine::evaluateXor({SignalValue::True, SignalValue::True}), SignalValue::False);
}

QTEST_APPLESS_MAIN(GateTests)
#include "GateTests.moc"
