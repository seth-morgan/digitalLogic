#include "digitallogic/ui/GatePaletteWidget.h"

#include <QDrag>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QMimeData>
#include <QPushButton>

namespace digitallogic::ui {

namespace {
constexpr const char* kGateMimeType = "application/x-digitallogic-gate";
}

GatePaletteWidget::GatePaletteWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(12);

    auto* title = new QLabel(QStringLiteral("Gates:"), this);
    layout->addWidget(title);

    m_andButton = new QPushButton(QStringLiteral("AND"), this);
    m_orButton = new QPushButton(QStringLiteral("OR"), this);
    m_notButton = new QPushButton(QStringLiteral("NOT"), this);

    for (QPushButton* button : {m_andButton, m_orButton, m_notButton}) {
        button->setMinimumSize(80, 36);
        layout->addWidget(button);
        button->installEventFilter(this);
    }

    layout->addStretch(1);
    setMinimumHeight(64);
}

bool GatePaletteWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() != QEvent::MouseButtonPress) {
        return QWidget::eventFilter(watched, event);
    }

    auto* mouseEvent = static_cast<QMouseEvent*>(event);
    if (mouseEvent->button() != Qt::LeftButton) {
        return QWidget::eventFilter(watched, event);
    }

    if (watched == m_andButton) {
        startGateDrag(GateKind::And, m_andButton);
        return true;
    }
    if (watched == m_orButton) {
        startGateDrag(GateKind::Or, m_orButton);
        return true;
    }
    if (watched == m_notButton) {
        startGateDrag(GateKind::Not, m_notButton);
        return true;
    }

    return QWidget::eventFilter(watched, event);
}

void GatePaletteWidget::startGateDrag(const GateKind kind, QWidget* sourceWidget)
{
    auto* mimeData = new QMimeData();
    QByteArray payload;
    payload.append(static_cast<char>(kind));
    mimeData->setData(QString::fromLatin1(kGateMimeType), payload);

    auto* drag = new QDrag(sourceWidget);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction);
    emit gateDragStarted(kind);
}

} // namespace digitallogic::ui
