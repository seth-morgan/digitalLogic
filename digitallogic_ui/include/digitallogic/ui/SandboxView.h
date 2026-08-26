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
    /**
     * @brief Constructs the sandbox view and its circuit controller.
     */
    explicit SandboxView(QWidget* parent = nullptr);

    /**
     * @brief Returns the circuit controller owned by this view.
     */
    [[nodiscard]] CircuitController* circuitController() const noexcept { return m_circuitController; }

protected:
    /**
     * @brief Accepts gate-palette drag events over the canvas.
     */
    void dragEnterEvent(QDragEnterEvent* event) override;
    /**
     * @brief Updates drag feedback while a gate is dragged over the canvas.
     */
    void dragMoveEvent(QDragMoveEvent* event) override;
    /**
     * @brief Places a gate when a palette item is dropped on the canvas.
     */
    void dropEvent(QDropEvent* event) override;
    /**
     * @brief Draws the sandbox background grid.
     */
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    /**
     * @brief Handles press for selection, wire start, and item interaction.
     */
    void mousePressEvent(QMouseEvent* event) override;
    /**
     * @brief Updates an in-progress wire drag as the cursor moves.
     */
    void mouseMoveEvent(QMouseEvent* event) override;
    /**
     * @brief Completes or cancels an in-progress wire drag on release.
     */
    void mouseReleaseEvent(QMouseEvent* event) override;
    /**
     * @brief Handles keyboard shortcuts such as Delete for selection removal.
     */
    void keyPressEvent(QKeyEvent* event) override;

private:
    /**
     * @brief Starts a wire drag when the press lands on a pin hotspot.
     */
    // Starts a wire drag when the press lands on a pin hotspot.
    [[nodiscard]] bool tryBeginWireAt(const QPointF& scenePos);

    CircuitController* m_circuitController{nullptr};
};

} // namespace digitallogic::ui
