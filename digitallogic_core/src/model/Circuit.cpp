/**
 * @file Circuit.cpp
 * @brief Implements circuit mutation, lookup, and wire-validation helpers.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/model/Circuit.h"

#include "digitallogic/model/PinIndices.h"

#include <algorithm>

namespace digitallogic {

/**
 * @brief Adds a pre-placed source node at the given canvas position.
 */
ComponentId Circuit::addSource(const QPointF& position, const SignalValue initialValue)
{
    const ComponentId id = makeComponentId();
    m_sources.emplace_back(id, initialValue);
    m_placements.insert(id, ComponentPlacement{id, position}); // Track canvas position in the model
    return id;
}

/**
 * @brief Adds a fixed output target at the given canvas position.
 */
ComponentId Circuit::addTarget(const QPointF& position)
{
    const ComponentId id = makeComponentId();
    m_targets.emplace_back(id);
    m_placements.insert(id, ComponentPlacement{id, position});
    return id;
}

/**
 * @brief Adds a gate of the requested kind at the given canvas position.
 */
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
    case GateKind::Nand:
        gate = std::make_unique<NandGate>(id);
        break;
    case GateKind::Xor:
        gate = std::make_unique<XorGate>(id);
        break;
    default:
        return std::nullopt;
    }

    m_gates.push_back(std::move(gate));
    m_placements.insert(id, ComponentPlacement{id, position});
    return id;
}

/**
 * @brief Validates a proposed wire without mutating the circuit.
 */
WireValidationResult Circuit::validateWire(const PinId& from, const PinId& to) const
{
    // Reject self-loops even if the pin roles would otherwise be valid.
    if (from.componentId == to.componentId) {
        return WireValidationResult::SameComponent;
    }

    const Gate* destinationGate = findGate(to.componentId);
    const TargetNode* destinationTarget = findTarget(to.componentId);
    if (destinationGate == nullptr && destinationTarget == nullptr) {
        return WireValidationResult::InvalidDestination;
    }

    // Destination must be a gate input index or the challenge target's single input.
    if (destinationGate != nullptr) {
        if (to.pinIndex < 0 || to.pinIndex >= destinationGate->inputCount()) {
            return WireValidationResult::InvalidDestination;
        }
    } else if (to.pinIndex != targetInputPinIndex()) {
        return WireValidationResult::InvalidDestination;
    }

    const bool fromIsSource = findSource(from.componentId) != nullptr;
    const Gate* fromGate = findGate(from.componentId);
    if (!fromIsSource && fromGate == nullptr) {
        return WireValidationResult::InvalidSource;
    }

    // Gate outputs use pin index == inputCount; inputs must not be used as sources.
    if (fromGate != nullptr && !isGateOutputPin(from.pinIndex, fromGate->inputCount())) {
        return WireValidationResult::InvalidSource;
    }

    if (fromIsSource && from.pinIndex != sourceOutputPinIndex()) {
        return WireValidationResult::InvalidSource;
    }

    // Each input pin accepts at most one incoming wire.
    for (const Wire& existing : m_wires) {
        if (existing.to == to) {
            return WireValidationResult::InputAlreadyConnected;
        }
    }

    return WireValidationResult::Ok;
}

/**
 * @brief Connects an output pin to an input pin.
 */
bool Circuit::addWire(const PinId& from, const PinId& to)
{
    if (validateWire(from, to) != WireValidationResult::Ok) {
        return false;
    }

    m_wires.push_back(Wire{from, to});
    return true;
}

/**
 * @brief Removes a gate and any wires attached to it.
 */
bool Circuit::removeGate(const ComponentId id)
{
    const auto gateIt = std::find_if(m_gates.begin(), m_gates.end(), [id](const std::unique_ptr<Gate>& gate) {
        return gate->id() == id;
    });

    if (gateIt == m_gates.end()) {
        return false;
    }

    // Erase-remove: drop every wire that touches the deleted gate.
    m_wires.erase(std::remove_if(m_wires.begin(), m_wires.end(),
                                 [id](const Wire& wire) {
                                     return wire.from.componentId == id || wire.to.componentId == id;
                                 }),
                  m_wires.end());

    m_gates.erase(gateIt);
    m_placements.remove(id);
    return true;
}

/**
 * @brief Removes a wire between two pins.
 */
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

/**
 * @brief Updates the stored canvas position for a component.
 */
bool Circuit::setComponentPosition(const ComponentId id, const QPointF& position)
{
    if (!m_placements.contains(id)) {
        return false;
    }

    m_placements[id].position = position;
    return true;
}

