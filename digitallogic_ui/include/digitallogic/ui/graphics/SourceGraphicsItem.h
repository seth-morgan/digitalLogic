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
    SourceGraphicsItem(ComponentId componentId, CircuitController* controller, QGraphicsItem* parent = nullptr);

    [[nodiscard]] ComponentId componentId() const noexcept { return m_componentId; }
    [[nodiscard]] PinGraphicsItem* outputPin() const noexcept { return m_outputPin; }

    void refreshFromModel(SignalValue value);
    void updateValueLabel(SignalValue value);
    void setDisplayLabel(const QString& label);
    // Locked sources (challenge mode) ignore toggle clicks and delete/move.
    void setLocked(bool locked);
    void clearSimulationHighlight();
    void updateWirePaths();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
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
