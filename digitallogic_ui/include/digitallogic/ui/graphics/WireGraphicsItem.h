/**
 * @file WireGraphicsItem.h
 * @brief Declares the graphics item for a wire connecting two pins.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/PinId.h"
#include "digitallogic/model/SignalValue.h"

#include <QColor>
#include <QGraphicsLineItem>

namespace digitallogic::ui {

class PinGraphicsItem;

/**
 * @brief Visual wire connecting two pins on the sandbox canvas.
 */
class WireGraphicsItem final : public QGraphicsLineItem {
public:
    WireGraphicsItem(PinGraphicsItem* fromPin, PinGraphicsItem* toPin, QGraphicsItem* parent = nullptr);

    [[nodiscard]] PinId fromPinId() const noexcept { return m_fromPinId; }
    [[nodiscard]] PinId toPinId() const noexcept { return m_toPinId; }

    void updatePath();
    void setSignalValue(SignalValue value, bool simulated);
    void clearSimulationHighlight();
    // Hit-test with a wider tolerance than the thin painted stroke.
    [[nodiscard]] bool containsScenePoint(const QPointF& scenePos) const;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    // Drives the animated signal-flow dashes along the wire.
    void advance(int phase) override;

private:
    void applySelectionStyle();

    PinGraphicsItem* m_fromPin{nullptr};
    PinGraphicsItem* m_toPin{nullptr};
    PinId m_fromPinId;
    PinId m_toPinId;
    QColor m_signalColor;
    SignalValue m_signalValue{SignalValue::Unknown};
    bool m_simulated{false};
    qreal m_flowPhase{0.0};
};

} // namespace digitallogic::ui
