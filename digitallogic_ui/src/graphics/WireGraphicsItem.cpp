#include "digitallogic/ui/graphics/WireGraphicsItem.h"

#include "digitallogic/ui/SignalColors.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include <QPainter>
#include <QPainterPathStroker>
#include <QPen>
#include <QStyleOptionGraphicsItem>

namespace digitallogic::ui {

namespace {
constexpr qreal kWireWidth = 2.5;
constexpr qreal kWireHitWidth = 10.0;
}

WireGraphicsItem::WireGraphicsItem(PinGraphicsItem* fromPin, PinGraphicsItem* toPin, QGraphicsItem* parent)
    : QGraphicsLineItem(parent)
    , m_fromPin(fromPin)
    , m_toPin(toPin)
    , m_fromPinId(fromPin != nullptr ? fromPin->pinId() : PinId{})
    , m_toPinId(toPin != nullptr ? toPin->pinId() : PinId{})
{
    setZValue(1.0);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    updatePath();
    applySelectionStyle();
}

void WireGraphicsItem::updatePath()
{
    if (m_fromPin == nullptr || m_toPin == nullptr) {
        return;
    }

    const QPointF fromScene = m_fromPin->sceneCenter();
    const QPointF toScene = m_toPin->sceneCenter();
    prepareGeometryChange();
    setLine(QLineF(fromScene, toScene));
}

void WireGraphicsItem::setSignalValue(const SignalValue value, const bool simulated)
{
    m_signalColor = signalColor(value, simulated);
    m_simulated = simulated;
    applySelectionStyle();
}

void WireGraphicsItem::clearSimulationHighlight()
{
    m_signalColor = QColor(QStringLiteral("#808080"));
    m_simulated = false;
    applySelectionStyle();
}

bool WireGraphicsItem::containsScenePoint(const QPointF& scenePos) const
{
    return shape().contains(mapFromScene(scenePos));
}

void WireGraphicsItem::applySelectionStyle()
{
    QPen wirePen(m_signalColor, kWireWidth);
    if (isSelected()) {
        wirePen.setColor(QColor(QStringLiteral("#f39c12")));
        wirePen.setWidth(kWireWidth + 1.5);
    }
    prepareGeometryChange();
    setPen(wirePen);
}

void WireGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QStyleOptionGraphicsItem styleOption(*option);
    styleOption.state &= ~QStyle::State_Selected;
    QGraphicsLineItem::paint(painter, &styleOption, widget);
}

QPainterPath WireGraphicsItem::shape() const
{
    QPainterPath path;
    path.moveTo(line().p1());
    path.lineTo(line().p2());

    QPainterPathStroker stroker;
    stroker.setWidth(kWireHitWidth);
    stroker.setCapStyle(Qt::RoundCap);
    return stroker.createStroke(path);
}

QVariant WireGraphicsItem::itemChange(const GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSelectedHasChanged) {
        applySelectionStyle();
    }
    return QGraphicsLineItem::itemChange(change, value);
}

} // namespace digitallogic::ui
