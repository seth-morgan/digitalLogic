#include "digitallogic/ui/graphics/SourceGraphicsItem.h"

#include "digitallogic/ui/AppTheme.h"
#include "digitallogic/ui/CircuitController.h"
#include "digitallogic/ui/SignalColors.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include <QBrush>
#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QPen>

namespace digitallogic::ui {

SourceGraphicsItem::SourceGraphicsItem(const ComponentId componentId, CircuitController* controller, QGraphicsItem* parent)
    : QGraphicsItemGroup(parent)
    , m_componentId(componentId)
    , m_controller(controller)
{
    m_body = new QGraphicsRectItem(0.0, 0.0, 70.0, 40.0, this);
    m_body->setBrush(QBrush(AppTheme::componentFill()));
    m_body->setPen(QPen(AppTheme::componentBorder(), 1.5));

    auto* title = new QGraphicsTextItem(QStringLiteral("SRC"), this);
    title->setPos(8.0, 4.0);
    title->setDefaultTextColor(AppTheme::textMuted());
    m_title = title;

    m_label = new QGraphicsTextItem(QStringLiteral("0"), this);
    m_label->setFont(QFont(QStringLiteral("Arial"), 14, QFont::Bold));
    m_label->setDefaultTextColor(AppTheme::textPrimary());
    m_label->setPos(26.0, 16.0);

    m_outputPin = new PinGraphicsItem(PinId{componentId, sourceOutputPinIndex()}, PinGraphicsItem::PinRole::SourceOutput,
                                      controller, this);
    m_outputPin->setPos(78.0, 20.0);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void SourceGraphicsItem::updateValueLabel(const SignalValue value)
{
    m_label->setPlainText(QString::fromUtf8(toString(value)));
}

void SourceGraphicsItem::refreshFromModel(const SignalValue value)
{
    updateValueLabel(value);
    m_body->setBrush(QBrush(signalColor(value, true)));
    if (m_outputPin != nullptr) {
        m_outputPin->setSignalValue(value, true);
    }
}

void SourceGraphicsItem::setDisplayLabel(const QString& label)
{
    if (m_title != nullptr) {
        m_title->setPlainText(label);
    }
}

void SourceGraphicsItem::setLocked(const bool locked)
{
    m_locked = locked;
    setFlag(QGraphicsItem::ItemIsMovable, !locked);
}

void SourceGraphicsItem::clearSimulationHighlight()
{
    m_body->setBrush(QBrush(AppTheme::componentFill()));
    if (m_outputPin != nullptr) {
        m_outputPin->setSignalValue(SignalValue::Unknown, false);
    }
}

void SourceGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_controller != nullptr && m_controller->isWireDragInProgress()) {
        event->ignore();
        return;
    }

    if (m_outputPin != nullptr) {
        const QPointF pinLocal = m_outputPin->mapFromScene(event->scenePos());
        if (m_outputPin->contains(pinLocal)) {
            event->ignore();
            return;
        }
    }

    if (m_controller != nullptr) {
        m_controller->toggleSource(m_componentId);
    }
    QGraphicsItemGroup::mousePressEvent(event);
}

void SourceGraphicsItem::updateWirePaths()
{
    if (m_controller != nullptr) {
        m_controller->updateAllWirePaths();
    }
}

QVariant SourceGraphicsItem::itemChange(const GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged || change == ItemScenePositionHasChanged) {
        updateWirePaths();
    }
    return QGraphicsItemGroup::itemChange(change, value);
}

} // namespace digitallogic::ui
