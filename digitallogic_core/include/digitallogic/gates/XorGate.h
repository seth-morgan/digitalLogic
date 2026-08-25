/**
 * @file XorGate.h
 * @brief Declares the two-input exclusive-OR (XOR) gate.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/Gate.h"
#include "digitallogic/model/ComponentId.h"

namespace digitallogic {

/**
 * @brief Evaluates a two-input XOR gate: output is true when inputs differ.
 */
class XorGate final : public Gate {
public:
    /**
     * @brief Constructs a two-input XOR gate with the given component id.
     */
    explicit XorGate(ComponentId id);

    /**
     * @brief Returns True when the two inputs differ; False when they match.
     */
    [[nodiscard]] SignalValue evaluate(const QVector<SignalValue>& inputs) const override;
};

} // namespace digitallogic
