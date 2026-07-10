#pragma once

#include <QAction>
#include <QHash>
#include <QMainWindow>

class QLabel;

namespace digitallogic::ui {

class GatePaletteWidget;
class SandboxView;
class SimulationController;
class ChallengePanelWidget;
class ChallengeController;
class WinCelebrationOverlay;

/**
 * @brief Top-level window with toolbar, sandbox, and gate palette.
 */
class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void setupUi();
    void setupToolbar();
    void saveCircuit();
    void openCircuit();
    void openChallengeMode();
    void showStatusMessage(const QString& message);
    void setSandboxActionsEnabled(bool enabled);
    void resizeEvent(QResizeEvent* event) override;

    SandboxView* m_sandboxView{nullptr};
    GatePaletteWidget* m_gatePalette{nullptr};
    ChallengePanelWidget* m_challengePanel{nullptr};
    SimulationController* m_simulationController{nullptr};
    ChallengeController* m_challengeController{nullptr};
    WinCelebrationOverlay* m_winOverlay{nullptr};
    QLabel* m_statusLabel{nullptr};
    QAction* m_saveAction{nullptr};
    QAction* m_openAction{nullptr};
    QAction* m_clearAction{nullptr};
};

} // namespace digitallogic::ui
