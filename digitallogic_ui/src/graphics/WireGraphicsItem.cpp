#include "digitallogic/ui/graphics/WireGraphicsItem.h"

#include "digitallogic/ui/AppTheme.h"
#include "digitallogic/ui/SignalColors.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include <QtMath>
#include <QPainter>
#include <QPainterPathStroker>
#include <QPen>
#include <QStyleOptionGraphicsItem>

namespace digitallogic::ui {

namespace {
constexpr qreal kWireWidth = 3.0;
constexpr qreal kWireHitWidth = 10.0;
constexpr qreal kFlowDashLength = 10.0;
constexpr qreal kFlowGapLength = 8.0;
}

WireGraphicsItem::WireGraphicsItem(PinGraphicsItem* fromPin, PinGraphicsItem* toPin, QGraphicsItem* parent)
    : QGraphicsLineItem(parent)
    , m_fromPin(fromPin)
    , m_toPin(toPin)
    , m_fromPinId(fromPin != nullptr ? fromPin->pinId() : PinId{})
    , m_toPinId(toPin != nullptr ? toPin->pinId() : PinId{})
    , m_signalColor(AppTheme::wireIdle())
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
    m_signalValue = value;
    m_signalColor = signalColor(value, simulated);
    m_simulated = simulated;
    applySelectionStyle();
}

void WireGraphicsItem::clearSimulationHighlight()
{
    m_signalValue = SignalValue::Unknown;
    m_signalColor = AppTheme::wireIdle();
    m_simulated = false;
    m_flowPhase = 0.0;
    applySelectionStyle();
}

bool WireGraphicsItem::containsScenePoint(const QPointF& scenePos) const
{
    return shape().contains(mapFromScene(scenePos));
}

void WireGraphicsItem::applySelectionStyle()
{
    QPen wirePen(m_signalColor, kWireWidth, Qt::SolidLine, Qt::RoundCap);
    if (isSelected()) {
        wirePen.setColor(AppTheme::selection());
        wirePen.setWidth(kWireWidth + 1.5);
    }
    prepareGeometryChange();
    setPen(wirePen);
}

void WireGraphicsItem::advance(const int phase)
{
    if (phase != 0 || !m_simulated || m_signalValue == SignalValue::Unknown) {
        return;
    }

    const qreal speed = m_signalValue == SignalValue::True ? 1.4 : 0.7;
    m_flowPhase += speed;
    if (m_flowPhase > kFlowDashLength + kFlowGapLength) {
        m_flowPhase = 0.0;
    }
    update();
}

void WireGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QStyleOptionGraphicsItem styleOption(*option);
    styleOption.state &= ~QStyle::State_Selected;

    QPen basePen = pen();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(basePen);
    painter->drawLine(line());

    if (!m_simulated || m_signalValue == SignalValue::Unknown) {
        return;
    }

    QColor flowColor = m_signalColor.lighter(140);
    flowColor.setAlpha(230);

    QPen flowPen(flowColor, kWireWidth - 0.5, Qt::CustomDashLine, Qt::RoundCap);
    flowPen.setDashPattern({kFlowDashLength, kFlowGapLength});
    flowPen.setDashOffset(-m_flowPhase);
    painter->setPen(flowPen);
    painter->drawLine(line());
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
