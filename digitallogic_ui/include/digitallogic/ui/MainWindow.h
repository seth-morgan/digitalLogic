#pragma once

#include <QMainWindow>

class QLabel;

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
    void saveCircuit();
    void openCircuit();
    void showStatusMessage(const QString& message);

    SandboxView* m_sandboxView{nullptr};
    GatePaletteWidget* m_gatePalette{nullptr};
    SimulationController* m_simulationController{nullptr};
    QLabel* m_statusLabel{nullptr};
};

} // namespace digitallogic::ui
