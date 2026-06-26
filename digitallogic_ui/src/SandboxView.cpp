#include "digitallogic/ui/SandboxView.h"

#include "digitallogic/model/Gate.h"
#include "digitallogic/ui/CircuitController.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsScene>
#include <QMimeData>
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
    setAcceptDrops(true);
    setDragMode(QGraphicsView::RubberBandDrag);

    m_circuitController = new CircuitController(this, this);
    m_circuitController->initializeDefaultSources();
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
