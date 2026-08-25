/**
 * @file PinGraphicsItem.h
 * @brief Declares the clickable connection pin graphics item for the sandbox.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/PinId.h"
#include "digitallogic/model/SignalValue.h"

#include <QGraphicsEllipseItem>
#include <QPointF>

namespace digitallogic::ui {

class CircuitController;

/**
 * @brief Clickable connection pin rendered on the sandbox canvas.
 */
class PinGraphicsItem final : public QGraphicsEllipseItem {
public:
    // Distinguishes valid wire endpoints (output -> input) during drag.
    enum class PinRole {
        SourceOutput,
        GateInput,
        GateOutput,
        TargetInput
    };

    PinGraphicsItem(PinId pinId, PinRole role, CircuitController* controller, QGraphicsItem* parent = nullptr);

    [[nodiscard]] PinId pinId() const noexcept { return m_pinId; }
    [[nodiscard]] PinRole role() const noexcept { return m_role; }
    [[nodiscard]] QPointF sceneCenter() const;

    void setSignalValue(SignalValue value, bool simulated);
    // Highlight while this pin is the start of an in-progress wire drag.
    void setPendingWire(bool pending);

private:
    PinId m_pinId;
    PinRole m_role;
    CircuitController* m_controller;
    SignalValue m_value{SignalValue::Unknown};
    bool m_simulated{false};
};

} // namespace digitallogic::ui
