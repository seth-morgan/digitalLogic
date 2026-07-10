#pragma once

#include "digitallogic/model/SignalValue.h"
#include "digitallogic/ui/AppTheme.h"

#include <QColor>

namespace digitallogic::ui {

/**
 * @brief Maps a signal value to the sandbox display color.
 */
[[nodiscard]] inline QColor signalColor(const SignalValue value, const bool simulated)
{
    if (!simulated || value == SignalValue::Unknown) {
        return AppTheme::wireIdle();
    }

    return (value == SignalValue::True) ? AppTheme::signalTrue() : AppTheme::signalFalse();
}

} // namespace digitallogic::ui
