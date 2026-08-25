/**
 * @file OrGate.h
 * @brief Declares the multi-input OR gate used by the digital logic simulator.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/Gate.h"
#include "digitallogic/model/ComponentId.h"

namespace digitallogic {

/**
 * @brief Evaluates an OR gate: output is true when any input is true.
 */
class OrGate final : public Gate {
public:
    /**
     * @brief Constructs a two-input OR gate with the given component id.
     */
    explicit OrGate(ComponentId id);

    /**
     * @brief Returns True if any input is True; False only when every input is False.
     */
    [[nodiscard]] SignalValue evaluate(const QVector<SignalValue>& inputs) const override;
};

} // namespace digitallogic
