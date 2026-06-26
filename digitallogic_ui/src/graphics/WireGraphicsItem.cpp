#include "digitallogic/ui/graphics/WireGraphicsItem.h"

#include "digitallogic/ui/SignalColors.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include <QPen>

namespace digitallogic::ui {

WireGraphicsItem::WireGraphicsItem(PinGraphicsItem* fromPin, PinGraphicsItem* toPin, QGraphicsItem* parent)
    : QGraphicsLineItem(parent)
    , m_fromPin(fromPin)
    , m_toPin(toPin)
    , m_fromPinId(fromPin != nullptr ? fromPin->pinId() : PinId{})
    , m_toPinId(toPin != nullptr ? toPin->pinId() : PinId{})
{
    setPen(QPen(QColor(QStringLiteral("#808080")), 2.5));
    setZValue(1.0);
    updatePath();
}

void WireGraphicsItem::updatePath()
{
    if (m_fromPin == nullptr || m_toPin == nullptr) {
        return;
    }

    const QPointF fromScene = m_fromPin->sceneBoundingRect().center();
    const QPointF toScene = m_toPin->sceneBoundingRect().center();
    setLine(QLineF(fromScene, toScene));
}

void WireGraphicsItem::setSignalValue(const SignalValue value, const bool simulated)
{
    setPen(QPen(signalColor(value, simulated), 2.5));
}

void WireGraphicsItem::clearSimulationHighlight()
{
    setPen(QPen(QColor(QStringLiteral("#808080")), 2.5));
}

} // namespace digitallogic::ui
