#pragma once

#include "digitallogic/model/ComponentId.h"
#include "digitallogic/model/SignalValue.h"

#include <QVector>

namespace digitallogic {

/**
 * @brief Supported logic gate kinds in the baseline simulator.
 */
enum class GateKind {
    And,
    Or,
    Not,
    Nand,
    Xor
};

/**
 * @brief Abstract logic gate with input and output pins.
 */
class Gate {
public:
    virtual ~Gate() = default;

    [[nodiscard]] ComponentId id() const noexcept { return m_id; }
    [[nodiscard]] GateKind kind() const noexcept { return m_kind; }
    [[nodiscard]] int inputCount() const noexcept { return m_inputCount; }

    /**
     * @brief Computes the gate output from resolved input values.
     * @param inputs Input values in pin order; size must equal inputCount().
     * @return Output signal value.
     */
    [[nodiscard]] virtual SignalValue evaluate(const QVector<SignalValue>& inputs) const = 0;

protected:
    Gate(ComponentId id, GateKind kind, int inputCount);

private:
    ComponentId m_id;
    GateKind m_kind;
    int m_inputCount;
};

} // namespace digitallogic
