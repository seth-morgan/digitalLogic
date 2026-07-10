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

    [[nodiscard]] SignalValue evaluate(const QVector<SignalValue>& inputs) const override;
};

} // namespace digitallogic
