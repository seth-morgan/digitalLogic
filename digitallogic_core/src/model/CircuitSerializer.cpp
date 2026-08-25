/**
 * @file CircuitSerializer.cpp
 * @brief Serializes and deserializes Circuit objects to versioned JSON.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/model/CircuitSerializer.h"

#include "digitallogic/model/ComponentIdFactory.h"
#include "digitallogic/model/PinIndices.h"

#include <QJsonArray>
#include <QJsonObject>

namespace digitallogic {

namespace {

constexpr int kFormatVersion = 1; // Bump when the on-disk schema changes incompatibly.

[[nodiscard]] QString gateKindToString(const GateKind kind)
{
    switch (kind) {
    case GateKind::And:
        return QStringLiteral("and");
    case GateKind::Or:
        return QStringLiteral("or");
    case GateKind::Not:
        return QStringLiteral("not");
    case GateKind::Nand:
        return QStringLiteral("nand");
    case GateKind::Xor:
        return QStringLiteral("xor");
    default:
        return QStringLiteral("unknown");
    }
}

[[nodiscard]] std::optional<GateKind> gateKindFromString(const QString& text)
{
    if (text == QStringLiteral("and")) {
        return GateKind::And;
    }
    if (text == QStringLiteral("or")) {
        return GateKind::Or;
    }
    if (text == QStringLiteral("not")) {
        return GateKind::Not;
    }
    if (text == QStringLiteral("nand")) {
        return GateKind::Nand;
    }
    if (text == QStringLiteral("xor")) {
        return GateKind::Xor;
    }
    return std::nullopt;
}

[[nodiscard]] QJsonObject pinToJson(const PinId& pin)
{
    QJsonObject object;
    object.insert(QStringLiteral("componentId"), static_cast<qint64>(pin.componentId.value));
    object.insert(QStringLiteral("pinIndex"), pin.pinIndex);
    return object;
}

[[nodiscard]] std::optional<PinId> pinFromJson(const QJsonObject& object)
{
    if (!object.contains(QStringLiteral("componentId")) || !object.contains(QStringLiteral("pinIndex"))) {
        return std::nullopt;
    }

    PinId pin;
    pin.componentId = ComponentId{static_cast<std::uint64_t>(object.value(QStringLiteral("componentId")).toInteger())};
    pin.pinIndex = object.value(QStringLiteral("pinIndex")).toInt();
    return pin;
}

[[nodiscard]] SignalValue signalFromJson(const QJsonValue& value)
{
    return value.toInt() != 0 ? SignalValue::True : SignalValue::False;
}

} // namespace

QJsonObject CircuitSerializer::toJson(const Circuit& circuit)
{
    QJsonObject root;
    root.insert(QStringLiteral("version"), kFormatVersion);

    QJsonArray sources;
    for (const SourceNode& source : circuit.sources()) {
        const ComponentPlacement placement = circuit.placements().value(source.id());
        QJsonObject entry;
        entry.insert(QStringLiteral("id"), static_cast<qint64>(source.id().value));
        entry.insert(QStringLiteral("x"), placement.position.x());
        entry.insert(QStringLiteral("y"), placement.position.y());
        entry.insert(QStringLiteral("value"), static_cast<int>(source.value()));
        sources.append(entry);
    }
    root.insert(QStringLiteral("sources"), sources);

    QJsonArray gates;
    for (const auto& gatePtr : circuit.gates()) {
        const ComponentPlacement placement = circuit.placements().value(gatePtr->id());
        QJsonObject entry;
        entry.insert(QStringLiteral("id"), static_cast<qint64>(gatePtr->id().value));
        entry.insert(QStringLiteral("kind"), gateKindToString(gatePtr->kind()));
        entry.insert(QStringLiteral("x"), placement.position.x());
        entry.insert(QStringLiteral("y"), placement.position.y());
        gates.append(entry);
    }
    root.insert(QStringLiteral("gates"), gates);

    QJsonArray wires;
    for (const Wire& wire : circuit.wires()) {
        QJsonObject entry;
        entry.insert(QStringLiteral("from"), pinToJson(wire.from));
        entry.insert(QStringLiteral("to"), pinToJson(wire.to));
        wires.append(entry);
    }
    root.insert(QStringLiteral("wires"), wires);

    return root;
}

std::optional<Circuit> CircuitSerializer::fromJson(const QJsonObject& json, QString& error)
{
    // Reject files that lack a matching version field.
    if (!json.contains(QStringLiteral("version")) || json.value(QStringLiteral("version")).toInt() != kFormatVersion) {
        error = QStringLiteral("Unsupported circuit file format.");
        return std::nullopt;
    }

    Circuit circuit;
    std::uint64_t maxId = 0; // Track highest id so the generator can resume past it.

    const QJsonArray sources = json.value(QStringLiteral("sources")).toArray();
    for (const QJsonValue& value : sources) {
        const QJsonObject entry = value.toObject();
        const ComponentId id{static_cast<std::uint64_t>(entry.value(QStringLiteral("id")).toInteger())};
        maxId = std::max(maxId, id.value);

        const QPointF position(entry.value(QStringLiteral("x")).toDouble(), entry.value(QStringLiteral("y")).toDouble());
        const SignalValue signal = signalFromJson(entry.value(QStringLiteral("value")));
        if (!circuit.addSourceWithId(id, position, signal)) {
            error = QStringLiteral("Duplicate source id in circuit file.");
            return std::nullopt;
        }
    }

    const QJsonArray gates = json.value(QStringLiteral("gates")).toArray();
    for (const QJsonValue& value : gates) {
        const QJsonObject entry = value.toObject();
        const ComponentId id{static_cast<std::uint64_t>(entry.value(QStringLiteral("id")).toInteger())};
        maxId = std::max(maxId, id.value);

        const std::optional<GateKind> kind = gateKindFromString(entry.value(QStringLiteral("kind")).toString());
        if (!kind.has_value()) {
            error = QStringLiteral("Unknown gate kind in circuit file.");
            return std::nullopt;
        }

        const QPointF position(entry.value(QStringLiteral("x")).toDouble(), entry.value(QStringLiteral("y")).toDouble());
        if (!circuit.addGateWithId(id, kind.value(), position)) {
            error = QStringLiteral("Duplicate gate id in circuit file.");
            return std::nullopt;
        }
    }

    const QJsonArray wires = json.value(QStringLiteral("wires")).toArray();
    for (const QJsonValue& value : wires) {
        const QJsonObject entry = value.toObject();
        const std::optional<PinId> from = pinFromJson(entry.value(QStringLiteral("from")).toObject());
        const std::optional<PinId> to = pinFromJson(entry.value(QStringLiteral("to")).toObject());
        if (!from.has_value() || !to.has_value()) {
            error = QStringLiteral("Invalid wire pin in circuit file.");
            return std::nullopt;
        }

        const WireValidationResult validation = circuit.validateWire(from.value(), to.value());
        if (validation != WireValidationResult::Ok) {
            // Reuse the same messages the UI shows for manual wiring mistakes.
            error = QString::fromUtf8(wireValidationMessage(validation));
            return std::nullopt;
        }

        if (!circuit.addWire(from.value(), to.value())) {
            error = QStringLiteral("Failed to add wire from circuit file.");
            return std::nullopt;
        }
    }

    seedComponentIdGenerator(maxId + 1); // Avoid colliding with restored component ids.
    return circuit;
}

} // namespace digitallogic
