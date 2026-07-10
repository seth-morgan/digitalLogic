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

[[nodiscard]] PinId sourceOutputPin(const ComponentId componentId)
{
    return PinId{componentId, sourceOutputPinIndex()};
}

[[nodiscard]] PinId gateOutputPin(const ComponentId componentId, const int inputCount)
{
    return PinId{componentId, gateOutputPinIndex(inputCount)};
}

[[nodiscard]] bool dependsOn(const Gate* gate, const Circuit& circuit, const QHash<ComponentId, QSet<ComponentId>>& adjacency, QSet<ComponentId>& visiting, QSet<ComponentId>& visited, QString& error)
{
    if (visited.contains(gate->id())) {
        return true;
    }

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

[[nodiscard]] std::optional<QVector<ComponentId>> topologicalOrder(const Circuit& circuit, QString& error)
{
    QHash<ComponentId, QSet<ComponentId>> adjacency;
    for (const Wire& wire : circuit.wires()) {
        adjacency[wire.to.componentId].insert(wire.from.componentId);
    }

    QSet<ComponentId> visiting;
    QSet<ComponentId> visited;
    QVector<ComponentId> order;

    for (const auto& gatePtr : circuit.gates()) {
        if (!dependsOn(gatePtr.get(), circuit, adjacency, visiting, visited, error)) {
            return std::nullopt;
        }
    }

    visiting.clear();
    visited.clear();

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
        order.push_back(gateId);
        return true;
    };

    for (const auto& gatePtr : circuit.gates()) {
        if (!visit(visit, gatePtr->id())) {
            return std::nullopt;
        }
    }

    return order;
}

[[nodiscard]] SignalValue resolvePin(const PinId& pin, const Circuit& circuit, const QHash<PinId, SignalValue>& resolved)
{
    if (const auto it = resolved.find(pin); it != resolved.end()) {
        return it.value();
    }

    if (const SourceNode* source = circuit.findSource(pin.componentId)) {
        return source->value();
    }

    return SignalValue::Unknown;
}

} // namespace

SignalValue SimulationEngine::evaluateAnd(const QVector<SignalValue>& inputs)
{
    return AndGate(ComponentId{}).evaluate(inputs);
}

SignalValue SimulationEngine::evaluateOr(const QVector<SignalValue>& inputs)
{
    return OrGate(ComponentId{}).evaluate(inputs);
}

SignalValue SimulationEngine::evaluateNot(const SignalValue input)
{
    return NotGate(ComponentId{}).evaluate(QVector<SignalValue>{input});
}

SignalValue SimulationEngine::evaluateNand(const QVector<SignalValue>& inputs)
{
    return NandGate(ComponentId{}).evaluate(inputs);
}

SignalValue SimulationEngine::evaluateXor(const QVector<SignalValue>& inputs)
{
    return XorGate(ComponentId{}).evaluate(inputs);
}

std::optional<QVector<PinSignal>> SimulationEngine::run(const Circuit& circuit,
                                                        const QHash<ComponentId, SignalValue>* sourceOverrides)
{
    QString error;
    const std::optional<QVector<ComponentId>> order = topologicalOrder(circuit, error);
    if (!order.has_value()) {
        return std::nullopt;
    }

    QHash<PinId, SignalValue> resolved;

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
