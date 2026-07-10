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
    explicit SimulationController(CircuitController* circuitController, QObject* parent = nullptr);

public slots:
    void runSimulation();
    void resetSimulation();

signals:
    void statusMessage(const QString& message);

private:
    CircuitController* m_circuitController;
    QHash<PinId, SignalValue> m_lastResults;
};

} // namespace digitallogic::ui
