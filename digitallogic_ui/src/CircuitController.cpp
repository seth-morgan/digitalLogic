#include "digitallogic/ui/CircuitController.h"

#include "digitallogic/model/PinIndices.h"
#include "digitallogic/model/CircuitSerializer.h"
#include "digitallogic/model/WireValidation.h"
#include "digitallogic/ui/SandboxView.h"
#include "digitallogic/ui/graphics/GateGraphicsItem.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"
#include "digitallogic/ui/graphics/SourceGraphicsItem.h"
#include "digitallogic/ui/graphics/WireGraphicsItem.h"

#include <QFile>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QLineF>
#include <QPen>
#include <QtMath>

namespace digitallogic::ui {

namespace {

constexpr qreal kPinHitRadius = 14.0;

void collectPins(QGraphicsItem* item, QVector<PinGraphicsItem*>& pins)
{
    if (item == nullptr) {
        return;
    }

    if (auto* pin = dynamic_cast<PinGraphicsItem*>(item)) {
        pins.push_back(pin);
    }

    for (QGraphicsItem* child : item->childItems()) {
        collectPins(child, pins);
    }
}

} // namespace

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

void CircuitController::beginWireDrag(PinGraphicsItem* fromPin, const QPointF& scenePos)
{
    if (fromPin == nullptr || m_view == nullptr || m_view->scene() == nullptr) {
        return;
    }

    if (fromPin->role() == PinGraphicsItem::PinRole::GateInput) {
        return;
    }

    cancelWireDrag();
    m_wireDragFromPin = fromPin;
    m_wireDragInProgress = true;
    m_wireDragFromPin->setPendingWire(true);

    m_wirePreviewLine = new QGraphicsLineItem();
    m_wirePreviewLine->setPen(QPen(QColor(QStringLiteral("#2980b9")), 2.5, Qt::DashLine));
    m_wirePreviewLine->setZValue(5.0);
    m_view->scene()->addItem(m_wirePreviewLine);
    updatePreviewLine(scenePos);
}

void CircuitController::updateWireDrag(const QPointF& scenePos)
{
    if (!m_wireDragInProgress) {
        return;
    }

    updatePreviewLine(scenePos);
}

void CircuitController::updatePreviewLine(const QPointF& scenePos)
{
    if (m_wireDragFromPin == nullptr || m_wirePreviewLine == nullptr) {
        return;
    }

    const QPointF fromScene = m_wireDragFromPin->sceneCenter();
    m_wirePreviewLine->setLine(QLineF(fromScene, scenePos));
}

void CircuitController::endWireDrag(const QPointF& scenePos)
{
    if (!m_wireDragInProgress) {
        return;
    }

    PinGraphicsItem* targetPin = findPinAtScenePos(scenePos);
    if (targetPin != nullptr && m_wireDragFromPin != nullptr && targetPin != m_wireDragFromPin) {
        if (targetPin->role() == PinGraphicsItem::PinRole::GateInput) {
            (void)tryConnectPins(m_wireDragFromPin->pinId(), targetPin->pinId());
        } else {
            emit statusMessage(QString::fromUtf8(wireValidationMessage(WireValidationResult::InvalidDestination)));
        }
    }

    cancelWireDrag();
}

bool CircuitController::tryConnectPins(const PinId& from, const PinId& to)
{
    const WireValidationResult validation = m_circuit.validateWire(from, to);
    if (validation != WireValidationResult::Ok) {
        reportWireFailure(validation);
        return false;
    }

    if (!m_circuit.addWire(from, to)) {
        return false;
    }

    createWireItem(from, to);
    emit circuitChanged();
    emit statusMessage(QStringLiteral("Wire connected."));
    return true;
}

void CircuitController::reportWireFailure(const WireValidationResult result)
{
    emit statusMessage(QString::fromUtf8(wireValidationMessage(result)));
}

void CircuitController::cancelWireDrag()
{
    clearPendingWireHighlight();

    if (m_wirePreviewLine != nullptr && m_view != nullptr && m_view->scene() != nullptr) {
        m_view->scene()->removeItem(m_wirePreviewLine);
        delete m_wirePreviewLine;
        m_wirePreviewLine = nullptr;
    }

    m_wireDragFromPin = nullptr;
    m_wireDragInProgress = false;
}

void CircuitController::clearPendingWireHighlight()
{
    if (m_wireDragFromPin != nullptr) {
        m_wireDragFromPin->setPendingWire(false);
    }
}

void CircuitController::updateAllWirePaths()
{
    for (WireGraphicsItem* wireItem : m_wireItems) {
        if (wireItem != nullptr) {
            wireItem->updatePath();
        }
    }

    if (m_wireDragInProgress && m_wirePreviewLine != nullptr && m_wireDragFromPin != nullptr) {
        const QLineF line = m_wirePreviewLine->line();
        m_wirePreviewLine->setLine(QLineF(m_wireDragFromPin->sceneCenter(), line.p2()));
    }
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

    QVector<PinGraphicsItem*> pins;
    for (QGraphicsItem* item : m_view->scene()->items()) {
        collectPins(item, pins);
    }

    for (PinGraphicsItem* pin : pins) {
        if (pin->pinId() == pinId) {
            return pin;
        }
    }

    return nullptr;
}

void CircuitController::updateGatePosition(const ComponentId gateId, const QPointF& position)
{
    (void)m_circuit.setComponentPosition(gateId, position);
}

void CircuitController::deleteSelection()
{
    if (m_view == nullptr || m_view->scene() == nullptr) {
        return;
    }

    const QList<QGraphicsItem*> selected = m_view->scene()->selectedItems();
    if (selected.isEmpty()) {
        emit statusMessage(QStringLiteral("Select a gate or wire to delete."));
        return;
    }

    int removedCount = 0;
    for (QGraphicsItem* item : selected) {
        if (auto* wireItem = dynamic_cast<WireGraphicsItem*>(item)) {
            if (m_circuit.removeWire(wireItem->fromPinId(), wireItem->toPinId())) {
                removeWireItem(wireItem);
                ++removedCount;
            }
            continue;
        }

        if (auto* gateItem = dynamic_cast<GateGraphicsItem*>(item)) {
            if (m_circuit.removeGate(gateItem->componentId())) {
                removeGateItem(gateItem);
                ++removedCount;
            }
        }
    }

    if (removedCount > 0) {
        emit circuitChanged();
        emit statusMessage(QStringLiteral("Deleted %1 item(s).").arg(removedCount));
    }
}

void CircuitController::removeWireItem(WireGraphicsItem* wireItem)
{
    if (wireItem == nullptr || m_view == nullptr || m_view->scene() == nullptr) {
        return;
    }

    m_wireItems.removeAll(wireItem);
    m_view->scene()->removeItem(wireItem);
    delete wireItem;
}

void CircuitController::removeGateItem(GateGraphicsItem* gateItem)
{
    if (gateItem == nullptr || m_view == nullptr || m_view->scene() == nullptr) {
        return;
    }

    QVector<WireGraphicsItem*> wiresToRemove;
    for (WireGraphicsItem* wireItem : m_wireItems) {
        if (wireItem == nullptr) {
            continue;
        }
        if (wireItem->fromPinId().componentId == gateItem->componentId()
            || wireItem->toPinId().componentId == gateItem->componentId()) {
            wiresToRemove.push_back(wireItem);
        }
    }

    for (WireGraphicsItem* wireItem : wiresToRemove) {
        removeWireItem(wireItem);
    }

    m_view->scene()->removeItem(gateItem);
    delete gateItem;
}

void CircuitController::clearCanvas()
{
    if (m_view == nullptr || m_view->scene() == nullptr) {
        return;
    }

    cancelWireDrag();

    QVector<GateGraphicsItem*> gates;
    for (QGraphicsItem* item : m_view->scene()->items()) {
        if (auto* gate = dynamic_cast<GateGraphicsItem*>(item)) {
            gates.push_back(gate);
        }
    }

    for (GateGraphicsItem* gate : gates) {
        removeGateItem(gate);
    }

    while (!m_wireItems.isEmpty()) {
        removeWireItem(m_wireItems.front());
    }

    m_circuit.clearGatesAndWires();

    for (const SourceNode& source : m_circuit.sources()) {
        if (SourceGraphicsItem* sourceItem = findSourceItem(source.id())) {
            sourceItem->updateValueLabel(source.value());
            sourceItem->clearSimulationHighlight();
        }
    }

    emit circuitChanged();
    emit statusMessage(QStringLiteral("Canvas cleared."));
}

bool CircuitController::saveToFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        emit statusMessage(QStringLiteral("Could not save circuit file."));
        return false;
    }

    const QJsonDocument document(CircuitSerializer::toJson(m_circuit));
    if (file.write(document.toJson(QJsonDocument::Indented)) < 0) {
        emit statusMessage(QStringLiteral("Could not write circuit file."));
        return false;
    }

    emit statusMessage(QStringLiteral("Circuit saved."));
    return true;
}

