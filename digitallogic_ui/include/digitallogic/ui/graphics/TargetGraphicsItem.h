/**
 * @file TargetGraphicsItem.h
 * @brief Declares the graphics item for a challenge output target.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/ComponentId.h"
#include "digitallogic/model/PinIndices.h"
#include "digitallogic/model/SignalValue.h"

#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

namespace digitallogic::ui {

class CircuitController;
class PinGraphicsItem;

/**
 * @brief Visual representation of a challenge output target.
 */
class TargetGraphicsItem final : public QGraphicsItemGroup {
public:
    TargetGraphicsItem(ComponentId componentId, CircuitController* controller, QGraphicsItem* parent = nullptr);

    [[nodiscard]] ComponentId componentId() const noexcept { return m_componentId; }
    [[nodiscard]] PinGraphicsItem* inputPin() const noexcept { return m_inputPin; }

    void setInputSignal(SignalValue value, bool simulated);
    void clearSimulationHighlight();
    void updateWirePaths();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    ComponentId m_componentId;
    CircuitController* m_controller;
    QGraphicsRectItem* m_body{nullptr};
    QGraphicsTextItem* m_valueLabel{nullptr};
    PinGraphicsItem* m_inputPin{nullptr};
};

} // namespace digitallogic::ui
