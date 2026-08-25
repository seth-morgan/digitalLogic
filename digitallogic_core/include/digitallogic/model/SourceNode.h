/**
 * @file SourceNode.h
 * @brief Declares a toggleable input source used as a circuit primary input.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/ComponentId.h"
#include "digitallogic/model/SignalValue.h"

namespace digitallogic {

/**
 * @brief Fixed input source placed in the sandbox canvas.
 */
class SourceNode final {
public:
    /**
     * @brief Creates a source with the given identifier and initial value.
     * @param id Unique component id.
     * @param initialValue Starting signal value.
     */
    SourceNode(ComponentId id, SignalValue initialValue = SignalValue::False);

    [[nodiscard]] ComponentId id() const noexcept { return m_id; }
    [[nodiscard]] SignalValue value() const noexcept { return m_value; }

    /**
     * @brief Sets the source output value.
     * @param value New signal value.
     */
    void setValue(SignalValue value) noexcept { m_value = value; }

    /**
     * @brief Toggles between false and true.
     */
    void toggle() noexcept;

private:
    ComponentId m_id;
    SignalValue m_value;
};

} // namespace digitallogic
