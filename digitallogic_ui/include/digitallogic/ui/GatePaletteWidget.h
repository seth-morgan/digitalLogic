/**
 * @file GatePaletteWidget.h
 * @brief Declares the bottom palette of draggable gate templates.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/Gate.h"

#include <QHash>
#include <QPushButton>
#include <QVector>

namespace digitallogic::ui {

/**
 * @brief Bottom palette with draggable gate templates.
 */
class GatePaletteWidget final : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs the gate palette and its draggable gate buttons.
     */
    explicit GatePaletteWidget(QWidget* parent = nullptr);

    /**
     * @brief Enables or disables challenge-mode budget constraints on the palette.
     */
    void setChallengeMode(bool enabled);
    /**
     * @brief Updates remaining gate counts and disables exhausted buttons.
     */
    // Remaining counts per GateKind; buttons disable when a budget hits zero.
    void updateGateBudget(const QHash<GateKind, int>& remaining);

signals:
    void gateDragStarted(GateKind kind);

protected:
    /**
     * @brief Intercepts button press events to start a gate drag.
     */
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    struct GateButtonEntry final {
        GateKind kind;
        QString label;
        QPushButton* button{nullptr};
    };

    /**
     * @brief Begins a drag-and-drop operation for the given gate kind.
     */
    void startGateDrag(GateKind kind, QWidget* sourceWidget);
    /**
     * @brief Refreshes enabled state and label for one palette button.
     */
    void refreshButtonState(GateButtonEntry& entry);
    /**
     * @brief Refreshes enabled state for every palette button.
     */
    void refreshAllButtons();
    /**
     * @brief Finds the palette entry whose button matches the watched object.
     */
    [[nodiscard]] GateButtonEntry* findEntry(QObject* watched);

    QVector<GateButtonEntry> m_gateButtons;
    bool m_challengeMode{false};
    QHash<GateKind, int> m_gateBudget;
};

} // namespace digitallogic::ui
