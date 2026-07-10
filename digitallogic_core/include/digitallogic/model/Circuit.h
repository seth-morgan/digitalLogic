#pragma once

#include "digitallogic/gates/AndGate.h"
#include "digitallogic/gates/NotGate.h"
#include "digitallogic/gates/OrGate.h"
#include "digitallogic/model/ComponentIdFactory.h"
#include "digitallogic/model/Gate.h"
#include "digitallogic/model/PinId.h"
#include "digitallogic/model/WireValidation.h"
#include "digitallogic/model/SourceNode.h"
#include "digitallogic/model/TargetNode.h"
#include "digitallogic/model/Wire.h"

#include <QHash>
#include <QPointF>
#include <QVector>
#include <memory>
#include <optional>
#include <vector>

namespace digitallogic {

/**
 * @brief Canvas placement metadata for a component (core-side, no UI types).
 */
struct ComponentPlacement final {
    ComponentId id;
    QPointF position;
};

/**
 * @brief Owns sources, gates, wires, and placement for a single circuit.
 */
class Circuit final {
public:
    Circuit() = default;
    Circuit(const Circuit&) = delete;
    Circuit& operator=(const Circuit&) = delete;
    Circuit(Circuit&&) noexcept = default;
    Circuit& operator=(Circuit&&) noexcept = default;

    /**
     * @brief Adds a pre-placed source node at the given canvas position.
     * @param position Sandbox coordinates.
     * @param initialValue Starting signal value.
     * @return Id of the created source.
     */
    [[nodiscard]] ComponentId addSource(const QPointF& position, SignalValue initialValue = SignalValue::False);

    /**
     * @brief Adds a gate of the requested kind at the given canvas position.
     * @param kind Gate type to instantiate.
     * @param position Sandbox coordinates.
     * @return Id of the created gate, or empty if kind is unsupported.
     */
    [[nodiscard]] std::optional<ComponentId> addGate(GateKind kind, const QPointF& position);

    /**
     * @brief Adds a fixed output target at the given canvas position.
     * @param position Sandbox coordinates.
     * @return Id of the created target.
     */
    [[nodiscard]] ComponentId addTarget(const QPointF& position);

    /**
     * @brief Connects an output pin to an input pin.
     * @param from Output pin (source output or gate output).
     * @param to Input pin on a gate or challenge target.
     * @return True when the wire was added.
     */
    [[nodiscard]] bool addWire(const PinId& from, const PinId& to);

    /**
     * @brief Validates a proposed wire without mutating the circuit.
     * @param from Output pin.
     * @param to Input pin on a gate or challenge target.
     * @return Validation outcome.
     */
    [[nodiscard]] WireValidationResult validateWire(const PinId& from, const PinId& to) const;

    /**
     * @brief Removes a gate and any wires attached to it.
     * @param id Gate component id.
     * @return True when the gate existed and was removed.
     */
    [[nodiscard]] bool removeGate(ComponentId id);

    /**
     * @brief Removes a wire between two pins.
     * @param from Output pin the wire starts from.
     * @param to Input pin the wire ends on.
     * @return True when the wire existed and was removed.
     */
    [[nodiscard]] bool removeWire(const PinId& from, const PinId& to);

    /**
     * @brief Updates the stored canvas position for a component.
     * @param id Component id.
     * @param position New sandbox coordinates.
     * @return True when the component exists.
     */
    [[nodiscard]] bool setComponentPosition(ComponentId id, const QPointF& position);

    [[nodiscard]] const QVector<SourceNode>& sources() const noexcept { return m_sources; }
    [[nodiscard]] const QVector<TargetNode>& targets() const noexcept { return m_targets; }
    [[nodiscard]] const std::vector<std::unique_ptr<Gate>>& gates() const noexcept { return m_gates; }
    [[nodiscard]] const QVector<Wire>& wires() const noexcept { return m_wires; }
    [[nodiscard]] const QHash<ComponentId, ComponentPlacement>& placements() const noexcept { return m_placements; }

    [[nodiscard]] SourceNode* findSource(ComponentId id);
    [[nodiscard]] const SourceNode* findSource(ComponentId id) const;
    [[nodiscard]] TargetNode* findTarget(ComponentId id);
    [[nodiscard]] const TargetNode* findTarget(ComponentId id) const;
    [[nodiscard]] Gate* findGate(ComponentId id);
    [[nodiscard]] const Gate* findGate(ComponentId id) const;

    /**
     * @brief Updates a source value by component id.
     * @param id Source component id.
     * @param value New signal value.
     * @return True when the source exists.
     */
    [[nodiscard]] bool setSourceValue(ComponentId id, SignalValue value);

    /**
     * @brief Toggles a source between true and false.
     * @param id Source component id.
     * @return True when the source exists.
     */
    [[nodiscard]] bool toggleSource(ComponentId id);

    /**
     * @brief Removes all gates and wires; resets source values to false.
     * @param keepTargets When true, target nodes and their placements are preserved.
     */
    void clearGatesAndWires(bool keepTargets = false);

    /**
     * @brief Removes all components and topology from the circuit.
     */
    void clearAll();

    /**
     * @brief Adds a source with an explicit id (for deserialization).
     */
    [[nodiscard]] bool addSourceWithId(ComponentId id, const QPointF& position, SignalValue initialValue);

    /**
     * @brief Adds a gate with an explicit id (for deserialization).
     */
    [[nodiscard]] bool addGateWithId(ComponentId id, GateKind kind, const QPointF& position);

private:
    QVector<SourceNode> m_sources;
    QVector<TargetNode> m_targets;
    std::vector<std::unique_ptr<Gate>> m_gates;
    QVector<Wire> m_wires;
    QHash<ComponentId, ComponentPlacement> m_placements;
};

} // namespace digitallogic
