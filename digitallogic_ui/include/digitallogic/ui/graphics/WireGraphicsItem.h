#pragma once

#include "digitallogic/model/PinId.h"
#include "digitallogic/model/SignalValue.h"

#include <QGraphicsLineItem>

namespace digitallogic::ui {

class PinGraphicsItem;

/**
 * @brief Visual wire connecting two pins on the sandbox canvas.
 */
class WireGraphicsItem final : public QGraphicsLineItem {
public:
    WireGraphicsItem(PinGraphicsItem* fromPin, PinGraphicsItem* toPin, QGraphicsItem* parent = nullptr);

    [[nodiscard]] PinId fromPinId() const noexcept { return m_fromPinId; }
    [[nodiscard]] PinId toPinId() const noexcept { return m_toPinId; }

    void updatePath();
    void setSignalValue(SignalValue value, bool simulated);
    void clearSimulationHighlight();

private:
    PinGraphicsItem* m_fromPin{nullptr};
    PinGraphicsItem* m_toPin{nullptr};
    PinId m_fromPinId;
    PinId m_toPinId;
};

} // namespace digitallogic::ui
