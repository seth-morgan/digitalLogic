/**
 * @file CircuitSerializer.h
 * @brief Declares JSON save/load helpers for Circuit (no UI types).
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/Circuit.h"

#include <QJsonObject>
#include <QString>
#include <optional>

namespace digitallogic {

/**
 * @brief Serializes circuits to and from JSON without UI dependencies.
 */
class CircuitSerializer final {
public:
    /**
     * @brief Serializes a circuit to a JSON object.
     * @param circuit Circuit to export.
     * @return JSON representation.
     */
    [[nodiscard]] static QJsonObject toJson(const Circuit& circuit);

    /**
     * @brief Deserializes a circuit from JSON.
     * @param json Saved circuit document.
     * @param error Populated with a message when deserialization fails.
     * @return Reconstructed circuit, or empty on failure.
     */
    [[nodiscard]] static std::optional<Circuit> fromJson(const QJsonObject& json, QString& error);
};

} // namespace digitallogic
