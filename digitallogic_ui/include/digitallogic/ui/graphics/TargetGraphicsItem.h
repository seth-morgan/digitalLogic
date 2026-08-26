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
    /**
     * @brief Constructs a target graphics item for the given component id.
     */
    TargetGraphicsItem(ComponentId componentId, CircuitController* controller, QGraphicsItem* parent = nullptr);

    /**
     * @brief Returns the target component id.
     */
    [[nodiscard]] ComponentId componentId() const noexcept { return m_componentId; }
    /**
     * @brief Returns the input pin graphics item.
     */
    [[nodiscard]] PinGraphicsItem* inputPin() const noexcept { return m_inputPin; }

    /**
     * @brief Updates the input appearance from a simulated signal value.
     */
    void setInputSignal(SignalValue value, bool simulated);
    /**
     * @brief Clears simulation highlight styling from the target.
     */
    void clearSimulationHighlight();
    /**
     * @brief Recomputes attached wire endpoints after the target moves.
     */
    void updateWirePaths();

protected:
    /**
     * @brief Syncs model position when the item is moved on the canvas.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    ComponentId m_componentId;
    CircuitController* m_controller;
    QGraphicsRectItem* m_body{nullptr};
    QGraphicsTextItem* m_valueLabel{nullptr};
    PinGraphicsItem* m_inputPin{nullptr};
};

} // namespace digitallogic::ui
