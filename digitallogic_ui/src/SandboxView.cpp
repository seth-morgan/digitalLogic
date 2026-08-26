/**
 * @file SandboxView.cpp
 * @brief Graphics view for the circuit canvas: wire drag, gate drop, grid, and keyboard delete.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/ui/SandboxView.h"

#include "digitallogic/model/Gate.h"
#include "digitallogic/ui/AppTheme.h"
#include "digitallogic/ui/CircuitController.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QTimer>
#include <cmath>
#include <QPainter>

#include "digitallogic/ui/SandboxMimeTypes.h"

namespace digitallogic::ui {

/**
 * @brief Constructs the sandbox view and its circuit controller.
 */
SandboxView::SandboxView(QWidget* parent)
    : QGraphicsView(parent)
{
    auto* scene = new QGraphicsScene(this);
    scene->setSceneRect(0.0, 0.0, 1200.0, 700.0);
    setScene(scene);
    setRenderHint(QPainter::Antialiasing, true);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setAcceptDrops(true);
    setDragMode(QGraphicsView::NoDrag);
    setFocusPolicy(Qt::StrongFocus);
    setFrameShape(QFrame::NoFrame);
    setBackgroundBrush(AppTheme::background());

    // ~30 FPS tick drives WireGraphicsItem::advance for animated signal flow.
    auto* animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, scene, &QGraphicsScene::advance);
    animationTimer->start(33);

    m_circuitController = new CircuitController(this, this);
    m_circuitController->initializeDefaultSources();
}

// Wire creation starts only from source or gate output pins.
/**
 * @brief Starts a wire drag when the press lands on a pin hotspot.
 */
bool SandboxView::tryBeginWireAt(const QPointF& scenePos)
{
    PinGraphicsItem* pin = m_circuitController->findPinAtScenePos(scenePos);
    if (pin == nullptr) {
        return false;
    }

    if (pin->role() != PinGraphicsItem::PinRole::SourceOutput && pin->role() != PinGraphicsItem::PinRole::GateOutput) {
        return false;
    }

    m_circuitController->beginWireDrag(pin, scenePos);
    setCursor(Qt::CrossCursor);
    return true;
}

/**
 * @brief Handles press for selection, wire start, and item interaction.
 */
void SandboxView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        const QPointF scenePos = mapToScene(event->position().toPoint());
        if (tryBeginWireAt(scenePos)) {
            event->accept();
            return;
        }
    }

    QGraphicsView::mousePressEvent(event);
    setFocus();
}

/**
 * @brief Updates an in-progress wire drag as the cursor moves.
 */
void SandboxView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_circuitController->isWireDragInProgress()) {
        m_circuitController->updateWireDrag(mapToScene(event->position().toPoint()));
        event->accept();
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

/**
 * @brief Completes or cancels an in-progress wire drag on release.
 */
void SandboxView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_circuitController->isWireDragInProgress()) {
        m_circuitController->endWireDrag(mapToScene(event->position().toPoint()));
        unsetCursor();
        event->accept();
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

/**
 * @brief Handles keyboard shortcuts such as Delete for selection removal.
 */
void SandboxView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        m_circuitController->deleteSelection();
        event->accept();
        return;
    }

    QGraphicsView::keyPressEvent(event);
}

/**
 * @brief Accepts gate-palette drag events over the canvas.
 */
void SandboxView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(QString::fromLatin1(kGateMimeType))) {
        event->acceptProposedAction();
    }
}

/**
 * @brief Updates drag feedback while a gate is dragged over the canvas.
 */
void SandboxView::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat(QString::fromLatin1(kGateMimeType))) {
        event->acceptProposedAction();
    }
}

/**
 * @brief Places a gate when a palette item is dropped on the canvas.
 */
void SandboxView::dropEvent(QDropEvent* event)
{
    if (!event->mimeData()->hasFormat(QString::fromLatin1(kGateMimeType))) {
        return;
    }

    // Palette encodes GateKind as a single byte in the custom MIME payload.
    const QByteArray payload = event->mimeData()->data(QString::fromLatin1(kGateMimeType));
    if (payload.isEmpty()) {
        return;
    }

    const GateKind kind = static_cast<GateKind>(payload.at(0));
    const QPointF scenePos = mapToScene(event->position().toPoint());

    if (m_circuitController->placeGateFromPalette(kind, scenePos)) {
        event->acceptProposedAction();
    }
}

/**
 * @brief Draws the sandbox background grid.
 */
void SandboxView::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, AppTheme::background());

    QPen gridPen(AppTheme::gridLine());
    gridPen.setWidth(1);
    painter->setPen(gridPen);

    // Align grid lines to a 20px lattice clipped to the exposed rect.
    constexpr qreal gridStep = 20.0;
    const qreal left = std::floor(rect.left() / gridStep) * gridStep;
    const qreal top = std::floor(rect.top() / gridStep) * gridStep;

    for (qreal x = left; x < rect.right(); x += gridStep) {
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }
    for (qreal y = top; y < rect.bottom(); y += gridStep) {
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }
}

} // namespace digitallogic::ui
