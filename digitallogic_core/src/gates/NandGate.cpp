#include "digitallogic/gates/NandGate.h"

namespace digitallogic {

NandGate::NandGate(const ComponentId id)
    : Gate(id, GateKind::Nand, 2)
{
}

SignalValue NandGate::evaluate(const QVector<SignalValue>& inputs) const
{
    if (inputs.size() < 2) {
        return SignalValue::Unknown;
    }

    for (const SignalValue input : inputs) {
        if (input == SignalValue::Unknown) {
            return SignalValue::Unknown;
        }
        if (input == SignalValue::False) {
            return SignalValue::True;
        }
    }

    return SignalValue::False;
}

} // namespace digitallogic
