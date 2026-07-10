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
    GateGraphicsItem(ComponentId componentId, GateKind kind, int inputCount, CircuitController* controller,
                     QGraphicsItem* parent = nullptr);

    [[nodiscard]] ComponentId componentId() const noexcept { return m_componentId; }
    [[nodiscard]] GateKind kind() const noexcept { return m_kind; }
    [[nodiscard]] PinGraphicsItem* outputPin() const noexcept { return m_outputPin; }
    [[nodiscard]] QVector<PinGraphicsItem*> inputPins() const { return m_inputPins; }

    void setOutputSignal(SignalValue value, bool simulated);
    void clearSimulationHighlight();
    void updateWirePaths();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    void applySelectionStyle();
    [[nodiscard]] static QString gateLabel(GateKind kind);

    ComponentId m_componentId;
    GateKind m_kind;
    CircuitController* m_controller;
    QGraphicsRectItem* m_body{nullptr};
    PinGraphicsItem* m_outputPin{nullptr};
    QVector<PinGraphicsItem*> m_inputPins;
};

} // namespace digitallogic::ui