/**
 * @brief Finds a mutable source by component id, or nullptr if absent.
 */
SourceNode* Circuit::findSource(const ComponentId id)
{
    for (SourceNode& source : m_sources) {
        if (source.id() == id) {
            return &source;
        }
    }
    return nullptr;
}

/**
 * @brief Finds a const source by component id, or nullptr if absent.
 */
const SourceNode* Circuit::findSource(const ComponentId id) const
{
    for (const SourceNode& source : m_sources) {
        if (source.id() == id) {
            return &source;
        }
    }
    return nullptr;
}

/**
 * @brief Finds a mutable target by component id, or nullptr if absent.
 */
TargetNode* Circuit::findTarget(const ComponentId id)
{
    for (TargetNode& target : m_targets) {
        if (target.id() == id) {
            return &target;
        }
    }
    return nullptr;
}

/**
 * @brief Finds a const target by component id, or nullptr if absent.
 */
const TargetNode* Circuit::findTarget(const ComponentId id) const
{
    for (const TargetNode& target : m_targets) {
        if (target.id() == id) {
            return &target;
        }
    }
    return nullptr;
}

/**
 * @brief Finds a mutable gate by component id, or nullptr if absent.
 */
Gate* Circuit::findGate(const ComponentId id)
{
    for (const auto& gate : m_gates) {
        if (gate->id() == id) {
            return gate.get();
        }
    }
    return nullptr;
}

/**
 * @brief Finds a const gate by component id, or nullptr if absent.
 */
const Gate* Circuit::findGate(const ComponentId id) const
{
    for (const auto& gate : m_gates) {
        if (gate->id() == id) {
            return gate.get();
        }
    }
    return nullptr;
}

/**
 * @brief Updates a source value by component id.
 */
bool Circuit::setSourceValue(const ComponentId id, const SignalValue value)
{
    SourceNode* source = findSource(id);
    if (source == nullptr) {
        return false;
    }

    source->setValue(value);
    return true;
}

/**
 * @brief Toggles a source between true and false.
 */
bool Circuit::toggleSource(const ComponentId id)
{
    SourceNode* source = findSource(id);
    if (source == nullptr) {
        return false;
    }

    source->toggle();
    return true;
}

/**
 * @brief Removes all gates and wires; resets source values to false.
 */
void Circuit::clearGatesAndWires(const bool keepTargets)
{
    m_gates.clear();
    m_wires.clear();

    // Keep source (and optionally target) placements so the canvas layout survives Clear.
    QHash<ComponentId, ComponentPlacement> preservedPlacements;
    for (const SourceNode& source : m_sources) {
        const auto placementIt = m_placements.find(source.id());
        if (placementIt != m_placements.end()) {
            preservedPlacements.insert(source.id(), placementIt.value());
        }
        findSource(source.id())->setValue(SignalValue::False);
    }

    if (keepTargets) {
        for (const TargetNode& target : m_targets) {
            const auto placementIt = m_placements.find(target.id());
            if (placementIt != m_placements.end()) {
                preservedPlacements.insert(target.id(), placementIt.value());
            }
        }
    } else {
        m_targets.clear();
    }

    m_placements = preservedPlacements;
}

/**
 * @brief Removes all components and topology from the circuit.
 */
void Circuit::clearAll()
{
    m_sources.clear();
    m_targets.clear();
    m_gates.clear();
    m_wires.clear();
    m_placements.clear();
}

/**
 * @brief Adds a source with an explicit id (for deserialization).
 */
bool Circuit::addSourceWithId(const ComponentId id, const QPointF& position, const SignalValue initialValue)
{
    if (findSource(id) != nullptr || findGate(id) != nullptr || findTarget(id) != nullptr) {
        return false;
    }

    m_sources.emplace_back(id, initialValue);
    m_placements.insert(id, ComponentPlacement{id, position});
    return true;
}

/**
 * @brief Adds a gate with an explicit id (for deserialization).
 */
bool Circuit::addGateWithId(const ComponentId id, const GateKind kind, const QPointF& position)
{
    if (findSource(id) != nullptr || findGate(id) != nullptr || findTarget(id) != nullptr) {
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
    case GateKind::Nand:
        gate = std::make_unique<NandGate>(id);
        break;
    case GateKind::Xor:
        gate = std::make_unique<XorGate>(id);
        break;
    default:
        return false;
    }

    m_gates.push_back(std::move(gate));
    m_placements.insert(id, ComponentPlacement{id, position});
    return true;
}

} // namespace digitallogic
