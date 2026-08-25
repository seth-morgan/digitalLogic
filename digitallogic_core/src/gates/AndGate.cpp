/**
 * @file AndGate.cpp
 * @brief Implements AND-gate evaluation for the digital logic simulator.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/gates/AndGate.h"

namespace digitallogic {

/**
 * @brief Constructs a two-input AND gate with the given component id.
 */
AndGate::AndGate(const ComponentId id)
    : Gate(id, GateKind::And, 2) // Two-input AND by default
{
}

/**
 * @brief Returns True only if every input is True; False or Unknown otherwise.
 */
SignalValue AndGate::evaluate(const QVector<SignalValue>& inputs) const
{
    // Need at least two resolved inputs to produce a known AND result.
    if (inputs.size() < 2) {
        return SignalValue::Unknown;
    }

    for (const SignalValue input : inputs) {
        // Any false input forces the AND output low (short-circuit).
        if (input == SignalValue::False) {
            return SignalValue::False;
        }
        // Unknown inputs make the whole result indeterminate.
        if (input == SignalValue::Unknown) {
            return SignalValue::Unknown;
        }
    }

    // All inputs were True.
    return SignalValue::True;
}

} // namespace digitallogic
