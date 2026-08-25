/**
 * @file CircuitController.cpp
 * @brief Owns the circuit model and syncs graphics for sources, gates, wires, targets, and challenges.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/ui/CircuitController.h"

#include "digitallogic/model/PinIndices.h"
#include "digitallogic/model/CircuitSerializer.h"
#include "digitallogic/model/WireValidation.h"
#include "digitallogic/ui/AppTheme.h"
#include "digitallogic/ui/SandboxView.h"
#include "digitallogic/ui/graphics/GateGraphicsItem.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"
#include "digitallogic/ui/graphics/SourceGraphicsItem.h"
#include "digitallogic/ui/graphics/TargetGraphicsItem.h"
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

// Recursively gather PinGraphicsItem children (pins live under source/gate/target groups).
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
    m_challengeMode = false;
    m_challengeTargetId = {};
    m_challengeSourceIdsByLabel.clear();
    clearChallengeGateBudget();

    const ComponentId sourceA = m_circuit.addSource(QPointF(80.0, 120.0), SignalValue::False);
    const ComponentId sourceB = m_circuit.addSource(QPointF(80.0, 240.0), SignalValue::False);
    createSourceItem(sourceA, QPointF(80.0, 120.0));
    createSourceItem(sourceB, QPointF(80.0, 240.0));
    emit circuitChanged();
}

void CircuitController::restoreSandboxMode()
{
    removeAllGraphics();
    m_circuit.clearAll();
    m_challengeMode = false;
    m_challengeTargetId = {};
    m_challengeSourceIdsByLabel.clear();
    clearChallengeGateBudget();
    initializeDefaultSources();
}

void CircuitController::loadChallengeLevel(const ChallengeLevel& level, QHash<QString, ComponentId>& outSourceIdsByLabel,
                                           ComponentId& outTargetId)
{
    removeAllGraphics();
    m_circuit.clearAll();
    m_challengeMode = true;
    m_challengeSourceIdsByLabel.clear();
    outSourceIdsByLabel.clear();

    // Challenge sources are locked in place; labels map to truth-table columns.
    for (const ChallengeSourceSpec& sourceSpec : level.sources) {
        const ComponentId sourceId = m_circuit.addSource(sourceSpec.position, SignalValue::False);
        m_challengeSourceIdsByLabel.insert(sourceSpec.label, sourceId);
        outSourceIdsByLabel.insert(sourceSpec.label, sourceId);
        createSourceItem(sourceId, sourceSpec.position, sourceSpec.label, true);
    }

    outTargetId = m_circuit.addTarget(level.targetPosition);
    m_challengeTargetId = outTargetId;
    createTargetItem(outTargetId, level.targetPosition);

    emit circuitChanged();
    emit statusMessage(level.description);
}

void CircuitController::setChallengeGateBudget(const QHash<GateKind, int>& remaining)
{
    m_challengeGateBudgetTotal = remaining;
    m_challengeGateBudget = remaining;
    emit gateBudgetChanged();
}

void CircuitController::clearChallengeGateBudget()
{
    m_challengeGateBudget.clear();
    m_challengeGateBudgetTotal.clear();
    emit gateBudgetChanged();
}

// Challenge sources and the OUT target cannot be deleted by the player.
bool CircuitController::isProtectedComponent(const ComponentId id) const
{
    if (!m_challengeMode) {
        return false;
    }

    if (id == m_challengeTargetId) {
        return true;
    }

    return m_challengeSourceIdsByLabel.values().contains(id);
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
    m_wirePreviewLine->setPen(QPen(AppTheme::gateAccent(GateKind::And), 2.5, Qt::DashLine));
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

    // Drop must land on a gate input or challenge target input pin.
    PinGraphicsItem* targetPin = findPinAtScenePos(scenePos);
    if (targetPin != nullptr && m_wireDragFromPin != nullptr && targetPin != m_wireDragFromPin) {
        if (targetPin->role() == PinGraphicsItem::PinRole::GateInput
            || targetPin->role() == PinGraphicsItem::PinRole::TargetInput) {
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

    // Keep the rubber-band preview anchored to the source pin while dragging.
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
        }
    }

    emit sourceValueChanged();
    emit circuitChanged();
}

bool CircuitController::placeGateFromPalette(const GateKind kind, const QPointF& scenePosition)
{
    if (m_challengeMode) {
        const int remaining = m_challengeGateBudget.value(kind, 0);
        if (remaining <= 0) {
            emit statusMessage(QStringLiteral("No more gates of this type are available for this level."));
            return false;
        }
    }

    const std::optional<ComponentId> gateId = m_circuit.addGate(kind, scenePosition);
    if (!gateId.has_value()) {
        return false;
    }

    int inputCount = 2;
    if (kind == GateKind::Not) {
        inputCount = 1;
    }

    createGateItem(gateId.value(), kind, inputCount, scenePosition);

    if (m_challengeMode && m_challengeGateBudget.contains(kind)) {
        m_challengeGateBudget[kind] -= 1;
        emit gateBudgetChanged();
    }

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
            if (isProtectedComponent(gateItem->componentId())) {
                continue;
            }
            if (m_circuit.removeGate(gateItem->componentId())) {
                // Refund the gate to the challenge budget when deleted.
                if (m_challengeMode) {
                    const GateKind kind = gateItem->kind();
                    if (m_challengeGateBudget.contains(kind)) {
                        m_challengeGateBudget[kind] += 1;
                    }
                }
                removeGateItem(gateItem);
                ++removedCount;
            }
        }
    }

    if (removedCount > 0) {
        if (m_challengeMode) {
            emit gateBudgetChanged();
        }
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

    // Removing a gate also removes any wires attached to its pins.
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

    // In challenge mode, sources/target stay; only gates and wires are cleared.
    m_circuit.clearGatesAndWires(m_challengeMode);

    for (const SourceNode& source : m_circuit.sources()) {
        if (SourceGraphicsItem* sourceItem = findSourceItem(source.id())) {
            sourceItem->updateValueLabel(source.value());
            sourceItem->clearSimulationHighlight();
        }
    }

    for (const TargetNode& target : m_circuit.targets()) {
        if (TargetGraphicsItem* targetItem = findTargetItem(target.id())) {
            targetItem->clearSimulationHighlight();
        }
    }

    if (m_challengeMode) {
        m_challengeGateBudget = m_challengeGateBudgetTotal;
        emit gateBudgetChanged();
    }

    emit circuitChanged();
    emit statusMessage(m_challengeMode ? QStringLiteral("Level reset.")
                                     : QStringLiteral("Canvas cleared."));
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
            || dynamic_cast<TargetGraphicsItem*>(item) != nullptr || dynamic_cast<WireGraphicsItem*>(item) != nullptr) {
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

    for (const TargetNode& target : m_circuit.targets()) {
        const ComponentPlacement placement = m_circuit.placements().value(target.id());
        createTargetItem(target.id(), placement.position);
    }

    for (const auto& gatePtr : m_circuit.gates()) {
        createGateItem(gatePtr->id(), gatePtr->kind(), gatePtr->inputCount(),
                       m_circuit.placements().value(gatePtr->id()).position);
    }

    for (const Wire& wire : m_circuit.wires()) {
        createWireItem(wire.from, wire.to);
    }
}

// Prefer the closest pin within kPinHitRadius for forgiving wire drop targeting.
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

TargetGraphicsItem* CircuitController::findTargetItem(const ComponentId id) const
{
    if (m_view == nullptr || m_view->scene() == nullptr) {
        return nullptr;
    }

    for (QGraphicsItem* item : m_view->scene()->items()) {
        if (auto* target = dynamic_cast<TargetGraphicsItem*>(item)) {
            if (target->componentId() == id) {
                return target;
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

        // Wire color follows the driving (from) pin's resolved value.
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

    for (const TargetNode& target : m_circuit.targets()) {
        const PinId targetInput{target.id(), targetInputPinIndex()};
        const auto targetIt = pinValues.find(targetInput);
        if (targetIt == pinValues.end()) {
            continue;
        }
        if (TargetGraphicsItem* targetItem = findTargetItem(target.id())) {
            targetItem->setInputSignal(targetIt.value(), true);
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
        } else if (auto* targetItem = dynamic_cast<TargetGraphicsItem*>(item)) {
            targetItem->clearSimulationHighlight();
        }
        collectPins(item, pins);
    }

    for (PinGraphicsItem* pin : pins) {
        pin->setSignalValue(SignalValue::Unknown, false);
    }
}

void CircuitController::createSourceItem(const ComponentId id, const QPointF& position, const QString& label,
                                         const bool locked)
{
    auto* sourceItem = new SourceGraphicsItem(id, this);
    if (!label.isEmpty()) {
        sourceItem->setDisplayLabel(label);
    }
    sourceItem->setLocked(locked);
    sourceItem->setPos(position);
    m_view->scene()->addItem(sourceItem);

    if (const SourceNode* source = m_circuit.findSource(id)) {
        sourceItem->updateValueLabel(source->value());
        sourceItem->clearSimulationHighlight();
    }
}

void CircuitController::createTargetItem(const ComponentId id, const QPointF& position)
{
    auto* targetItem = new TargetGraphicsItem(id, this);
    targetItem->setPos(position);
    m_view->scene()->addItem(targetItem);
    targetItem->clearSimulationHighlight();
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
