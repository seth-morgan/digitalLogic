/**
 * @file NandGate.h
 * @brief Declares the two-input NAND gate (AND followed by NOT).
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/Gate.h"
#include "digitallogic/model/ComponentId.h"

namespace digitallogic {

/**
 * @brief Evaluates a NAND gate: output is false only when all inputs are true.
 */
class NandGate final : public Gate {
public:
    explicit NandGate(ComponentId id);

    // Returns False only if every input is True; True when any input is False.
    [[nodiscard]] SignalValue evaluate(const QVector<SignalValue>& inputs) const override;
};

} // namespace digitallogic
