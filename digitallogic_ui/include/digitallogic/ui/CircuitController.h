#pragma once

#include "digitallogic/model/Circuit.h"
#include "digitallogic/model/PinId.h"
#include "digitallogic/model/SignalValue.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include <QHash>
#include <QObject>
#include <QPointF>
#include <optional>

namespace digitallogic::ui {

class GateGraphicsItem;
class SandboxView;
class SourceGraphicsItem;
class WireGraphicsItem;
/**
 * @brief Synchronizes sandbox interactions with the core circuit model.
 */
class CircuitController final : public QObject {
    Q_OBJECT

public:
    explicit CircuitController(SandboxView* view, QObject* parent = nullptr);

    [[nodiscard]] Circuit& circuit() noexcept { return m_circuit; }
    [[nodiscard]] const Circuit& circuit() const noexcept { return m_circuit; }

    void initializeDefaultSources();
    void handlePinClicked(const PinId& pinId, PinGraphicsItem::PinRole role);
    void toggleSource(ComponentId sourceId);
    [[nodiscard]] bool placeGateFromPalette(GateKind kind, const QPointF& scenePosition);

    [[nodiscard]] PinGraphicsItem* findPin(const PinId& pinId) const;
    [[nodiscard]] SourceGraphicsItem* findSourceItem(ComponentId id) const;
    [[nodiscard]] GateGraphicsItem* findGateItem(ComponentId id) const;

    void applySimulationResults(const QHash<PinId, SignalValue>& pinValues);
    void clearSimulationVisuals();

signals:
    void circuitChanged();

private:
    void createSourceItem(ComponentId id, const QPointF& position);
    void createGateItem(ComponentId id, GateKind kind, int inputCount, const QPointF& position);
    void createWireItem(const PinId& from, const PinId& to);

    SandboxView* m_view;
    Circuit m_circuit;
    std::optional<PinId> m_pendingWireFrom;
};

} // namespace digitallogic::ui
