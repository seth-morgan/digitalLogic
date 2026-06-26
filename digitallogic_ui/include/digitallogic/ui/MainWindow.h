#pragma once

#include <QMainWindow>

namespace digitallogic::ui {

class GatePaletteWidget;
class SandboxView;
class SimulationController;

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

    SandboxView* m_sandboxView{nullptr};
    GatePaletteWidget* m_gatePalette{nullptr};
    SimulationController* m_simulationController{nullptr};
};

} // namespace digitallogic::ui
