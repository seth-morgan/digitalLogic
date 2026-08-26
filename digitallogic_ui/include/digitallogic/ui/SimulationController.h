/**
 * @file SimulationController.h
 * @brief Declares the UI controller that runs simulation and paints pin results.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include "digitallogic/model/PinId.h"
#include "digitallogic/model/SignalValue.h"
#include "digitallogic/model/SimulationEngine.h"

#include <QHash>
#include <QObject>

namespace digitallogic::ui {

class CircuitController;

/**
 * @brief Runs simulation and applies visual results to the sandbox.
 */
class SimulationController final : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a simulation controller for the given circuit controller.
     */
    explicit SimulationController(CircuitController* circuitController, QObject* parent = nullptr);

public slots:
    /**
     * @brief Runs the simulation engine and paints resulting pin values.
     */
    void runSimulation();
    /**
     * @brief Clears the last simulation results from the sandbox visuals.
     */
    void resetSimulation();

signals:
    void statusMessage(const QString& message);

private:
    CircuitController* m_circuitController;
    // Cached so reset can clear the last painted pin colors.
    QHash<PinId, SignalValue> m_lastResults;
};

} // namespace digitallogic::ui
