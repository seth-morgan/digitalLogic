#pragma once

#include "digitallogic/model/Gate.h"
#include "digitallogic/model/ComponentId.h"

namespace digitallogic {

/**
 * @brief Evaluates a two-input XOR gate: output is true when inputs differ.
 */
class XorGate final : public Gate {
public:
    explicit XorGate(ComponentId id);

    [[nodiscard]] SignalValue evaluate(const QVector<SignalValue>& inputs) const override;
};

} // namespace digitallogic
