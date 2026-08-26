/**
 * @file GateGraphicsItem.cpp
 * @brief Movable logic-gate block with input/output pins and selection styling.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/ui/graphics/GateGraphicsItem.h"

#include "digitallogic/ui/AppTheme.h"
#include "digitallogic/model/PinIndices.h"
#include "digitallogic/ui/CircuitController.h"
#include "digitallogic/ui/SignalColors.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include <QBrush>
#include <QFont>
#include <QGraphicsTextItem>
#include <QPen>

namespace digitallogic::ui {

/**
 * @brief Constructs a gate graphics item with the given kind and input count.
 */
GateGraphicsItem::GateGraphicsItem(const ComponentId componentId, const GateKind kind, const int inputCount,
                                   CircuitController* controller, QGraphicsItem* parent)
    : QGraphicsItemGroup(parent)
    , m_componentId(componentId)
    , m_kind(kind)
    , m_controller(controller)
{
    const double bodyWidth = 90.0;
    const double bodyHeight = 50.0;

    m_body = new QGraphicsRectItem(0.0, 0.0, bodyWidth, bodyHeight, this);
    m_body->setBrush(QBrush(AppTheme::componentFill()));
    m_body->setPen(QPen(AppTheme::componentBorder(), 1.5));

    auto* label = new QGraphicsTextItem(gateLabel(kind), this);
    label->setFont(QFont(QStringLiteral("Arial"), 12, QFont::Bold));
    label->setDefaultTextColor(AppTheme::textPrimary());
    label->setPos(18.0, 16.0);

    // Space input pins evenly along the left edge (NOT has a single input).
    const double inputSpacing = bodyHeight / static_cast<double>(inputCount + 1);
    for (int inputIndex = 0; inputIndex < inputCount; ++inputIndex) {
        auto* inputPin = new PinGraphicsItem(PinId{componentId, inputIndex}, PinGraphicsItem::PinRole::GateInput, this);
        inputPin->setPos(-8.0, inputSpacing * static_cast<double>(inputIndex + 1));
        m_inputPins.push_back(inputPin);
    }

    m_outputPin = new PinGraphicsItem(PinId{componentId, gateOutputPinIndex(inputCount)}, PinGraphicsItem::PinRole::GateOutput,
                                      this);
    m_outputPin->setPos(bodyWidth + 8.0, bodyHeight / 2.0);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    applySelectionStyle();
}

/**
 * @brief Applies selected/deselected border styling to the gate body.
 */
void GateGraphicsItem::applySelectionStyle()
{
    if (m_body == nullptr) {
        return;
    }

    if (isSelected()) {
        m_body->setPen(QPen(AppTheme::selection(), 3.0));
    } else {
        m_body->setPen(QPen(AppTheme::componentBorder(), 1.5));
    }
}

/**
 * @brief Returns the short display label for a gate kind.
 */
QString GateGraphicsItem::gateLabel(const GateKind kind)
{
    switch (kind) {
    case GateKind::And:
        return QStringLiteral("AND");
    case GateKind::Or:
        return QStringLiteral("OR");
    case GateKind::Not:
        return QStringLiteral("NOT");
    case GateKind::Nand:
        return QStringLiteral("NAND");
    case GateKind::Xor:
        return QStringLiteral("XOR");
    default:
        return QStringLiteral("?");
    }
}

/**
 * @brief Updates the output pin appearance from a simulated signal value.
 */
void GateGraphicsItem::setOutputSignal(const SignalValue value, const bool simulated)
{
    m_body->setBrush(QBrush(signalColor(value, simulated)));
    if (m_outputPin != nullptr) {
        m_outputPin->setSignalValue(value, simulated);
    }
}

/**
 * @brief Clears simulation highlight styling from the gate output.
 */
void GateGraphicsItem::clearSimulationHighlight()
{
    m_body->setBrush(QBrush(AppTheme::componentFill()));
    if (m_outputPin != nullptr) {
        m_outputPin->setSignalValue(SignalValue::Unknown, false);
    }
    for (PinGraphicsItem* inputPin : m_inputPins) {
        if (inputPin != nullptr) {
            inputPin->setSignalValue(SignalValue::Unknown, false);
        }
    }
}

/**
 * @brief Recomputes attached wire endpoints after the gate is moved.
 */
void GateGraphicsItem::updateWirePaths()
{
    if (m_controller != nullptr) {
        m_controller->updateAllWirePaths();
    }
}

/**
 * @brief Syncs model position and selection style when the item changes.
 */
QVariant GateGraphicsItem::itemChange(const GraphicsItemChange change, const QVariant& value)
{
    // Persist model position and redraw attached wires when the gate is moved.
    if (change == ItemPositionHasChanged || change == ItemScenePositionHasChanged) {
        updateWirePaths();
        if (m_controller != nullptr) {
            m_controller->updateGatePosition(m_componentId, pos());
        }
    }
    if (change == ItemSelectedHasChanged) {
        applySelectionStyle();
    }
    return QGraphicsItemGroup::itemChange(change, value);
}

} // namespace digitallogic::ui
