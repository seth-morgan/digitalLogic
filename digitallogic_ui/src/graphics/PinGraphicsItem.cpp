#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include "digitallogic/ui/CircuitController.h"
#include "digitallogic/ui/SignalColors.h"

#include <QBrush>
#include <QPen>

namespace digitallogic::ui {

namespace {
constexpr qreal kPinRadius = 9.0;
}

PinGraphicsItem::PinGraphicsItem(const PinId pinId, const PinRole role, CircuitController* controller, QGraphicsItem* parent)
    : QGraphicsEllipseItem(-kPinRadius, -kPinRadius, kPinRadius * 2.0, kPinRadius * 2.0, parent)
    , m_pinId(pinId)
    , m_role(role)
    , m_controller(controller)
{
    setBrush(QBrush(signalColor(SignalValue::Unknown, false)));
    setPen(QPen(Qt::black, 1.5));
    setZValue(10.0);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::NoButton);
}

QPointF PinGraphicsItem::sceneCenter() const
{
    return mapToScene(0.0, 0.0);
}

void PinGraphicsItem::setSignalValue(const SignalValue value, const bool simulated)
{
    m_value = value;
    m_simulated = simulated;
    setBrush(QBrush(signalColor(value, simulated)));
}

void PinGraphicsItem::setPendingWire(const bool pending)
{
    if (pending) {
        setPen(QPen(QColor(QStringLiteral("#2980b9")), 3.0));
    } else {
        setPen(QPen(Qt::black, 1.5));
    }
}

} // namespace digitallogic::ui
