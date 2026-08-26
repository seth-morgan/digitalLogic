/**
 * @file SimulationController.cpp
 * @brief Runs the simulation engine and applies results to the circuit canvas.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/ui/SimulationController.h"

#include "digitallogic/ui/CircuitController.h"

namespace digitallogic::ui {

/**
 * @brief Constructs a simulation controller for the given circuit controller.
 */
SimulationController::SimulationController(CircuitController* circuitController, QObject* parent)
    : QObject(parent)
    , m_circuitController(circuitController)
{
}

/**
 * @brief Runs the simulation engine and paints resulting pin values.
 */
void SimulationController::runSimulation()
{
    if (m_circuitController == nullptr) {
        return;
    }

    const std::optional<QVector<PinSignal>> results = SimulationEngine::run(m_circuitController->circuit());
    // Engine returns nullopt when the graph contains a cycle.
    if (!results.has_value()) {
        emit statusMessage(QStringLiteral("Cannot simulate: cycle detected in the circuit."));
        return;
    }

    QHash<PinId, SignalValue> pinValues;
    for (const PinSignal& pinSignal : results.value()) {
        pinValues.insert(pinSignal.pin, pinSignal.value);
    }

    m_circuitController->applySimulationResults(pinValues);
}

/**
 * @brief Clears the last simulation results from the sandbox visuals.
 */
void SimulationController::resetSimulation()
{
    if (m_circuitController != nullptr) {
        m_circuitController->clearSimulationVisuals();
    }
}

} // namespace digitallogic::ui
