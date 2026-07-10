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

void GatePaletteWidget::setChallengeMode(const bool enabled)
{
    m_challengeMode = enabled;
    if (!enabled) {
        m_gateBudget.clear();
        refreshButtonState(m_andButton, GateKind::And, QStringLiteral("AND"));
        refreshButtonState(m_orButton, GateKind::Or, QStringLiteral("OR"));
        refreshButtonState(m_notButton, GateKind::Not, QStringLiteral("NOT"));
        m_andButton->setEnabled(true);
        m_orButton->setEnabled(true);
        m_notButton->setEnabled(true);
    }
}

void GatePaletteWidget::updateGateBudget(const QHash<GateKind, int>& remaining)
{
    m_gateBudget = remaining;
    if (!m_challengeMode) {
        return;
    }

    refreshButtonState(m_andButton, GateKind::And, QStringLiteral("AND"));
    refreshButtonState(m_orButton, GateKind::Or, QStringLiteral("OR"));
    refreshButtonState(m_notButton, GateKind::Not, QStringLiteral("NOT"));

    m_andButton->setEnabled(m_gateBudget.value(GateKind::And, 0) > 0);
    m_orButton->setEnabled(m_gateBudget.value(GateKind::Or, 0) > 0);
    m_notButton->setEnabled(m_gateBudget.value(GateKind::Not, 0) > 0);
}

void GatePaletteWidget::refreshButtonState(QPushButton* button, const GateKind kind, const QString& baseLabel)
{
    if (button == nullptr) {
        return;
    }

    if (!m_challengeMode || !m_gateBudget.contains(kind)) {
        button->setText(baseLabel);
        button->setVisible(!m_challengeMode || m_gateBudget.contains(kind));
        return;
    }

    button->setVisible(true);
    button->setText(QStringLiteral("%1 (%2)").arg(baseLabel).arg(m_gateBudget.value(kind)));
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
        if (!m_andButton->isEnabled()) {
            return true;
        }
        startGateDrag(GateKind::And, m_andButton);
        return true;
    }
    if (watched == m_orButton) {
        if (!m_orButton->isEnabled()) {
            return true;
        }
        startGateDrag(GateKind::Or, m_orButton);
        return true;
    }
    if (watched == m_notButton) {
        if (!m_notButton->isEnabled()) {
            return true;
        }
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
