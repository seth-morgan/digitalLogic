/**
 * @file AppTheme.h
 * @brief Declares centralized dark-theme colors and widget stylesheets for the UI.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/Gate.h"

#include <QColor>
#include <QString>

namespace digitallogic::ui {

/**
 * @brief Centralized dark-theme colors and widget styles for the simulator UI.
 */
struct AppTheme final {
    /**
     * @brief Returns the main application background color.
     */
    static QColor background();
    /**
     * @brief Returns the default surface color for panels and chrome.
     */
    static QColor surface();
    /**
     * @brief Returns the elevated surface color for overlays and raised panels.
     */
    static QColor surfaceElevated();
    /**
     * @brief Returns the primary text color.
     */
    static QColor textPrimary();
    /**
     * @brief Returns the muted secondary text color.
     */
    static QColor textMuted();
    /**
     * @brief Returns the sandbox grid line color.
     */
    static QColor gridLine();
    /**
     * @brief Returns the idle (unsimulated) wire color.
     */
    static QColor wireIdle();
    /**
     * @brief Returns the default component body fill color.
     */
    static QColor componentFill();
    /**
     * @brief Returns the default component border color.
     */
    static QColor componentBorder();
    /**
     * @brief Returns the selection highlight color.
     */
    static QColor selection();
    /**
     * @brief Returns the accent color used for challenge targets.
     */
    static QColor targetAccent();
    /**
     * @brief Returns the color used for simulated true signals.
     */
    static QColor signalTrue();
    /**
     * @brief Returns the color used for simulated false signals.
     */
    static QColor signalFalse();

    /**
     * @brief Returns the global QSS applied once at application startup.
     */
    // Global QSS applied once at application startup.
    [[nodiscard]] static QString applicationStylesheet();
    /**
     * @brief Returns the stylesheet for a palette button of the given gate kind.
     */
    // Per-gate palette button colors keyed by GateKind.
    [[nodiscard]] static QString gateButtonStylesheet(GateKind kind);
    /**
     * @brief Returns the stylesheet for standard toolbar buttons.
     */
    [[nodiscard]] static QString toolbarButtonStylesheet();
    /**
     * @brief Returns the stylesheet for primary action buttons.
     */
    [[nodiscard]] static QString primaryActionStylesheet();
    /**
     * @brief Returns the accent color associated with a gate kind.
     */
    static QColor gateAccent(GateKind kind);
};

} // namespace digitallogic::ui
