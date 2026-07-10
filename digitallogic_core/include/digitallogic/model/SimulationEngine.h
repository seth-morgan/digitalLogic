#pragma once

#include "digitallogic/model/Circuit.h"
#include "digitallogic/model/PinId.h"
#include "digitallogic/model/SignalValue.h"

#include <QHash>
#include <QString>
#include <optional>

namespace digitallogic {

/**
 * @brief Result of a single simulation step for one pin.
 */
struct PinSignal final {
    PinId pin;
    SignalValue value{SignalValue::Unknown};
};

/**
 * @brief Evaluates a circuit by propagating signal values from sources through gates.
 */
class SimulationEngine final {
public:
    /**
     * @brief Runs simulation over the given circuit.
     * @param circuit Circuit topology and source values.
     * @param sourceOverrides Optional per-source values used instead of stored source values.
     * @return Resolved pin values when successful.
     */
    [[nodiscard]] static std::optional<QVector<PinSignal>> run(
        const Circuit& circuit, const QHash<ComponentId, SignalValue>* sourceOverrides = nullptr);

    /**
     * @brief Static gate helpers exposed for unit testing.
     */
    [[nodiscard]] static SignalValue evaluateAnd(const QVector<SignalValue>& inputs);
    [[nodiscard]] static SignalValue evaluateOr(const QVector<SignalValue>& inputs);
    [[nodiscard]] static SignalValue evaluateNot(const SignalValue input);
    [[nodiscard]] static SignalValue evaluateNand(const QVector<SignalValue>& inputs);
    [[nodiscard]] static SignalValue evaluateXor(const QVector<SignalValue>& inputs);
};

} // namespace digitallogic
