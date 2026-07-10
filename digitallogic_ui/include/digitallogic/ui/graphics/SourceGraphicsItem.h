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
    void clearSimulationHighlight();
    void updateWirePaths();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    ComponentId m_componentId;
    CircuitController* m_controller;
    QGraphicsRectItem* m_body{nullptr};
    QGraphicsTextItem* m_label{nullptr};
    PinGraphicsItem* m_outputPin{nullptr};
};

} // namespace digitallogic::ui
