/**
 * @file SimulationEngine.cpp
 * @brief Topologically evaluates circuits and resolves pin signal values.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/model/SimulationEngine.h"

#include "digitallogic/model/PinIndices.h"
#include "digitallogic/gates/AndGate.h"
#include "digitallogic/gates/NandGate.h"
#include "digitallogic/gates/NotGate.h"
#include "digitallogic/gates/OrGate.h"
#include "digitallogic/gates/XorGate.h"

#include <QSet>
#include <QVector>

namespace digitallogic {

namespace {

/**
 * @brief Builds the output PinId for a source component.
 */
[[nodiscard]] PinId sourceOutputPin(const ComponentId componentId)
{
    return PinId{componentId, sourceOutputPinIndex()};
}

/**
 * @brief Builds the output PinId for a gate given its input arity.
 */
[[nodiscard]] PinId gateOutputPin(const ComponentId componentId, const int inputCount)
{
    return PinId{componentId, gateOutputPinIndex(inputCount)};
}

/**
 * @brief DFS cycle check: visiting = on the current path, visited = fully explored.
 */
[[nodiscard]] bool dependsOn(const Gate* gate, const Circuit& circuit, const QHash<ComponentId, QSet<ComponentId>>& adjacency, QSet<ComponentId>& visiting, QSet<ComponentId>& visited, QString& error)
{
    if (visited.contains(gate->id())) {
        return true;
    }

    // Re-entering a node still on the stack means a feedback loop exists.
    if (visiting.contains(gate->id())) {
        error = QStringLiteral("Cycle detected in circuit");
        return false;
    }

    visiting.insert(gate->id());

    const auto upstreamIt = adjacency.find(gate->id());
    if (upstreamIt != adjacency.end()) {
        for (const ComponentId upstreamId : upstreamIt.value()) {
            if (const Gate* upstreamGate = circuit.findGate(upstreamId)) {
                if (!dependsOn(upstreamGate, circuit, adjacency, visiting, visited, error)) {
                    return false;
                }
            }
        }
    }

    visiting.remove(gate->id());
    visited.insert(gate->id());
    return true;
}

/**
 * @brief Returns a gate evaluation order, or empty if the circuit has a cycle.
 */
[[nodiscard]] std::optional<QVector<ComponentId>> topologicalOrder(const Circuit& circuit, QString& error)
{
    // adjacency[gate] = set of upstream component ids that drive its inputs.
    QHash<ComponentId, QSet<ComponentId>> adjacency;
    for (const Wire& wire : circuit.wires()) {
        adjacency[wire.to.componentId].insert(wire.from.componentId);
    }

    QSet<ComponentId> visiting;
    QSet<ComponentId> visited;
    QVector<ComponentId> order;

    // First pass: reject cyclic graphs before building the evaluation order.
    for (const auto& gatePtr : circuit.gates()) {
        if (!dependsOn(gatePtr.get(), circuit, adjacency, visiting, visited, error)) {
            return std::nullopt;
        }
    }

    visiting.clear();
    visited.clear();

    // Recursive lambda (self) performs a post-order DFS to emit gates after dependencies.
    const auto visit = [&](const auto& self, ComponentId gateId) -> bool {
        if (visited.contains(gateId)) {
            return true;
        }
        if (visiting.contains(gateId)) {
            error = QStringLiteral("Cycle detected in circuit");
            return false;
        }

        visiting.insert(gateId);
        const auto upstreamIt = adjacency.find(gateId);
        if (upstreamIt != adjacency.end()) {
            for (const ComponentId upstreamId : upstreamIt.value()) {
                if (circuit.findGate(upstreamId) != nullptr) {
                    if (!self(self, upstreamId)) {
                        return false;
                    }
                }
            }
        }
        visiting.remove(gateId);
        visited.insert(gateId);
        order.push_back(gateId); // Emit only after all upstream gates.
        return true;
    };

    for (const auto& gatePtr : circuit.gates()) {
        if (!visit(visit, gatePtr->id())) {
            return std::nullopt;
        }
    }

    return order;
}

/**
 * @brief Resolves a pin value from the map, falling back to live source values.
 */
