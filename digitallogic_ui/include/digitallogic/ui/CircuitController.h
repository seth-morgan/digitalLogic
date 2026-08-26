/**
 * @file CircuitController.h
 * @brief Declares the controller that syncs sandbox graphics with the circuit model.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/challenge/ChallengeLevel.h"
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
class TargetGraphicsItem;
class WireGraphicsItem;

/**
 * @brief Synchronizes sandbox interactions with the core circuit model.
 */
class CircuitController final : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a controller bound to the given sandbox view.
     */
    explicit CircuitController(SandboxView* view, QObject* parent = nullptr);

    /**
     * @brief Returns a mutable reference to the underlying circuit model.
     */
    [[nodiscard]] Circuit& circuit() noexcept { return m_circuit; }
    /**
     * @brief Returns a const reference to the underlying circuit model.
     */
    [[nodiscard]] const Circuit& circuit() const noexcept { return m_circuit; }

    /**
     * @brief Places the default sandbox input sources on an empty canvas.
     */
    void initializeDefaultSources();
    /**
     * @brief Leaves challenge mode and restores a normal sandbox canvas.
     */
    void restoreSandboxMode();
    /**
     * @brief Loads a challenge level's locked sources and target onto the canvas.
     */
    // Builds locked challenge sources/target and returns ids for validation.
    void loadChallengeLevel(const ChallengeLevel& level, QHash<QString, ComponentId>& outSourceIdsByLabel,
                              ComponentId& outTargetId);
    /**
     * @brief Sets the remaining challenge gate budget used for placement checks.
     */
    void setChallengeGateBudget(const QHash<GateKind, int>& remaining);
    /**
     * @brief Clears any challenge gate budget constraints.
     */
    void clearChallengeGateBudget();
    /**
     * @brief Returns whether challenge mode is currently active.
     */
    [[nodiscard]] bool isChallengeMode() const noexcept { return m_challengeMode; }
    /**
     * @brief Returns the challenge output target component id.
     */
    [[nodiscard]] ComponentId challengeTargetId() const noexcept { return m_challengeTargetId; }
    /**
     * @brief Returns challenge source component ids keyed by label.
     */
    [[nodiscard]] QHash<QString, ComponentId> challengeSourceIdsByLabel() const { return m_challengeSourceIdsByLabel; }
    /**
     * @brief Returns the remaining challenge gate budget by kind.
     */
    [[nodiscard]] QHash<GateKind, int> challengeGateBudgetRemaining() const { return m_challengeGateBudget; }
    /**
     * @brief Toggles the boolean value of an input source.
     */
    void toggleSource(ComponentId sourceId);
    /**
     * @brief Places a gate from the palette at the given scene position.
     */
    [[nodiscard]] bool placeGateFromPalette(GateKind kind, const QPointF& scenePosition);

    /**
     * @brief Begins a wire drag starting at the given pin.
     */
    void beginWireDrag(PinGraphicsItem* fromPin, const QPointF& scenePos);
    /**
     * @brief Updates the rubber-band wire preview during an active drag.
     */
    void updateWireDrag(const QPointF& scenePos);
    /**
     * @brief Completes a wire drag at the release scene position.
     */
    void endWireDrag(const QPointF& scenePos);
    /**
     * @brief Returns whether a wire drag is currently in progress.
     */
    [[nodiscard]] bool isWireDragInProgress() const noexcept { return m_wireDragInProgress; }
    /**
     * @brief Recomputes geometry for every wire on the canvas.
     */
    void updateAllWirePaths();
    /**
     * @brief Updates a gate's model position after it is moved in the view.
     */
    void updateGatePosition(ComponentId gateId, const QPointF& position);
    /**
     * @brief Deletes the currently selected canvas items from model and view.
     */
    void deleteSelection();
    /**
     * @brief Clears all components and graphics from the canvas.
     */
    void clearCanvas();
    /**
     * @brief Saves the circuit model to the given file path.
     */
    [[nodiscard]] bool saveToFile(const QString& path);
    /**
     * @brief Loads a circuit model from the given file path.
     */
    [[nodiscard]] bool loadFromFile(const QString& path);

    /**
     * @brief Finds the graphics pin for the given pin id.
     */
    [[nodiscard]] PinGraphicsItem* findPin(const PinId& pinId) const;
    /**
     * @brief Finds a pin under the given scene position, if any.
     */
    [[nodiscard]] PinGraphicsItem* findPinAtScenePos(const QPointF& scenePos) const;
    /**
     * @brief Finds the source graphics item for the given component id.
     */
    [[nodiscard]] SourceGraphicsItem* findSourceItem(ComponentId id) const;
    /**
     * @brief Finds the target graphics item for the given component id.
     */
    [[nodiscard]] TargetGraphicsItem* findTargetItem(ComponentId id) const;
    /**
     * @brief Finds the gate graphics item for the given component id.
     */
    [[nodiscard]] GateGraphicsItem* findGateItem(ComponentId id) const;
    /**
     * @brief Returns whether the component is a protected challenge source or target.
     */
    // Challenge sources/target cannot be deleted or moved by the player.
    [[nodiscard]] bool isProtectedComponent(ComponentId id) const;

    /**
     * @brief Applies simulated pin values to sandbox graphics.
     */
    void applySimulationResults(const QHash<PinId, SignalValue>& pinValues);
    /**
     * @brief Clears simulation coloring from all sandbox graphics.
     */
    void clearSimulationVisuals();

