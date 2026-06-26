#pragma once

#include "digitallogic/model/Gate.h"
#include "digitallogic/model/ComponentId.h"

namespace digitallogic {

/**
 * @brief Evaluates an OR gate: output is true when any input is true.
 */
class OrGate final : public Gate {
public:
    explicit OrGate(ComponentId id);

    [[nodiscard]] SignalValue evaluate(const QVector<SignalValue>& inputs) const override;
};

} // namespace digitallogic
