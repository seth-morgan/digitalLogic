#include "digitallogic/model/Circuit.h"

namespace digitallogic {

ComponentId Circuit::addSource(const QPointF& position, const SignalValue initialValue)
{
    const ComponentId id = makeComponentId();
    m_sources.emplace_back(id, initialValue);
    m_placements.insert(id, ComponentPlacement{id, position});
    return id;
}

std::optional<ComponentId> Circuit::addGate(const GateKind kind, const QPointF& position)
{
    const ComponentId id = makeComponentId();
    std::unique_ptr<Gate> gate;

    switch (kind) {
    case GateKind::And:
        gate = std::make_unique<AndGate>(id);
        break;
    case GateKind::Or:
        gate = std::make_unique<OrGate>(id);
        break;
    case GateKind::Not:
        gate = std::make_unique<NotGate>(id);
        break;
    default:
        return std::nullopt;
    }

    m_gates.push_back(std::move(gate));
    m_placements.insert(id, ComponentPlacement{id, position});
    return id;
}

bool Circuit::addWire(const PinId& from, const PinId& to)
{
    if (from.componentId == to.componentId) {
        return false;
    }

    const Gate* destinationGate = findGate(to.componentId);
    if (destinationGate == nullptr) {
        return false;
    }

    if (to.pinIndex < 0 || to.pinIndex >= destinationGate->inputCount()) {
        return false;
    }

    const bool fromIsSource = findSource(from.componentId) != nullptr;
    const Gate* fromGate = findGate(from.componentId);
    if (!fromIsSource && fromGate == nullptr) {
        return false;
    }

    if (fromGate != nullptr && from.pinIndex != 0) {
        return false;
    }

    for (const Wire& existing : m_wires) {
        if (existing.to == to) {
            return false;
        }
    }

    m_wires.push_back(Wire{from, to});
    return true;
}

SourceNode* Circuit::findSource(const ComponentId id)
{
    for (SourceNode& source : m_sources) {
        if (source.id() == id) {
            return &source;
        }
    }
    return nullptr;
}

const SourceNode* Circuit::findSource(const ComponentId id) const
{
    for (const SourceNode& source : m_sources) {
        if (source.id() == id) {
            return &source;
        }
    }
    return nullptr;
}

Gate* Circuit::findGate(const ComponentId id)
{
    for (const auto& gate : m_gates) {
        if (gate->id() == id) {
            return gate.get();
        }
    }
    return nullptr;
}

const Gate* Circuit::findGate(const ComponentId id) const
{
    for (const auto& gate : m_gates) {
        if (gate->id() == id) {
            return gate.get();
        }
    }
    return nullptr;
}

bool Circuit::setSourceValue(const ComponentId id, const SignalValue value)
{
    SourceNode* source = findSource(id);
    if (source == nullptr) {
        return false;
    }

    source->setValue(value);
    return true;
}

bool Circuit::toggleSource(const ComponentId id)
{
    SourceNode* source = findSource(id);
    if (source == nullptr) {
        return false;
    }

    source->toggle();
    return true;
}

} // namespace digitallogic
