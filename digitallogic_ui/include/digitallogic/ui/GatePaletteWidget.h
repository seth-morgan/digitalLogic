#pragma once

#include "digitallogic/model/Gate.h"

#include <QHash>
#include <QPushButton>

namespace digitallogic::ui {

/**
 * @brief Bottom palette with draggable AND, OR, and NOT gate templates.
 */
class GatePaletteWidget final : public QWidget {
    Q_OBJECT

public:
    explicit GatePaletteWidget(QWidget* parent = nullptr);

    void setChallengeMode(bool enabled);
    void updateGateBudget(const QHash<GateKind, int>& remaining);

signals:
    void gateDragStarted(GateKind kind);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void startGateDrag(GateKind kind, QWidget* sourceWidget);
    void refreshButtonState(QPushButton* button, GateKind kind, const QString& baseLabel);

    QPushButton* m_andButton{nullptr};
    QPushButton* m_orButton{nullptr};
    QPushButton* m_notButton{nullptr};
    bool m_challengeMode{false};
    QHash<GateKind, int> m_gateBudget;
};

} // namespace digitallogic::ui
