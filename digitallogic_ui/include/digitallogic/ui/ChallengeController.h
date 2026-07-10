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

/**
 * @brief Orchestrates challenge mode flow, validation, and level progression.
 */
class ChallengeController final : public QObject {
    Q_OBJECT

public:
    ChallengeController(CircuitController* circuitController, GatePaletteWidget* gatePalette,
                        ChallengePanelWidget* challengePanel, SimulationController* simulationController,
                        QObject* parent = nullptr);

    void openLevelPicker();
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
    std::optional<int> m_currentLevelIndex;
    bool m_levelSolved{false};
};

} // namespace digitallogic::ui
