#include "digitallogic/gates/AndGate.h"

namespace digitallogic {

AndGate::AndGate(const ComponentId id)
    : Gate(id, GateKind::And, 2)
{
}

SignalValue AndGate::evaluate(const QVector<SignalValue>& inputs) const
{
    if (inputs.size() < 2) {
        return SignalValue::Unknown;
    }

    for (const SignalValue input : inputs) {
        if (input == SignalValue::False) {
            return SignalValue::False;
        }
        if (input == SignalValue::Unknown) {
            return SignalValue::Unknown;
        }
    }

    return SignalValue::True;
}

} // namespace digitallogic
