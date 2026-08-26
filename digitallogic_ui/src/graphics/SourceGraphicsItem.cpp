/**
 * @file SourceGraphicsItem.cpp
 * @brief Movable signal source block with toggleable value and output pin.
 * @author Seth Morgan
 * @date 2026-08-25
 */
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

/**
 * @brief Constructs a source graphics item for the given component id.
 */
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

/**
 * @brief Updates the displayed true/false value label.
 */
void SourceGraphicsItem::updateValueLabel(const SignalValue value)
{
    m_label->setPlainText(QString::fromUtf8(toString(value)));
}

/**
 * @brief Refreshes body and label appearance from the model signal value.
 */
void SourceGraphicsItem::refreshFromModel(const SignalValue value)
{
    updateValueLabel(value);
    m_body->setBrush(QBrush(signalColor(value, true)));
    if (m_outputPin != nullptr) {
        m_outputPin->setSignalValue(value, true);
    }
}

/**
 * @brief Sets the optional display label shown on the source.
 */
void SourceGraphicsItem::setDisplayLabel(const QString& label)
{
    if (m_title != nullptr) {
        m_title->setPlainText(label);
    }
}

/**
 * @brief Locks the source against toggle, move, and delete in challenge mode.
 */
void SourceGraphicsItem::setLocked(const bool locked)
{
    m_locked = locked;
    setFlag(QGraphicsItem::ItemIsMovable, !locked);
}

/**
 * @brief Clears simulation highlight styling from the source.
 */
void SourceGraphicsItem::clearSimulationHighlight()
{
    m_body->setBrush(QBrush(AppTheme::componentFill()));
    if (m_outputPin != nullptr) {
        m_outputPin->setSignalValue(SignalValue::Unknown, false);
    }
}

/**
 * @brief Toggles the source value on click when unlocked.
 */
void SourceGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // Defer to SandboxView while a wire drag is active.
    if (m_controller != nullptr && m_controller->isWireDragInProgress()) {
        event->ignore();
        return;
    }

    // Clicks on the output pin should start a wire, not toggle the source.
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

/**
 * @brief Recomputes attached wire endpoints after the source moves.
 */
void SourceGraphicsItem::updateWirePaths()
{
    if (m_controller != nullptr) {
        m_controller->updateAllWirePaths();
    }
}

/**
 * @brief Syncs model position when the item is moved on the canvas.
 */
QVariant SourceGraphicsItem::itemChange(const GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged || change == ItemScenePositionHasChanged) {
        updateWirePaths();
    }
    return QGraphicsItemGroup::itemChange(change, value);
}

} // namespace digitallogic::ui
