/**
 * @file AndGate.h
 * @brief Declares the two-input AND gate used by the digital logic simulator.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/Gate.h"
#include "digitallogic/model/ComponentId.h"

namespace digitallogic {

/**
 * @brief Evaluates an AND gate: output is true only when all inputs are true.
 */
class AndGate final : public Gate {
public:
    /**
     * @brief Constructs a two-input AND gate with the given component id.
     */
    explicit AndGate(ComponentId id);

    /**
     * @brief Returns True only if every input is True; False or Unknown otherwise.
     */
    [[nodiscard]] SignalValue evaluate(const QVector<SignalValue>& inputs) const override;
};

} // namespace digitallogic
