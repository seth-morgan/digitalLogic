#include "digitallogic/gates/NotGate.h"

namespace digitallogic {

NotGate::NotGate(const ComponentId id)
    : Gate(id, GateKind::Not, 1)
{
}

SignalValue NotGate::evaluate(const QVector<SignalValue>& inputs) const
{
    if (inputs.isEmpty()) {
        return SignalValue::Unknown;
    }

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
