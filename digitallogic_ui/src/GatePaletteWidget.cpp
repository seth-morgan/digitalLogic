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

    m_gateButtons = {
        {GateKind::And, QStringLiteral("AND"), new QPushButton(QStringLiteral("AND"), this)},
        {GateKind::Or, QStringLiteral("OR"), new QPushButton(QStringLiteral("OR"), this)},
        {GateKind::Not, QStringLiteral("NOT"), new QPushButton(QStringLiteral("NOT"), this)},
        {GateKind::Nand, QStringLiteral("NAND"), new QPushButton(QStringLiteral("NAND"), this)},
        {GateKind::Xor, QStringLiteral("XOR"), new QPushButton(QStringLiteral("XOR"), this)},
    };

    for (GateButtonEntry& entry : m_gateButtons) {
        entry.button->setMinimumSize(80, 36);
        layout->addWidget(entry.button);
        entry.button->installEventFilter(this);
    }

    layout->addStretch(1);
    setMinimumHeight(64);
}

void GatePaletteWidget::setChallengeMode(const bool enabled)
{
    m_challengeMode = enabled;
    if (!enabled) {
        m_gateBudget.clear();
    }
    refreshAllButtons();
}

void GatePaletteWidget::updateGateBudget(const QHash<GateKind, int>& remaining)
{
    m_gateBudget = remaining;
    refreshAllButtons();
}

void GatePaletteWidget::refreshButtonState(GateButtonEntry& entry)
{
    if (entry.button == nullptr) {
        return;
    }

    if (!m_challengeMode || !m_gateBudget.contains(entry.kind)) {
        entry.button->setText(entry.label);
        entry.button->setVisible(!m_challengeMode || m_gateBudget.contains(entry.kind));
        entry.button->setEnabled(!m_challengeMode);
        return;
    }

    entry.button->setVisible(true);
    entry.button->setText(QStringLiteral("%1 (%2)").arg(entry.label).arg(m_gateBudget.value(entry.kind)));
    entry.button->setEnabled(m_gateBudget.value(entry.kind) > 0);
}

void GatePaletteWidget::refreshAllButtons()
{
    for (GateButtonEntry& entry : m_gateButtons) {
        refreshButtonState(entry);
    }
}

GatePaletteWidget::GateButtonEntry* GatePaletteWidget::findEntry(QObject* watched)
{
    for (GateButtonEntry& entry : m_gateButtons) {
        if (entry.button == watched) {
            return &entry;
        }
    }
    return nullptr;
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

    GateButtonEntry* entry = findEntry(watched);
    if (entry == nullptr || entry->button == nullptr || !entry->button->isEnabled()) {
        return QWidget::eventFilter(watched, event);
    }

    startGateDrag(entry->kind, entry->button);
    return true;
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
