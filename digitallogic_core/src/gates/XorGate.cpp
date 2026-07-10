#include "digitallogic/gates/XorGate.h"

namespace digitallogic {

XorGate::XorGate(const ComponentId id)
    : Gate(id, GateKind::Xor, 2)
{
}

SignalValue XorGate::evaluate(const QVector<SignalValue>& inputs) const
{
    if (inputs.size() < 2) {
        return SignalValue::Unknown;
    }

    if (inputs.at(0) == SignalValue::Unknown || inputs.at(1) == SignalValue::Unknown) {
        return SignalValue::Unknown;
    }

    return inputs.at(0) != inputs.at(1) ? SignalValue::True : SignalValue::False;
}

} // namespace digitallogic
