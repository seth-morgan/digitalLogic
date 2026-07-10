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
    [[nodiscard]] bool tryBeginWireAt(const QPointF& scenePos);

    CircuitController* m_circuitController{nullptr};
};

} // namespace digitallogic::ui
