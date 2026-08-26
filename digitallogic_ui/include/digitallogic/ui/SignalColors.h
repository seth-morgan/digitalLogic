/**
 * @file SignalColors.h
 * @brief Maps SignalValue to sandbox display colors based on simulation state.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/SignalValue.h"
#include "digitallogic/ui/AppTheme.h"

#include <QColor>

namespace digitallogic::ui {

/**
 * @brief Returns the sandbox display color for a signal value when simulated.
 */
[[nodiscard]] inline QColor signalColor(const SignalValue value, const bool simulated)
{
    // Idle/unknown wires stay muted until a simulation run paints true/false.
    if (!simulated || value == SignalValue::Unknown) {
        return AppTheme::wireIdle();
    }

    return (value == SignalValue::True) ? AppTheme::signalTrue() : AppTheme::signalFalse();
}

} // namespace digitallogic::ui
