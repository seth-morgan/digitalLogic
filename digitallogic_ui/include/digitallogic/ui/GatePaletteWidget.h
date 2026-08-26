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
    explicit GatePaletteWidget(QWidget* parent = nullptr);

    void setChallengeMode(bool enabled);
    // Remaining counts per GateKind; buttons disable when a budget hits zero.
    void updateGateBudget(const QHash<GateKind, int>& remaining);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    struct GateButtonEntry final {
        GateKind kind;
        QString label;
        QPushButton* button{nullptr};
    };

    void startGateDrag(GateKind kind, QWidget* sourceWidget);
    void refreshButtonState(GateButtonEntry& entry);
    void refreshAllButtons();
    [[nodiscard]] GateButtonEntry* findEntry(QObject* watched);

    QVector<GateButtonEntry> m_gateButtons;
    bool m_challengeMode{false};
    QHash<GateKind, int> m_gateBudget;
};

} // namespace digitallogic::ui
