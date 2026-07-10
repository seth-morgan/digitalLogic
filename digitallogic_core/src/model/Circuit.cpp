#include "digitallogic/model/Circuit.h"

#include "digitallogic/model/PinIndices.h"

#include <algorithm>

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

WireValidationResult Circuit::validateWire(const PinId& from, const PinId& to) const
{
    if (from.componentId == to.componentId) {
        return WireValidationResult::SameComponent;
    }

    const Gate* destinationGate = findGate(to.componentId);
    if (destinationGate == nullptr) {
        return WireValidationResult::InvalidDestination;
    }

    if (to.pinIndex < 0 || to.pinIndex >= destinationGate->inputCount()) {
        return WireValidationResult::InvalidDestination;
    }

    const bool fromIsSource = findSource(from.componentId) != nullptr;
    const Gate* fromGate = findGate(from.componentId);
    if (!fromIsSource && fromGate == nullptr) {
        return WireValidationResult::InvalidSource;
    }

    if (fromGate != nullptr && !isGateOutputPin(from.pinIndex, fromGate->inputCount())) {
        return WireValidationResult::InvalidSource;
    }

    if (fromIsSource && from.pinIndex != sourceOutputPinIndex()) {
        return WireValidationResult::InvalidSource;
    }

    for (const Wire& existing : m_wires) {
        if (existing.to == to) {
            return WireValidationResult::InputAlreadyConnected;
        }
    }

    return WireValidationResult::Ok;
}

bool Circuit::addWire(const PinId& from, const PinId& to)
{
    if (validateWire(from, to) != WireValidationResult::Ok) {
        return false;
    }

    m_wires.push_back(Wire{from, to});
    return true;
}

bool Circuit::removeGate(const ComponentId id)
{
    const auto gateIt = std::find_if(m_gates.begin(), m_gates.end(), [id](const std::unique_ptr<Gate>& gate) {
        return gate->id() == id;
    });

    if (gateIt == m_gates.end()) {
        return false;
    }

    m_wires.erase(std::remove_if(m_wires.begin(), m_wires.end(),
                                 [id](const Wire& wire) {
                                     return wire.from.componentId == id || wire.to.componentId == id;
                                 }),
                  m_wires.end());

    m_gates.erase(gateIt);
    m_placements.remove(id);
    return true;
}

bool Circuit::removeWire(const PinId& from, const PinId& to)
{
    const auto wireIt = std::find_if(m_wires.begin(), m_wires.end(), [&](const Wire& wire) {
        return wire.from == from && wire.to == to;
    });

    if (wireIt == m_wires.end()) {
        return false;
    }

    m_wires.erase(wireIt);
    return true;
}

bool Circuit::setComponentPosition(const ComponentId id, const QPointF& position)
{
    if (!m_placements.contains(id)) {
        return false;
    }

    m_placements[id].position = position;
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

void Circuit::clearGatesAndWires()
{
    m_gates.clear();
    m_wires.clear();

    QHash<ComponentId, ComponentPlacement> sourcePlacements;
    for (const SourceNode& source : m_sources) {
        const auto placementIt = m_placements.find(source.id());
        if (placementIt != m_placements.end()) {
            sourcePlacements.insert(source.id(), placementIt.value());
        }
        findSource(source.id())->setValue(SignalValue::False);
    }

    m_placements = sourcePlacements;
}

bool Circuit::addSourceWithId(const ComponentId id, const QPointF& position, const SignalValue initialValue)
{
    if (findSource(id) != nullptr || findGate(id) != nullptr) {
        return false;
    }

    m_sources.emplace_back(id, initialValue);
    m_placements.insert(id, ComponentPlacement{id, position});
    return true;
}

bool Circuit::addGateWithId(const ComponentId id, const GateKind kind, const QPointF& position)
{
    if (findSource(id) != nullptr || findGate(id) != nullptr) {
        return false;
    }

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
        return false;
    }

    m_gates.push_back(std::move(gate));
    m_placements.insert(id, ComponentPlacement{id, position});
    return true;
}

} // namespace digitallogic
