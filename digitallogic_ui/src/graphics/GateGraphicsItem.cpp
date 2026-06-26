#include "digitallogic/ui/graphics/GateGraphicsItem.h"

#include "digitallogic/ui/CircuitController.h"
#include "digitallogic/ui/SignalColors.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include <QBrush>
#include <QFont>
#include <QGraphicsTextItem>
#include <QPen>

namespace digitallogic::ui {

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
    m_body->setBrush(QBrush(QColor(QStringLiteral("#ecf0f1"))));
    m_body->setPen(QPen(Qt::black, 1.5));

    auto* label = new QGraphicsTextItem(gateLabel(kind), this);
    label->setFont(QFont(QStringLiteral("Arial"), 12, QFont::Bold));
    label->setDefaultTextColor(Qt::black);
    label->setPos(18.0, 16.0);

    const double inputSpacing = bodyHeight / static_cast<double>(inputCount + 1);
    for (int inputIndex = 0; inputIndex < inputCount; ++inputIndex) {
        auto* inputPin = new PinGraphicsItem(PinId{componentId, inputIndex}, PinGraphicsItem::PinRole::GateInput,
                                               controller, this);
        inputPin->setPos(-8.0, inputSpacing * static_cast<double>(inputIndex + 1));
        m_inputPins.push_back(inputPin);
    }

    m_outputPin = new PinGraphicsItem(PinId{componentId, 0}, PinGraphicsItem::PinRole::GateOutput, controller, this);
    m_outputPin->setPos(bodyWidth + 8.0, bodyHeight / 2.0);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QString GateGraphicsItem::gateLabel(const GateKind kind)
{
    switch (kind) {
    case GateKind::And:
        return QStringLiteral("AND");
    case GateKind::Or:
        return QStringLiteral("OR");
    case GateKind::Not:
        return QStringLiteral("NOT");
    default:
        return QStringLiteral("?");
    }
}

void GateGraphicsItem::setOutputSignal(const SignalValue value, const bool simulated)
{
    m_body->setBrush(QBrush(signalColor(value, simulated)));
    if (m_outputPin != nullptr) {
        m_outputPin->setSignalValue(value, simulated);
    }
}

void GateGraphicsItem::clearSimulationHighlight()
{
    m_body->setBrush(QBrush(QColor(QStringLiteral("#ecf0f1"))));
    if (m_outputPin != nullptr) {
        m_outputPin->setSignalValue(SignalValue::Unknown, false);
    }
    for (PinGraphicsItem* inputPin : m_inputPins) {
        if (inputPin != nullptr) {
            inputPin->setSignalValue(SignalValue::Unknown, false);
        }
    }
}

} // namespace digitallogic::ui
