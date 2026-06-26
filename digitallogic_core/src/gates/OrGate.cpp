#include "digitallogic/gates/OrGate.h"

namespace digitallogic {

OrGate::OrGate(const ComponentId id)
    : Gate(id, GateKind::Or, 2)
{
}

SignalValue OrGate::evaluate(const QVector<SignalValue>& inputs) const
{
    if (inputs.isEmpty()) {
        return SignalValue::Unknown;
    }

    for (const SignalValue input : inputs) {
        if (input == SignalValue::True) {
            return SignalValue::True;
        }
        if (input == SignalValue::Unknown) {
            return SignalValue::Unknown;
        }
    }

    return SignalValue::False;
}

} // namespace digitallogic
