#pragma once

#include "digitallogic/model/Gate.h"

#include <QPushButton>

namespace digitallogic::ui {

/**
 * @brief Bottom palette with draggable AND, OR, and NOT gate templates.
 */
class GatePaletteWidget final : public QWidget {
    Q_OBJECT

public:
    explicit GatePaletteWidget(QWidget* parent = nullptr);

signals:
    void gateDragStarted(GateKind kind);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void startGateDrag(GateKind kind, QWidget* sourceWidget);

    QPushButton* m_andButton{nullptr};
    QPushButton* m_orButton{nullptr};
    QPushButton* m_notButton{nullptr};
};

} // namespace digitallogic::ui