signals:
    void circuitChanged();
    void statusMessage(const QString& message);
    void gateBudgetChanged();
    void sourceValueChanged();

private:
    /**
     * @brief Attempts to connect two pins and reports validation failures.
     */
    [[nodiscard]] bool tryConnectPins(const PinId& from, const PinId& to);
    /**
     * @brief Emits a user-facing status message for a wire validation failure.
     */
    void reportWireFailure(WireValidationResult result);
    /**
     * @brief Cancels an in-progress wire drag and removes the preview line.
     */
    void cancelWireDrag();
    /**
     * @brief Creates sandbox graphics for a source component.
     */
    void createSourceItem(ComponentId id, const QPointF& position, const QString& label = QString(), bool locked = false);
    /**
     * @brief Creates sandbox graphics for a target component.
     */
    void createTargetItem(ComponentId id, const QPointF& position);
    /**
     * @brief Creates sandbox graphics for a gate component.
     */
    void createGateItem(ComponentId id, GateKind kind, int inputCount, const QPointF& position);
    /**
     * @brief Creates a wire graphics item between two pins.
     */
    void createWireItem(const PinId& from, const PinId& to);
    /**
     * @brief Removes a wire graphics item from the scene and bookkeeping.
     */
    void removeWireItem(WireGraphicsItem* wireItem);
    /**
     * @brief Removes a gate graphics item and its attached wires.
     */
    void removeGateItem(GateGraphicsItem* gateItem);
    /**
     * @brief Removes all circuit graphics from the scene.
     */
    void removeAllGraphics();
    /**
     * @brief Rebuilds all graphics items from the current circuit model.
     */
    void rebuildGraphics();
    /**
     * @brief Clears pending-wire highlight from the drag-start pin.
     */
    void clearPendingWireHighlight();
    /**
     * @brief Updates the rubber-band preview line to the given scene position.
     */
    void updatePreviewLine(const QPointF& scenePos);
    /**
     * @brief Clears challenge-only controller state and gate budgets.
     */
    void resetChallengeState();
    /**
     * @brief Places the default sandbox input sources on an empty canvas.
     */
    void addDefaultSources();

    SandboxView* m_view;
    Circuit m_circuit;
    PinGraphicsItem* m_wireDragFromPin{nullptr};
    // Temporary rubber-band line while the user drags a new wire.
    QGraphicsLineItem* m_wirePreviewLine{nullptr};
    QVector<WireGraphicsItem*> m_wireItems;
    bool m_wireDragInProgress{false};
    bool m_challengeMode{false};
    ComponentId m_challengeTargetId{};
    QHash<QString, ComponentId> m_challengeSourceIdsByLabel;
    QHash<GateKind, int> m_challengeGateBudget;
    // Original level limits used when refreshing remaining budget after place/delete.
    QHash<GateKind, int> m_challengeGateBudgetTotal;
};

} // namespace digitallogic::ui
