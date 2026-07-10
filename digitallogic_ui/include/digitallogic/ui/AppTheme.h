#pragma once

#include "digitallogic/model/Gate.h"

#include <QColor>
#include <QString>

namespace digitallogic::ui {

/**
 * @brief Centralized dark-theme colors and widget styles for the simulator UI.
 */
struct AppTheme final {
    static QColor background();
    static QColor surface();
    static QColor surfaceElevated();
    static QColor textPrimary();
    static QColor textMuted();
    static QColor gridLine();
    static QColor wireIdle();
    static QColor componentFill();
    static QColor componentBorder();
    static QColor selection();
    static QColor targetAccent();
    static QColor signalTrue();
    static QColor signalFalse();

    [[nodiscard]] static QString applicationStylesheet();
    [[nodiscard]] static QString gateButtonStylesheet(GateKind kind);
    [[nodiscard]] static QString toolbarButtonStylesheet();
    [[nodiscard]] static QString primaryActionStylesheet();
    static QColor gateAccent(GateKind kind);
};

} // namespace digitallogic::ui
