#pragma once

#include "digitallogic/model/Gate.h"
#include "digitallogic/model/ComponentId.h"

namespace digitallogic {

/**
 * @brief Evaluates an AND gate: output is true only when all inputs are true.
 */
class AndGate final : public Gate {
public:
    explicit AndGate(ComponentId id);

    [[nodiscard]] SignalValue evaluate(const QVector<SignalValue>& inputs) const override;
};

} // namespace digitallogic
