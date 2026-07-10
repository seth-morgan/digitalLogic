#include "digitallogic/ui/SandboxView.h"

#include "digitallogic/model/Gate.h"
#include "digitallogic/ui/CircuitController.h"
#include "digitallogic/ui/graphics/PinGraphicsItem.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <cmath>
#include <QPainter>

namespace digitallogic::ui {

namespace {
constexpr const char* kGateMimeType = "application/x-digitallogic-gate";
}

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

    m_circuitController = new CircuitController(this, this);
    m_circuitController->initializeDefaultSources();
}

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

void SandboxView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_circuitController->isWireDragInProgress()) {
        m_circuitController->updateWireDrag(mapToScene(event->position().toPoint()));
        event->accept();
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

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

void SandboxView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        m_circuitController->deleteSelection();
        event->accept();
        return;
    }

    QGraphicsView::keyPressEvent(event);
}

void SandboxView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(QString::fromLatin1(kGateMimeType))) {
        event->acceptProposedAction();
    }
}

void SandboxView::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat(QString::fromLatin1(kGateMimeType))) {
        event->acceptProposedAction();
    }
}

void SandboxView::dropEvent(QDropEvent* event)
{
    if (!event->mimeData()->hasFormat(QString::fromLatin1(kGateMimeType))) {
        return;
    }

    const QByteArray payload = event->mimeData()->data(QString::fromLatin1(kGateMimeType));
    const GateKind kind = static_cast<GateKind>(payload.at(0));
    const QPointF scenePos = mapToScene(event->position().toPoint());

    if (m_circuitController->placeGateFromPalette(kind, scenePos)) {
        event->acceptProposedAction();
    }
}

void SandboxView::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, QColor(QStringLiteral("#fafafa")));

    QPen gridPen(QColor(QStringLiteral("#e0e0e0")));
    gridPen.setWidth(1);
    painter->setPen(gridPen);

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
