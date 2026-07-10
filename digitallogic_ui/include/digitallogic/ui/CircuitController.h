#pragma once

#include "digitallogic/model/Circuit.h"
#include "digitallogic/model/PinId.h"
#include "digitallogic/model/SignalValue.h"
#include "digitallogic/model/WireValidation.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include <QHash>
#include <QObject>
#include <QPointF>
#include <QString>
#include <QVector>
#include <optional>

class QGraphicsLineItem;

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
    void toggleSource(ComponentId sourceId);
    [[nodiscard]] bool placeGateFromPalette(GateKind kind, const QPointF& scenePosition);

    void beginWireDrag(PinGraphicsItem* fromPin, const QPointF& scenePos);
    void updateWireDrag(const QPointF& scenePos);
    void endWireDrag(const QPointF& scenePos);
    [[nodiscard]] bool isWireDragInProgress() const noexcept { return m_wireDragInProgress; }
    void updateAllWirePaths();
    void updateGatePosition(ComponentId gateId, const QPointF& position);
    void deleteSelection();
    void clearCanvas();
    [[nodiscard]] bool saveToFile(const QString& path);
    [[nodiscard]] bool loadFromFile(const QString& path);

    [[nodiscard]] PinGraphicsItem* findPin(const PinId& pinId) const;
    [[nodiscard]] PinGraphicsItem* findPinAtScenePos(const QPointF& scenePos) const;
    [[nodiscard]] WireGraphicsItem* findWireAtScenePos(const QPointF& scenePos) const;
    [[nodiscard]] SourceGraphicsItem* findSourceItem(ComponentId id) const;
    [[nodiscard]] GateGraphicsItem* findGateItem(ComponentId id) const;

    void applySimulationResults(const QHash<PinId, SignalValue>& pinValues);
    void clearSimulationVisuals();

signals:
    void circuitChanged();
    void statusMessage(const QString& message);

private:
    [[nodiscard]] bool tryConnectPins(const PinId& from, const PinId& to);
    void reportWireFailure(WireValidationResult result);
    void cancelWireDrag();
    void createSourceItem(ComponentId id, const QPointF& position);
    void createGateItem(ComponentId id, GateKind kind, int inputCount, const QPointF& position);
    void createWireItem(const PinId& from, const PinId& to);
    void removeWireItem(WireGraphicsItem* wireItem);
    void removeGateItem(GateGraphicsItem* gateItem);
    void removeAllGraphics();
    void rebuildGraphics();
    void clearPendingWireHighlight();
    void updatePreviewLine(const QPointF& scenePos);

    SandboxView* m_view;
    Circuit m_circuit;
    PinGraphicsItem* m_wireDragFromPin{nullptr};
    QGraphicsLineItem* m_wirePreviewLine{nullptr};
    QVector<WireGraphicsItem*> m_wireItems;
    bool m_wireDragInProgress{false};
};

} // namespace digitallogic::ui
