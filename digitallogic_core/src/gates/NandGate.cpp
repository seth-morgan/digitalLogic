/**
 * @file NandGate.cpp
 * @brief Implements NAND-gate evaluation for the digital logic simulator.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/gates/NandGate.h"

namespace digitallogic {

/**
 * @brief Constructs a two-input NAND gate with the given component id.
 */
NandGate::NandGate(const ComponentId id)
    : Gate(id, GateKind::Nand, 2) // Two-input NAND by default
{
}

/**
 * @brief Returns False only if every input is True; True when any input is False.
 */
SignalValue NandGate::evaluate(const QVector<SignalValue>& inputs) const
{
    if (inputs.size() < 2) {
        return SignalValue::Unknown;
    }

    for (const SignalValue input : inputs) {
        // Unknown must be checked first so it is not mistaken for False.
        if (input == SignalValue::Unknown) {
            return SignalValue::Unknown;
        }
        // Any false input makes NAND output true (De Morgan / inverted AND).
        if (input == SignalValue::False) {
            return SignalValue::True;
        }
    }

    // All inputs were True, so NAND yields False.
    return SignalValue::False;
}

} // namespace digitallogic
