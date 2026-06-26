#include "digitallogic/ui/CircuitController.h"

#include "digitallogic/ui/SandboxView.h"
#include "digitallogic/ui/graphics/GateGraphicsItem.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"
#include "digitallogic/ui/graphics/SourceGraphicsItem.h"
#include "digitallogic/ui/graphics/WireGraphicsItem.h"

namespace digitallogic::ui {

CircuitController::CircuitController(SandboxView* view, QObject* parent)
    : QObject(parent)
    , m_view(view)
{
}

void CircuitController::initializeDefaultSources()
{
    const ComponentId sourceA = m_circuit.addSource(QPointF(80.0, 120.0), SignalValue::False);
    const ComponentId sourceB = m_circuit.addSource(QPointF(80.0, 240.0), SignalValue::False);
    createSourceItem(sourceA, QPointF(80.0, 120.0));
    createSourceItem(sourceB, QPointF(80.0, 240.0));
    emit circuitChanged();
}

void CircuitController::handlePinClicked(const PinId& pinId, const PinGraphicsItem::PinRole role)
{
    if (role == PinGraphicsItem::PinRole::GateInput) {
        if (!m_pendingWireFrom.has_value()) {
            return;
        }

        const PinId from = m_pendingWireFrom.value();
        if (m_circuit.addWire(from, pinId)) {
            createWireItem(from, pinId);
            emit circuitChanged();
        }
        m_pendingWireFrom.reset();
        return;
    }

    m_pendingWireFrom = pinId;
}

void CircuitController::toggleSource(const ComponentId sourceId)
{
    if (!m_circuit.toggleSource(sourceId)) {
        return;
    }

    if (SourceGraphicsItem* item = findSourceItem(sourceId)) {
        if (const SourceNode* source = m_circuit.findSource(sourceId)) {
            item->updateValueLabel(source->value());
            item->clearSimulationHighlight();
        }
    }

    emit circuitChanged();
}

bool CircuitController::placeGateFromPalette(const GateKind kind, const QPointF& scenePosition)
{
    const std::optional<ComponentId> gateId = m_circuit.addGate(kind, scenePosition);
    if (!gateId.has_value()) {
        return false;
    }

    int inputCount = 2;
    if (kind == GateKind::Not) {
        inputCount = 1;
    }

    createGateItem(gateId.value(), kind, inputCount, scenePosition);
    emit circuitChanged();
    return true;
}

PinGraphicsItem* CircuitController::findPin(const PinId& pinId) const
{
    if (m_view == nullptr || m_view->scene() == nullptr) {
        return nullptr;
    }

    const auto items = m_view->scene()->items();
    for (QGraphicsItem* item : items) {
        if (auto* pin = dynamic_cast<PinGraphicsItem*>(item)) {
            if (pin->pinId() == pinId) {
                return pin;
            }
        }
    }

    return nullptr;
}

SourceGraphicsItem* CircuitController::findSourceItem(const ComponentId id) const
{
    if (m_view == nullptr || m_view->scene() == nullptr) {
        return nullptr;
    }

    for (QGraphicsItem* item : m_view->scene()->items()) {
        if (auto* source = dynamic_cast<SourceGraphicsItem*>(item)) {
            if (source->componentId() == id) {
                return source;
            }
        }
    }

    return nullptr;
}

GateGraphicsItem* CircuitController::findGateItem(const ComponentId id) const
{
    if (m_view == nullptr || m_view->scene() == nullptr) {
        return nullptr;
    }

    for (QGraphicsItem* item : m_view->scene()->items()) {
        if (auto* gate = dynamic_cast<GateGraphicsItem*>(item)) {
            if (gate->componentId() == id) {
                return gate;
            }
        }
    }

    return nullptr;
}

void CircuitController::applySimulationResults(const QHash<PinId, SignalValue>& pinValues)
{
    for (auto it = pinValues.begin(); it != pinValues.end(); ++it) {
        if (PinGraphicsItem* pin = findPin(it.key())) {
            pin->setSignalValue(it.value(), true);
        }
    }

    for (const Wire& wire : m_circuit.wires()) {
        for (QGraphicsItem* item : m_view->scene()->items()) {
            if (auto* wireItem = dynamic_cast<WireGraphicsItem*>(item)) {
                if (wireItem->fromPinId() == wire.from && wireItem->toPinId() == wire.to) {
                    const auto fromIt = pinValues.find(wire.from);
                    if (fromIt != pinValues.end()) {
                        wireItem->setSignalValue(fromIt.value(), true);
                    }
                    wireItem->updatePath();
                }
            }
        }
    }

    for (const auto& gatePtr : m_circuit.gates()) {
        const PinId outputPinId{gatePtr->id(), 0};
        const auto outputIt = pinValues.find(outputPinId);
        if (outputIt == pinValues.end()) {
            continue;
        }

        if (GateGraphicsItem* gateItem = findGateItem(gatePtr->id())) {
            gateItem->setOutputSignal(outputIt.value(), true);
        }

        for (int inputIndex = 0; inputIndex < gatePtr->inputCount(); ++inputIndex) {
            const PinId inputPinId{gatePtr->id(), inputIndex};
            if (PinGraphicsItem* inputPin = findPin(inputPinId)) {
                const auto inputIt = pinValues.find(inputPinId);
                if (inputIt != pinValues.end()) {
                    inputPin->setSignalValue(inputIt.value(), true);
                }
            }
        }
    }

    for (const SourceNode& source : m_circuit.sources()) {
        if (SourceGraphicsItem* sourceItem = findSourceItem(source.id())) {
            sourceItem->refreshFromModel(source.value());
        }
    }
}

void CircuitController::clearSimulationVisuals()
{
    if (m_view == nullptr || m_view->scene() == nullptr) {
        return;
    }

    for (QGraphicsItem* item : m_view->scene()->items()) {
        if (auto* wireItem = dynamic_cast<WireGraphicsItem*>(item)) {
            wireItem->clearSimulationHighlight();
        } else if (auto* gateItem = dynamic_cast<GateGraphicsItem*>(item)) {
            gateItem->clearSimulationHighlight();
        } else if (auto* sourceItem = dynamic_cast<SourceGraphicsItem*>(item)) {
            sourceItem->clearSimulationHighlight();
            if (const SourceNode* source = m_circuit.findSource(sourceItem->componentId())) {
                sourceItem->updateValueLabel(source->value());
            }
        } else if (auto* pinItem = dynamic_cast<PinGraphicsItem*>(item)) {
            pinItem->setSignalValue(SignalValue::Unknown, false);
        }
    }

    m_pendingWireFrom.reset();
}

void CircuitController::createSourceItem(const ComponentId id, const QPointF& position)
{
    auto* sourceItem = new SourceGraphicsItem(id, this);
    sourceItem->setPos(position);
    m_view->scene()->addItem(sourceItem);

    if (const SourceNode* source = m_circuit.findSource(id)) {
        sourceItem->updateValueLabel(source->value());
        sourceItem->clearSimulationHighlight();
    }
}

void CircuitController::createGateItem(const ComponentId id, const GateKind kind, const int inputCount, const QPointF& position)
{
    auto* gateItem = new GateGraphicsItem(id, kind, inputCount, this);
    gateItem->setPos(position);
    m_view->scene()->addItem(gateItem);
}

void CircuitController::createWireItem(const PinId& from, const PinId& to)
{
    PinGraphicsItem* fromPin = findPin(from);
    PinGraphicsItem* toPin = findPin(to);
    if (fromPin == nullptr || toPin == nullptr) {
        return;
    }

    auto* wireItem = new WireGraphicsItem(fromPin, toPin);
    m_view->scene()->addItem(wireItem);
    wireItem->updatePath();
}

} // namespace digitallogic::ui
