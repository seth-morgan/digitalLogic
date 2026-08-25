/**
 * @file TargetGraphicsItem.cpp
 * @brief Fixed challenge OUT target with input pin and simulated value display.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/ui/graphics/TargetGraphicsItem.h"

#include "digitallogic/ui/AppTheme.h"
#include "digitallogic/ui/CircuitController.h"
#include "digitallogic/ui/SignalColors.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include <QBrush>
#include <QFont>
#include <QPen>

namespace digitallogic::ui {

TargetGraphicsItem::TargetGraphicsItem(const ComponentId componentId, CircuitController* controller, QGraphicsItem* parent)
    : QGraphicsItemGroup(parent)
    , m_componentId(componentId)
    , m_controller(controller)
{
    m_body = new QGraphicsRectItem(0.0, 0.0, 90.0, 50.0, this);
    m_body->setBrush(QBrush(AppTheme::componentFill()));
    m_body->setPen(QPen(AppTheme::targetAccent(), 2.0));

    auto* title = new QGraphicsTextItem(QStringLiteral("OUT"), this);
    title->setPos(10.0, 4.0);
    title->setDefaultTextColor(AppTheme::targetAccent());

    m_valueLabel = new QGraphicsTextItem(QStringLiteral("-"), this);
    m_valueLabel->setFont(QFont(QStringLiteral("Arial"), 14, QFont::Bold));
    m_valueLabel->setDefaultTextColor(AppTheme::textPrimary());
    m_valueLabel->setPos(34.0, 20.0);

    m_inputPin = new PinGraphicsItem(PinId{componentId, targetInputPinIndex()}, PinGraphicsItem::PinRole::TargetInput,
                                     controller, this);
    m_inputPin->setPos(-8.0, 25.0);

    // Targets are placement-locked; only challenge setup positions them.
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
}

void TargetGraphicsItem::setInputSignal(const SignalValue value, const bool simulated)
{
    m_valueLabel->setPlainText(QString::fromUtf8(toString(value)));
    m_body->setBrush(QBrush(signalColor(value, simulated)));
    if (m_inputPin != nullptr) {
        m_inputPin->setSignalValue(value, simulated);
    }
}

void TargetGraphicsItem::clearSimulationHighlight()
{
    m_body->setBrush(QBrush(AppTheme::componentFill()));
    m_valueLabel->setPlainText(QStringLiteral("-"));
    if (m_inputPin != nullptr) {
        m_inputPin->setSignalValue(SignalValue::Unknown, false);
    }
}

void TargetGraphicsItem::updateWirePaths()
{
    if (m_controller != nullptr) {
        m_controller->updateAllWirePaths();
    }
}

QVariant TargetGraphicsItem::itemChange(const GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged || change == ItemScenePositionHasChanged) {
        updateWirePaths();
    }
    return QGraphicsItemGroup::itemChange(change, value);
}

} // namespace digitallogic::ui
