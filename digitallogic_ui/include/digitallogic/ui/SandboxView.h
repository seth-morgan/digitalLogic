/**
 * @file SandboxView.h
 * @brief Declares the graphics view used as the circuit sandbox canvas.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include <QGraphicsView>

class QMouseEvent;

namespace digitallogic::ui {

class CircuitController;

/**
 * @brief Main sandbox canvas for placing and wiring circuit components.
 */
class SandboxView final : public QGraphicsView {
    Q_OBJECT

public:
    explicit SandboxView(QWidget* parent = nullptr);

    [[nodiscard]] CircuitController* circuitController() const noexcept { return m_circuitController; }

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    // Starts a wire drag when the press lands on a pin hotspot.
    [[nodiscard]] bool tryBeginWireAt(const QPointF& scenePos);

    CircuitController* m_circuitController{nullptr};
};

} // namespace digitallogic::ui