bool CircuitController::loadFromFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        emit statusMessage(QStringLiteral("Could not open circuit file."));
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        emit statusMessage(QStringLiteral("Invalid circuit file."));
        return false;
    }

    QString error;
    std::optional<Circuit> loaded = CircuitSerializer::fromJson(document.object(), error);
    if (!loaded.has_value()) {
        emit statusMessage(error);
        return false;
    }

    removeAllGraphics();
    m_circuit = std::move(*loaded);
    rebuildGraphics();
    emit circuitChanged();
    emit statusMessage(QStringLiteral("Circuit loaded."));
    return true;
}

void CircuitController::removeAllGraphics()
{
    if (m_view == nullptr || m_view->scene() == nullptr) {
        return;
    }

    cancelWireDrag();

    QVector<QGraphicsItem*> removable;
    for (QGraphicsItem* item : m_view->scene()->items()) {
        if (dynamic_cast<SourceGraphicsItem*>(item) != nullptr || dynamic_cast<GateGraphicsItem*>(item) != nullptr
            || dynamic_cast<WireGraphicsItem*>(item) != nullptr) {
            removable.push_back(item);
        }
    }

    for (QGraphicsItem* item : removable) {
        m_view->scene()->removeItem(item);
        delete item;
    }

    m_wireItems.clear();
}

