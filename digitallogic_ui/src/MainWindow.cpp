#include "digitallogic/ui/MainWindow.h"

#include "digitallogic/ui/ChallengeController.h"
#include "digitallogic/ui/ChallengePanelWidget.h"
#include "digitallogic/ui/CircuitController.h"
#include "digitallogic/ui/GatePaletteWidget.h"
#include "digitallogic/ui/SandboxView.h"
#include "digitallogic/ui/SimulationController.h"

#include <QAction>
#include <QFileDialog>
#include <QLabel>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

namespace digitallogic::ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
    setupToolbar();
    setWindowTitle(QStringLiteral("Digital Logic Simulator"));
    resize(1280, 800);
}

void MainWindow::setupUi()
{
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_sandboxView = new SandboxView(central);
    m_challengePanel = new ChallengePanelWidget(central);
    m_gatePalette = new GatePaletteWidget(central);

    layout->addWidget(m_challengePanel);
    layout->addWidget(m_sandboxView, 4);
    layout->addWidget(m_gatePalette, 1);
    setCentralWidget(central);

    m_simulationController = new SimulationController(m_sandboxView->circuitController(), this);
    m_challengeController = new ChallengeController(m_sandboxView->circuitController(), m_gatePalette, m_challengePanel,
                                                    m_simulationController, this);
    m_statusLabel = new QLabel(QStringLiteral("Drag from an output pin to a gate input. Select items and press Delete to remove."));
    m_statusLabel->setContentsMargins(8, 0, 8, 0);
    statusBar()->addWidget(m_statusLabel, 1);

    connect(m_sandboxView->circuitController(), &CircuitController::statusMessage, this, &MainWindow::showStatusMessage);
    connect(m_sandboxView->circuitController(), &CircuitController::sourceValueChanged, m_simulationController,
            &SimulationController::runSimulation);
    connect(m_challengeController, &ChallengeController::statusMessage, this, &MainWindow::showStatusMessage);
    connect(m_challengeController, &ChallengeController::challengeModeChanged, this,
            [this](const bool challengeActive) { setSandboxActionsEnabled(!challengeActive); });
}

void MainWindow::setupToolbar()
{
    auto* toolbar = addToolBar(QStringLiteral("Simulation"));
    toolbar->setMovable(false);

    auto* runAction = toolbar->addAction(QStringLiteral("Run"));
    auto* resetAction = toolbar->addAction(QStringLiteral("Reset"));
    auto* clearAction = toolbar->addAction(QStringLiteral("Clear"));
    auto* deleteAction = toolbar->addAction(QStringLiteral("Delete"));
    toolbar->addSeparator();
    auto* challengeAction = toolbar->addAction(QStringLiteral("Challenge"));
    toolbar->addSeparator();
    m_saveAction = toolbar->addAction(QStringLiteral("Save"));
    m_openAction = toolbar->addAction(QStringLiteral("Open"));

    CircuitController* controller = m_sandboxView->circuitController();

    connect(runAction, &QAction::triggered, m_simulationController, &SimulationController::runSimulation);
    connect(resetAction, &QAction::triggered, m_simulationController, &SimulationController::resetSimulation);
    connect(clearAction, &QAction::triggered, controller, &CircuitController::clearCanvas);
    connect(deleteAction, &QAction::triggered, controller, &CircuitController::deleteSelection);
    connect(challengeAction, &QAction::triggered, this, &MainWindow::openChallengeMode);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveCircuit);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openCircuit);
}

void MainWindow::saveCircuit()
{
    const QString path = QFileDialog::getSaveFileName(this, QStringLiteral("Save Circuit"), QString(),
                                                      QStringLiteral("Circuit JSON (*.json)"));
    if (path.isEmpty()) {
        return;
    }

    (void)m_sandboxView->circuitController()->saveToFile(path);
}

void MainWindow::openCircuit()
{
    const QString path = QFileDialog::getOpenFileName(this, QStringLiteral("Open Circuit"), QString(),
                                                      QStringLiteral("Circuit JSON (*.json)"));
    if (path.isEmpty()) {
        return;
    }

    (void)m_sandboxView->circuitController()->loadFromFile(path);
}

void MainWindow::openChallengeMode()
{
    if (m_challengeController != nullptr) {
        m_challengeController->openLevelPicker();
    }
}

void MainWindow::setSandboxActionsEnabled(const bool enabled)
{
    const bool sandboxEnabled = enabled;
    if (m_saveAction != nullptr) {
        m_saveAction->setEnabled(sandboxEnabled);
    }
    if (m_openAction != nullptr) {
        m_openAction->setEnabled(sandboxEnabled);
    }
}

void MainWindow::showStatusMessage(const QString& message)
{
    if (m_statusLabel != nullptr) {
        m_statusLabel->setText(message);
    }
}

} // namespace digitallogic::ui
