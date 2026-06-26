#include "digitallogic/ui/MainWindow.h"

#include "digitallogic/ui/GatePaletteWidget.h"
#include "digitallogic/ui/SandboxView.h"
#include "digitallogic/ui/SimulationController.h"

#include <QAction>
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
    m_gatePalette = new GatePaletteWidget(central);

    layout->addWidget(m_sandboxView, 4);
    layout->addWidget(m_gatePalette, 1);
    setCentralWidget(central);

    m_simulationController = new SimulationController(m_sandboxView->circuitController(), this);
}

void MainWindow::setupToolbar()
{
    auto* toolbar = addToolBar(QStringLiteral("Simulation"));
    toolbar->setMovable(false);

    auto* runAction = toolbar->addAction(QStringLiteral("Run"));
    auto* resetAction = toolbar->addAction(QStringLiteral("Reset"));

    connect(runAction, &QAction::triggered, m_simulationController, &SimulationController::runSimulation);
    connect(resetAction, &QAction::triggered, m_simulationController, &SimulationController::resetSimulation);
}

} // namespace digitallogic::ui
