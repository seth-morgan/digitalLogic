#pragma once

#include "digitallogic/model/SignalValue.h"

#include <QColor>

namespace digitallogic::ui {

/**
 * @brief Maps a signal value to the sandbox display color.
 */
[[nodiscard]] inline QColor signalColor(const SignalValue value, const bool simulated)
{
    if (!simulated || value == SignalValue::Unknown) {
        return QColor(QStringLiteral("#808080"));
    }

    return (value == SignalValue::True) ? QColor(QStringLiteral("#2ecc71"))
                                        : QColor(QStringLiteral("#e74c3c"));
}

} // namespace digitallogic::ui
