/**
 * @file XorGate.cpp
 * @brief Implements exclusive-OR (XOR) gate evaluation.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/gates/XorGate.h"

namespace digitallogic {

XorGate::XorGate(const ComponentId id)
    : Gate(id, GateKind::Xor, 2) // Exactly two inputs for XOR
{
}

SignalValue XorGate::evaluate(const QVector<SignalValue>& inputs) const
{
    if (inputs.size() < 2) {
        return SignalValue::Unknown;
    }

    // Either unknown input makes the XOR result indeterminate.
    if (inputs.at(0) == SignalValue::Unknown || inputs.at(1) == SignalValue::Unknown) {
        return SignalValue::Unknown;
    }

    // True when the two bits differ (exclusive or).
    return inputs.at(0) != inputs.at(1) ? SignalValue::True : SignalValue::False;
}

} // namespace digitallogic
