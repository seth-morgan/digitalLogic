/**
 * @file NotGate.cpp
 * @brief Implements NOT-gate (inverter) evaluation.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/gates/NotGate.h"

namespace digitallogic {

/**
 * @brief Constructs a single-input NOT gate with the given component id.
 */
NotGate::NotGate(const ComponentId id)
    : Gate(id, GateKind::Not, 1) // Single-input inverter
{
}

/**
 * @brief Inverts the first input; returns Unknown when the input list is empty.
 */
SignalValue NotGate::evaluate(const QVector<SignalValue>& inputs) const
{
    if (inputs.isEmpty()) {
        return SignalValue::Unknown;
    }

    // Invert the first (and only) pin value.
    switch (inputs.front()) {
    case SignalValue::True:
        return SignalValue::False;
    case SignalValue::False:
        return SignalValue::True;
    case SignalValue::Unknown:
    default:
        return SignalValue::Unknown;
    }
}

} // namespace digitallogic
