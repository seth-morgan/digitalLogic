/**
 * @file SourceGraphicsItem.h
 * @brief Declares the graphics item for a toggleable input source.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/ComponentId.h"
#include "digitallogic/model/PinIndices.h"
#include "digitallogic/model/SignalValue.h"

#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>

namespace digitallogic::ui {

class CircuitController;
class PinGraphicsItem;

/**
 * @brief Visual representation of a toggleable input source.
 */
class SourceGraphicsItem final : public QGraphicsItemGroup {
public:
    /**
     * @brief Constructs a source graphics item for the given component id.
     */
    SourceGraphicsItem(ComponentId componentId, CircuitController* controller, QGraphicsItem* parent = nullptr);

    /**
     * @brief Returns the source component id.
     */
    [[nodiscard]] ComponentId componentId() const noexcept { return m_componentId; }
    /**
     * @brief Returns the output pin graphics item.
     */
    [[nodiscard]] PinGraphicsItem* outputPin() const noexcept { return m_outputPin; }

    /**
     * @brief Refreshes body and label appearance from the model signal value.
     */
    void refreshFromModel(SignalValue value);
    /**
     * @brief Updates the displayed true/false value label.
     */
    void updateValueLabel(SignalValue value);
    /**
     * @brief Sets the optional display label shown on the source.
     */
    void setDisplayLabel(const QString& label);
    /**
     * @brief Locks the source against toggle, move, and delete in challenge mode.
     */
    // Locked sources (challenge mode) ignore toggle clicks and delete/move.
    void setLocked(bool locked);
    /**
     * @brief Clears simulation highlight styling from the source.
     */
    void clearSimulationHighlight();
    /**
     * @brief Recomputes attached wire endpoints after the source moves.
     */
    void updateWirePaths();

protected:
    /**
     * @brief Toggles the source value on click when unlocked.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    /**
     * @brief Syncs model position when the item is moved on the canvas.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    ComponentId m_componentId;
    CircuitController* m_controller;
    QGraphicsRectItem* m_body{nullptr};
    QGraphicsTextItem* m_title{nullptr};
    QGraphicsTextItem* m_label{nullptr};
    PinGraphicsItem* m_outputPin{nullptr};
    bool m_locked{false};
};

} // namespace digitallogic::ui
