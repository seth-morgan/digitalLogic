/**
 * @file ChallengeController.h
 * @brief Declares orchestration of challenge mode, validation, and level flow.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include <QObject>
#include <optional>

namespace digitallogic {
struct ChallengeLevel;
}

namespace digitallogic::ui {

class ChallengePanelWidget;
class CircuitController;
class GatePaletteWidget;
class SimulationController;
class WinCelebrationOverlay;

/**
 * @brief Orchestrates challenge mode flow, validation, and level progression.
 */
class ChallengeController final : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a challenge controller wired to sandbox and challenge UI.
     */
    ChallengeController(CircuitController* circuitController, GatePaletteWidget* gatePalette,
                        ChallengePanelWidget* challengePanel, SimulationController* simulationController,
                        WinCelebrationOverlay* winOverlay, QObject* parent = nullptr);

    /**
     * @brief Opens the level picker dialog and starts the chosen level.
     */
    void openLevelPicker();
    /**
     * @brief Returns whether a challenge level is currently active.
     */
    // Active when a level index is set (challenge mode entered).
    [[nodiscard]] bool isActive() const noexcept { return m_currentLevelIndex.has_value(); }

public slots:
    /**
     * @brief Loads and starts the challenge level at the given catalog index.
     */
    void startLevel(int index);
    /**
     * @brief Validates the player's circuit against the current level.
     */
    void checkSolution();
    /**
     * @brief Advances from a solved level to the next catalog level.
     */
    void advanceToNextLevel();
    /**
     * @brief Exits challenge mode and restores sandbox mode.
     */
    void exitChallengeMode();

signals:
    void challengeModeChanged(bool active);
    void statusMessage(const QString& message);

private:
    /**
     * @brief Applies the level's gate allowances as the current budget.
     */
    void applyLevelBudget(const ChallengeLevel& level);
    /**
     * @brief Refreshes the palette with the remaining challenge gate budget.
     */
    void refreshPaletteBudget();

    CircuitController* m_circuitController{nullptr};
    GatePaletteWidget* m_gatePalette{nullptr};
    ChallengePanelWidget* m_challengePanel{nullptr};
    SimulationController* m_simulationController{nullptr};
    WinCelebrationOverlay* m_winOverlay{nullptr};
    std::optional<int> m_currentLevelIndex;
    bool m_levelSolved{false};
};

} // namespace digitallogic::ui
