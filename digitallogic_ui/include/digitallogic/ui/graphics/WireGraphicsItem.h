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
    /**
     * @brief Constructs a wire between the given from and to pins.
     */
    WireGraphicsItem(PinGraphicsItem* fromPin, PinGraphicsItem* toPin, QGraphicsItem* parent = nullptr);

    /**
     * @brief Returns the source pin id of this wire.
     */
    [[nodiscard]] PinId fromPinId() const noexcept { return m_fromPinId; }
    /**
     * @brief Returns the destination pin id of this wire.
     */
    [[nodiscard]] PinId toPinId() const noexcept { return m_toPinId; }

    /**
     * @brief Recomputes the wire line endpoints from the connected pins.
     */
    void updatePath();
    /**
     * @brief Updates wire color from a simulated signal value.
     */
    void setSignalValue(SignalValue value, bool simulated);
    /**
     * @brief Clears simulation coloring from the wire.
     */
    void clearSimulationHighlight();
    /**
     * @brief Returns whether the scene point hits this wire within tolerance.
     */
    // Hit-test with a wider tolerance than the thin painted stroke.
    [[nodiscard]] bool containsScenePoint(const QPointF& scenePos) const;

protected:
    /**
     * @brief Paints the wire stroke and optional animated signal flow.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    /**
     * @brief Returns a widened shape used for selection and hit testing.
     */
    QPainterPath shape() const override;
    /**
     * @brief Updates selection styling when the item's state changes.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    /**
     * @brief Advances the animated signal-flow dashes along the wire.
     */
    // Drives the animated signal-flow dashes along the wire.
    void advance(int phase) override;

private:
    /**
     * @brief Applies selected/deselected stroke styling to the wire.
     */
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
