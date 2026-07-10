#include "digitallogic/ui/SimulationController.h"

#include "digitallogic/ui/CircuitController.h"

namespace digitallogic::ui {

SimulationController::SimulationController(CircuitController* circuitController, QObject* parent)
    : QObject(parent)
    , m_circuitController(circuitController)
{
}

void SimulationController::runSimulation()
{
    if (m_circuitController == nullptr) {
        return;
    }

    const std::optional<QVector<PinSignal>> results = SimulationEngine::run(m_circuitController->circuit());
    if (!results.has_value()) {
        emit statusMessage(QStringLiteral("Cannot simulate: cycle detected in the circuit."));
        return;
    }

    m_lastResults.clear();
    for (const PinSignal& pinSignal : results.value()) {
        m_lastResults.insert(pinSignal.pin, pinSignal.value);
    }

    m_circuitController->applySimulationResults(m_lastResults);
}

void SimulationController::resetSimulation()
{
    m_lastResults.clear();
    if (m_circuitController != nullptr) {
        m_circuitController->clearSimulationVisuals();
    }
}

} // namespace digitallogic::ui
