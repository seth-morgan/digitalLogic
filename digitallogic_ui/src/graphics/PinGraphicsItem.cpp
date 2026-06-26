#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include "digitallogic/ui/CircuitController.h"
#include "digitallogic/ui/SignalColors.h"

#include <QBrush>
#include <QGraphicsSceneMouseEvent>
#include <QPen>

namespace digitallogic::ui {

namespace {
constexpr qreal kPinRadius = 6.0;
}

PinGraphicsItem::PinGraphicsItem(const PinId pinId, const PinRole role, CircuitController* controller, QGraphicsItem* parent)
    : QGraphicsEllipseItem(-kPinRadius, -kPinRadius, kPinRadius * 2.0, kPinRadius * 2.0, parent)
    , m_pinId(pinId)
    , m_role(role)
    , m_controller(controller)
{
    setBrush(QBrush(signalColor(SignalValue::Unknown, false)));
    setPen(QPen(Qt::black, 1.0));
    setZValue(2.0);
    setAcceptHoverEvents(true);
}

void PinGraphicsItem::setSignalValue(const SignalValue value, const bool simulated)
{
    m_value = value;
    m_simulated = simulated;
    setBrush(QBrush(signalColor(value, simulated)));
}

void PinGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_controller != nullptr) {
        m_controller->handlePinClicked(m_pinId, m_role);
    }
    QGraphicsEllipseItem::mousePressEvent(event);
}

} // namespace digitallogic::ui