void CircuitController::rebuildGraphics()
{
    for (const SourceNode& source : m_circuit.sources()) {
        const ComponentPlacement placement = m_circuit.placements().value(source.id());
        createSourceItem(source.id(), placement.position);
    }

    for (const auto& gatePtr : m_circuit.gates()) {
        createGateItem(gatePtr->id(), gatePtr->kind(), gatePtr->inputCount(),
                       m_circuit.placements().value(gatePtr->id()).position);
    }

    for (const Wire& wire : m_circuit.wires()) {
        createWireItem(wire.from, wire.to);
    }
}

PinGraphicsItem* CircuitController::findPinAtScenePos(const QPointF& scenePos) const
{
    if (m_view == nullptr || m_view->scene() == nullptr) {
        return nullptr;
    }

    QVector<PinGraphicsItem*> pins;
    for (QGraphicsItem* item : m_view->scene()->items()) {
        collectPins(item, pins);
    }

    PinGraphicsItem* closestPin = nullptr;
    qreal closestDistance = kPinHitRadius;

    for (PinGraphicsItem* pin : pins) {
        const qreal distance = QLineF(pin->sceneCenter(), scenePos).length();
        if (distance <= closestDistance) {
            closestDistance = distance;
            closestPin = pin;
        }
    }

    return closestPin;
}

WireGraphicsItem* CircuitController::findWireAtScenePos(const QPointF& scenePos) const
{
    for (WireGraphicsItem* wireItem : m_wireItems) {
        if (wireItem != nullptr && wireItem->containsScenePoint(scenePos)) {
            return wireItem;
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

    for (WireGraphicsItem* wireItem : m_wireItems) {
        if (wireItem == nullptr) {
            continue;
        }

        const auto fromIt = pinValues.find(wireItem->fromPinId());
        if (fromIt != pinValues.end()) {
            wireItem->setSignalValue(fromIt.value(), true);
        }
        wireItem->updatePath();
    }

    for (const auto& gatePtr : m_circuit.gates()) {
        const PinId outputPinId{gatePtr->id(), gateOutputPinIndex(gatePtr->inputCount())};
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

    cancelWireDrag();

    for (WireGraphicsItem* wireItem : m_wireItems) {
        if (wireItem != nullptr) {
            wireItem->clearSimulationHighlight();
        }
    }

    QVector<PinGraphicsItem*> pins;
    for (QGraphicsItem* item : m_view->scene()->items()) {
        if (auto* gateItem = dynamic_cast<GateGraphicsItem*>(item)) {
            gateItem->clearSimulationHighlight();
        } else if (auto* sourceItem = dynamic_cast<SourceGraphicsItem*>(item)) {
            sourceItem->clearSimulationHighlight();
            if (const SourceNode* source = m_circuit.findSource(sourceItem->componentId())) {
                sourceItem->updateValueLabel(source->value());
            }
        }
        collectPins(item, pins);
    }

    for (PinGraphicsItem* pin : pins) {
        pin->setSignalValue(SignalValue::Unknown, false);
    }
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
    m_wireItems.push_back(wireItem);
    wireItem->updatePath();
}

} // namespace digitallogic::ui
