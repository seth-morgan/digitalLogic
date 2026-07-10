#include "digitallogic/challenge/ChallengeValidator.h"

#include "digitallogic/model/PinIndices.h"
#include "digitallogic/model/SimulationEngine.h"

namespace digitallogic {

namespace {

[[nodiscard]] QHash<GateKind, int> countGatesByKind(const Circuit& circuit)
{
    QHash<GateKind, int> counts;
    for (const auto& gatePtr : circuit.gates()) {
        counts[gatePtr->kind()] += 1;
    }
    return counts;
}

[[nodiscard]] bool targetHasInputWire(const Circuit& circuit, const ComponentId targetId)
{
    const PinId targetInput{targetId, targetInputPinIndex()};
    for (const Wire& wire : circuit.wires()) {
        if (wire.to == targetInput) {
            return true;
        }
    }
    return false;
}

[[nodiscard]] SignalValue readTargetInputValue(const Circuit& circuit, const ComponentId targetId,
                                               const QHash<PinId, SignalValue>& resolved)
{
    const PinId targetInput{targetId, targetInputPinIndex()};
    const auto directIt = resolved.find(targetInput);
    if (directIt != resolved.end()) {
        return directIt.value();
    }

    for (const Wire& wire : circuit.wires()) {
        if (wire.to != targetInput) {
            continue;
        }
        const auto fromIt = resolved.find(wire.from);
        if (fromIt != resolved.end()) {
            return fromIt.value();
        }
    }

    return SignalValue::Unknown;
}

[[nodiscard]] QString formatSignal(const SignalValue value)
{
    return QString::fromUtf8(toString(value));
}

} // namespace

ChallengeValidationResult ChallengeValidator::validate(const Circuit& circuit, const ChallengeLevel& level,
                                                       const QHash<QString, ComponentId>& sourceIdsByLabel,
                                                       const ComponentId targetId)
{
    ChallengeValidationResult result;

    if (circuit.findTarget(targetId) == nullptr) {
        result.message = QStringLiteral("Challenge target is missing.");
        return result;
    }

    const QHash<GateKind, int> gateCounts = countGatesByKind(circuit);

    QHash<GateKind, int> allowedCounts;
    for (const ChallengeGateAllowance& allowance : level.gateAllowances) {
        allowedCounts.insert(allowance.kind, allowance.maxCount);
    }

    for (auto it = gateCounts.begin(); it != gateCounts.end(); ++it) {
        if (!allowedCounts.contains(it.key())) {
            result.message = QStringLiteral("Only the allowed gate types may be used for this level.");
            return result;
        }
    }

    for (const ChallengeGateAllowance& allowance : level.gateAllowances) {
        const int placed = gateCounts.value(allowance.kind, 0);
        if (placed > allowance.maxCount) {
            result.message = QStringLiteral("Too many gates placed for this level.");
            return result;
        }
        if (placed < allowance.maxCount) {
            result.message = QStringLiteral("Place all required gates before checking your solution.");
            return result;
        }
    }

    if (!targetHasInputWire(circuit, targetId)) {
        result.message = QStringLiteral("Connect your circuit to the output target.");
        return result;
    }

    for (const ChallengeSourceSpec& sourceSpec : level.sources) {
        if (!sourceIdsByLabel.contains(sourceSpec.label)) {
            result.message = QStringLiteral("Challenge sources are misconfigured.");
            return result;
        }
    }

    for (const ChallengeTestCase& testCase : level.testCases) {
        QHash<ComponentId, SignalValue> overrides;
        for (auto it = testCase.sourceValues.begin(); it != testCase.sourceValues.end(); ++it) {
            const ComponentId sourceId = sourceIdsByLabel.value(it.key());
            overrides.insert(sourceId, it.value());
        }

        const std::optional<QVector<PinSignal>> simulation = SimulationEngine::run(circuit, &overrides);
        if (!simulation.has_value()) {
            result.message = QStringLiteral("Circuit has a cycle or cannot be simulated.");
            return result;
        }

        QHash<PinId, SignalValue> resolved;
        for (const PinSignal& pinSignal : simulation.value()) {
            resolved.insert(pinSignal.pin, pinSignal.value);
        }

        const SignalValue actual = readTargetInputValue(circuit, targetId, resolved);
        if (actual != testCase.expectedTargetValue) {
            result.message = QStringLiteral("Output is %1 but should be %2 for one input combination.")
                                 .arg(formatSignal(actual), formatSignal(testCase.expectedTargetValue));
            return result;
        }
    }

    result.success = true;
    result.message = QStringLiteral("Level complete! Output matches for all input combinations.");
    return result;
}

} // namespace digitallogic
