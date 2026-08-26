/**
 * @file ChallengeController.h
 * @brief Declares orchestration of challenge mode, validation, and level flow.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include <QObject>
#include <QString>
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
    ChallengeController(CircuitController* circuitController, GatePaletteWidget* gatePalette,
                        ChallengePanelWidget* challengePanel, SimulationController* simulationController,
                        WinCelebrationOverlay* winOverlay, QObject* parent = nullptr);

    void openLevelPicker();
    // Active when a level index is set (challenge mode entered).
    [[nodiscard]] bool isActive() const noexcept { return m_currentLevelIndex.has_value(); }

public slots:
    void startLevel(int index);
    void checkSolution();
    void advanceToNextLevel();
    void exitChallengeMode();

signals:
    void challengeModeChanged(bool active);
    void statusMessage(const QString& message);

private:
    void applyLevelBudget(const ChallengeLevel& level);
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
