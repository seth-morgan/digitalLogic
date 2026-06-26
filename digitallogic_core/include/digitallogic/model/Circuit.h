#pragma once

#include "digitallogic/gates/AndGate.h"
#include "digitallogic/gates/NotGate.h"
#include "digitallogic/gates/OrGate.h"
#include "digitallogic/model/ComponentIdFactory.h"
#include "digitallogic/model/Gate.h"
#include "digitallogic/model/PinId.h"
#include "digitallogic/model/SourceNode.h"
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
     * @brief Connects an output pin to an input pin.
     * @param from Output pin (source output uses pin index 0).
     * @param to Input pin on a gate.
     * @return True when the wire was added.
     */
    [[nodiscard]] bool addWire(const PinId& from, const PinId& to);

    [[nodiscard]] const QVector<SourceNode>& sources() const noexcept { return m_sources; }
    [[nodiscard]] const std::vector<std::unique_ptr<Gate>>& gates() const noexcept { return m_gates; }
    [[nodiscard]] const QVector<Wire>& wires() const noexcept { return m_wires; }
    [[nodiscard]] const QHash<ComponentId, ComponentPlacement>& placements() const noexcept { return m_placements; }

    [[nodiscard]] SourceNode* findSource(ComponentId id);
    [[nodiscard]] const SourceNode* findSource(ComponentId id) const;
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

private:
    QVector<SourceNode> m_sources;
    std::vector<std::unique_ptr<Gate>> m_gates;
    QVector<Wire> m_wires;
    QHash<ComponentId, ComponentPlacement> m_placements;
};

} // namespace digitallogic
