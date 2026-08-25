/**
 * @file NotGate.h
 * @brief Declares the single-input NOT (inverter) gate.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/Gate.h"
#include "digitallogic/model/ComponentId.h"

namespace digitallogic {

/**
 * @brief Evaluates a NOT gate: inverts a single input.
 */
class NotGate final : public Gate {
public:
    explicit NotGate(ComponentId id);

    // Inverts the first input; returns Unknown when the input list is empty.
    [[nodiscard]] SignalValue evaluate(const QVector<SignalValue>& inputs) const override;
};

} // namespace digitallogic
