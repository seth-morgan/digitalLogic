/**
 * @file GateGraphicsItem.h
 * @brief Declares the graphics item for a logic gate with input/output pins.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/ComponentId.h"
#include "digitallogic/model/Gate.h"
#include "digitallogic/model/SignalValue.h"

#include <QGraphicsItemGroup>
#include <QVector>

namespace digitallogic::ui {

class CircuitController;
class PinGraphicsItem;

/**
 * @brief Visual representation of a logic gate with input and output pins.
 */
class GateGraphicsItem final : public QGraphicsItemGroup {
public:
    /**
     * @brief Constructs a gate graphics item with the given kind and input count.
     */
    GateGraphicsItem(ComponentId componentId, GateKind kind, int inputCount, CircuitController* controller,
                     QGraphicsItem* parent = nullptr);

    /**
     * @brief Returns the gate component id.
     */
    [[nodiscard]] ComponentId componentId() const noexcept { return m_componentId; }
    /**
     * @brief Returns the gate kind.
     */
    [[nodiscard]] GateKind kind() const noexcept { return m_kind; }
    /**
     * @brief Returns the output pin graphics item.
     */
    [[nodiscard]] PinGraphicsItem* outputPin() const noexcept { return m_outputPin; }
    /**
     * @brief Returns the input pin graphics items.
     */
    [[nodiscard]] QVector<PinGraphicsItem*> inputPins() const { return m_inputPins; }

    /**
     * @brief Updates the output pin appearance from a simulated signal value.
     */
    void setOutputSignal(SignalValue value, bool simulated);
    /**
     * @brief Clears simulation highlight styling from the gate output.
     */
    void clearSimulationHighlight();
    /**
     * @brief Recomputes attached wire endpoints after the gate is moved.
     */
    // Recomputes attached wire endpoints after the gate is moved.
    void updateWirePaths();

protected:
    /**
     * @brief Syncs model position and selection style when the item changes.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    /**
     * @brief Applies selected/deselected border styling to the gate body.
     */
    void applySelectionStyle();
    /**
     * @brief Returns the short display label for a gate kind.
     */
    [[nodiscard]] static QString gateLabel(GateKind kind);

    ComponentId m_componentId;
    GateKind m_kind;
    CircuitController* m_controller;
    QGraphicsRectItem* m_body{nullptr};
    PinGraphicsItem* m_outputPin{nullptr};
    QVector<PinGraphicsItem*> m_inputPins;
};

} // namespace digitallogic::ui
