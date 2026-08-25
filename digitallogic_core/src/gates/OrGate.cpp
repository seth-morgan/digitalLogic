/**
 * @file OrGate.cpp
 * @brief Implements OR-gate evaluation for the digital logic simulator.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/gates/OrGate.h"

namespace digitallogic {

OrGate::OrGate(const ComponentId id)
    : Gate(id, GateKind::Or, 2) // Two-input OR by default
{
}

SignalValue OrGate::evaluate(const QVector<SignalValue>& inputs) const
{
    if (inputs.isEmpty()) {
        return SignalValue::Unknown;
    }

    for (const SignalValue input : inputs) {
        // Any true input forces the OR output high (short-circuit).
        if (input == SignalValue::True) {
            return SignalValue::True;
        }
        // Unknown inputs make the whole result indeterminate.
        if (input == SignalValue::Unknown) {
            return SignalValue::Unknown;
        }
    }

    // All inputs were False.
    return SignalValue::False;
}

} // namespace digitallogic
