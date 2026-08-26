/**
 * @file MainWindow.h
 * @brief Declares the top-level window wiring toolbar, sandbox, and challenge UI.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include <QAction>
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
    /**
     * @brief Constructs the main window and builds the sandbox UI.
     */
    explicit MainWindow(QWidget* parent = nullptr);

private:
    /**
     * @brief Builds the central layout, sandbox, palette, and challenge panel.
     */
    void setupUi();
    /**
     * @brief Creates toolbar actions for save, open, clear, simulate, and challenges.
     */
    void setupToolbar();
    /**
     * @brief Saves the current circuit to a user-selected file.
     */
    void saveCircuit();
    /**
     * @brief Opens a circuit from a user-selected file.
     */
    void openCircuit();
    /**
     * @brief Opens the challenge level picker and enters challenge mode.
     */
    void openChallengeMode();
    /**
     * @brief Shows a short status message in the window status area.
     */
    void showStatusMessage(const QString& message);
    /**
     * @brief Enables or disables sandbox file actions during challenge mode.
     */
    // Disables save/open while challenge mode owns the canvas.
    void setSandboxActionsEnabled(bool enabled);
    /**
     * @brief Keeps the win celebration overlay sized to the window.
     */
    void resizeEvent(QResizeEvent* event) override;

    SandboxView* m_sandboxView{nullptr};
    GatePaletteWidget* m_gatePalette{nullptr};
    ChallengePanelWidget* m_challengePanel{nullptr};
    SimulationController* m_simulationController{nullptr};
    ChallengeController* m_challengeController{nullptr};
    // Full-window flash; resized to match MainWindow on every resize.
    WinCelebrationOverlay* m_winOverlay{nullptr};
    QLabel* m_statusLabel{nullptr};
    QAction* m_saveAction{nullptr};
    QAction* m_openAction{nullptr};
};

} // namespace digitallogic::ui
