/**
 * @file ChallengeController.cpp
 * @brief Orchestrates challenge mode: level load, budget, validation, and return to sandbox.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/ui/ChallengeController.h"

#include "digitallogic/challenge/ChallengeCatalog.h"
#include "digitallogic/challenge/ChallengeLevel.h"
#include "digitallogic/challenge/ChallengeValidator.h"
#include "digitallogic/ui/ChallengeDialog.h"
#include "digitallogic/ui/ChallengePanelWidget.h"
#include "digitallogic/ui/CircuitController.h"
#include "digitallogic/ui/GatePaletteWidget.h"
#include "digitallogic/ui/SimulationController.h"
#include "digitallogic/ui/WinCelebrationOverlay.h"

namespace digitallogic::ui {

/**
 * @brief Constructs a challenge controller wired to sandbox and challenge UI.
 */
ChallengeController::ChallengeController(CircuitController* circuitController, GatePaletteWidget* gatePalette,
                                         ChallengePanelWidget* challengePanel,
                                         SimulationController* simulationController, WinCelebrationOverlay* winOverlay,
                                         QObject* parent)
    : QObject(parent)
    , m_circuitController(circuitController)
    , m_gatePalette(gatePalette)
    , m_challengePanel(challengePanel)
    , m_simulationController(simulationController)
    , m_winOverlay(winOverlay)
{
    if (m_circuitController != nullptr) {
        connect(m_circuitController, &CircuitController::gateBudgetChanged, this, &ChallengeController::refreshPaletteBudget);
    }

    if (m_challengePanel != nullptr) {
        connect(m_challengePanel, &ChallengePanelWidget::checkSolutionRequested, this, &ChallengeController::checkSolution);
        connect(m_challengePanel, &ChallengePanelWidget::nextLevelRequested, this, &ChallengeController::advanceToNextLevel);
        connect(m_challengePanel, &ChallengePanelWidget::exitChallengeRequested, this, &ChallengeController::exitChallengeMode);
    }
}

/**
 * @brief Opens the level picker dialog and starts the chosen level.
 */
void ChallengeController::openLevelPicker()
{
    ChallengeDialog dialog;
    if (dialog.exec() != QDialog::Accepted || dialog.selectedLevelIndex() < 0) {
        return;
    }

    startLevel(dialog.selectedLevelIndex());
}

/**
 * @brief Loads and starts the challenge level at the given catalog index.
 */
void ChallengeController::startLevel(const int index)
{
    const ChallengeLevel* level = ChallengeCatalog::levelAt(index);
    if (level == nullptr || m_circuitController == nullptr) {
        emit statusMessage(QStringLiteral("Unknown challenge level."));
        return;
    }

    m_currentLevelIndex = index;
    m_levelSolved = false;

    // Load places fixed sources/target; budget limits which gates the player may add.
    QHash<QString, ComponentId> sourceIdsByLabel;
    ComponentId targetId;
    m_circuitController->loadChallengeLevel(*level, sourceIdsByLabel, targetId);
    applyLevelBudget(*level);

    if (m_simulationController != nullptr) {
        m_simulationController->resetSimulation();
    }

    if (m_challengePanel != nullptr) {
        const bool hasNextLevel = index + 1 < ChallengeCatalog::levelCount();
        m_challengePanel->showLevel(level->title, level->description, hasNextLevel);
        m_challengePanel->setVisibleForChallenge(true);
    }

    if (m_gatePalette != nullptr) {
        m_gatePalette->setChallengeMode(true);
        refreshPaletteBudget();
    }

    emit challengeModeChanged(true);
    emit statusMessage(QStringLiteral("Challenge started: %1").arg(level->title));
}

/**
 * @brief Applies the level's gate allowances as the current budget.
 */
void ChallengeController::applyLevelBudget(const ChallengeLevel& level)
{
    if (m_circuitController == nullptr) {
        return;
    }

    QHash<GateKind, int> budget;
    for (const ChallengeGateAllowance& allowance : level.gateAllowances) {
        budget.insert(allowance.kind, allowance.maxCount);
    }
    m_circuitController->setChallengeGateBudget(budget);
}

/**
 * @brief Refreshes the palette with the remaining challenge gate budget.
 */
void ChallengeController::refreshPaletteBudget()
{
    if (m_gatePalette == nullptr || m_circuitController == nullptr || !m_circuitController->isChallengeMode()) {
        return;
    }

    m_gatePalette->updateGateBudget(m_circuitController->challengeGateBudgetRemaining());
}

/**
 * @brief Validates the player's circuit against the current level.
 */
void ChallengeController::checkSolution()
{
    if (!m_currentLevelIndex.has_value() || m_circuitController == nullptr) {
        return;
    }

    const ChallengeLevel* level = ChallengeCatalog::levelAt(m_currentLevelIndex.value());
    if (level == nullptr) {
        return;
    }

    // Run sim first so the canvas reflects current pin values before validation messaging.
    if (m_simulationController != nullptr) {
        m_simulationController->runSimulation();
    }

    const ChallengeValidationResult result = ChallengeValidator::validate(
        m_circuitController->circuit(), *level, m_circuitController->challengeSourceIdsByLabel(),
        m_circuitController->challengeTargetId());

    emit statusMessage(result.message);

    if (result.success) {
        m_levelSolved = true;
        if (m_challengePanel != nullptr) {
            m_challengePanel->setLevelSolved(true);
        }
        if (m_winOverlay != nullptr) {
            m_winOverlay->play();
        }
    }
}

/**
 * @brief Advances from a solved level to the next catalog level.
 */
void ChallengeController::advanceToNextLevel()
{
    if (!m_levelSolved || !m_currentLevelIndex.has_value()) {
        return;
    }

    const int nextIndex = m_currentLevelIndex.value() + 1;
    if (nextIndex >= ChallengeCatalog::levelCount()) {
        emit statusMessage(QStringLiteral("You completed every challenge level!"));
        exitChallengeMode();
        return;
    }

    startLevel(nextIndex);
}

/**
 * @brief Exits challenge mode and restores sandbox mode.
 */
void ChallengeController::exitChallengeMode()
{
    m_currentLevelIndex.reset();
    m_levelSolved = false;

    if (m_circuitController != nullptr) {
        m_circuitController->restoreSandboxMode();
    }

    if (m_simulationController != nullptr) {
        m_simulationController->resetSimulation();
    }

    if (m_challengePanel != nullptr) {
        m_challengePanel->setVisibleForChallenge(false);
    }

    if (m_gatePalette != nullptr) {
        m_gatePalette->setChallengeMode(false);
        m_gatePalette->updateGateBudget({});
    }

    emit challengeModeChanged(false);
    emit statusMessage(QStringLiteral("Returned to sandbox mode."));
}

} // namespace digitallogic::ui