[[nodiscard]] SignalValue resolvePin(const PinId& pin, const Circuit& circuit, const QHash<PinId, SignalValue>& resolved)
{
    if (const auto it = resolved.find(pin); it != resolved.end()) {
        return it.value();
    }

    // Fall back to the live source value when the pin belongs to a SourceNode.
    if (const SourceNode* source = circuit.findSource(pin.componentId)) {
        return source->value();
    }

    return SignalValue::Unknown;
}

} // namespace

/**
 * @brief Evaluates an AND over the given inputs (for unit tests).
 */
SignalValue SimulationEngine::evaluateAnd(const QVector<SignalValue>& inputs)
{
    return AndGate(ComponentId{}).evaluate(inputs);
}

/**
 * @brief Evaluates an OR over the given inputs (for unit tests).
 */
SignalValue SimulationEngine::evaluateOr(const QVector<SignalValue>& inputs)
{
    return OrGate(ComponentId{}).evaluate(inputs);
}

/**
 * @brief Evaluates a NOT of the given input (for unit tests).
 */
SignalValue SimulationEngine::evaluateNot(const SignalValue input)
{
    return NotGate(ComponentId{}).evaluate(QVector<SignalValue>{input});
}

/**
 * @brief Evaluates a NAND over the given inputs (for unit tests).
 */
SignalValue SimulationEngine::evaluateNand(const QVector<SignalValue>& inputs)
{
    return NandGate(ComponentId{}).evaluate(inputs);
}

/**
 * @brief Evaluates an XOR over the given inputs (for unit tests).
 */
SignalValue SimulationEngine::evaluateXor(const QVector<SignalValue>& inputs)
{
    return XorGate(ComponentId{}).evaluate(inputs);
}

/**
 * @brief Runs simulation over the given circuit and returns resolved pin values.
 */
std::optional<QVector<PinSignal>> SimulationEngine::run(const Circuit& circuit,
                                                        const QHash<ComponentId, SignalValue>* sourceOverrides)
{
    QString error;
    const std::optional<QVector<ComponentId>> order = topologicalOrder(circuit, error);
    if (!order.has_value()) {
        return std::nullopt; // Cyclic circuit cannot be evaluated safely.
    }

    QHash<PinId, SignalValue> resolved;

    // Seed the map with source outputs (optionally overridden for challenge tests).
    for (const SourceNode& source : circuit.sources()) {
        SignalValue value = source.value();
        if (sourceOverrides != nullptr) {
            const auto overrideIt = sourceOverrides->find(source.id());
            if (overrideIt != sourceOverrides->end()) {
                value = overrideIt.value();
            }
        }
        resolved.insert(sourceOutputPin(source.id()), value);
    }

    // Evaluate each gate once its upstream pins are known.
    for (const ComponentId gateId : order.value()) {
        const Gate* gate = circuit.findGate(gateId);
        if (gate == nullptr) {
            continue;
        }

        QVector<SignalValue> inputs;
        inputs.reserve(gate->inputCount());
        for (int inputIndex = 0; inputIndex < gate->inputCount(); ++inputIndex) {
            const PinId inputPin{gateId, inputIndex};
            SignalValue inputValue = SignalValue::Unknown;

            // Find the single wire that (may) drive this input pin.
            for (const Wire& wire : circuit.wires()) {
                if (wire.to == inputPin) {
                    inputValue = resolvePin(wire.from, circuit, resolved);
                    break;
                }
            }

            inputs.push_back(inputValue);
        }

        resolved.insert(gateOutputPin(gateId, gate->inputCount()), gate->evaluate(inputs));
    }

    // Copy driven values onto destination pins so UI coloring can read either end.
    for (const Wire& wire : circuit.wires()) {
        if (!resolved.contains(wire.to)) {
            resolved.insert(wire.to, resolvePin(wire.from, circuit, resolved));
        }
    }

    QVector<PinSignal> results;
    results.reserve(resolved.size());
    for (auto it = resolved.begin(); it != resolved.end(); ++it) {
        results.push_back(PinSignal{it.key(), it.value()});
    }

    return results;
}

} // namespace digitallogic
