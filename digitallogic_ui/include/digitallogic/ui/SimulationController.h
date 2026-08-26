/**
 * @file SimulationController.h
 * @brief Declares the UI controller that runs simulation and paints pin results.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/SimulationEngine.h"

#include <QObject>
#include <QString>

namespace digitallogic::ui {

class CircuitController;

/**
 * @brief Runs simulation and applies visual results to the sandbox.
 */
class SimulationController final : public QObject {
    Q_OBJECT

public:
    explicit SimulationController(CircuitController* circuitController, QObject* parent = nullptr);

public slots:
    void runSimulation();
    void resetSimulation();

signals:
    void statusMessage(const QString& message);

private:
    CircuitController* m_circuitController;
};

} // namespace digitallogic::ui
