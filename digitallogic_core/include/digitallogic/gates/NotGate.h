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

    [[nodiscard]] SignalValue evaluate(const QVector<SignalValue>& inputs) const override;
};

} // namespace digitallogic
