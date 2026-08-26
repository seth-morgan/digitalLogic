/**
 * @file GatePaletteWidget.cpp
 * @brief Bottom palette of draggable gate buttons with optional challenge budgets.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/ui/GatePaletteWidget.h"

#include "digitallogic/ui/AppTheme.h"

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

/**
 * @brief Constructs the gate palette and its draggable gate buttons.
 */
GatePaletteWidget::GatePaletteWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(12);

    auto* title = new QLabel(QStringLiteral("Gates:"), this);
    title->setStyleSheet(QStringLiteral("color: #9aa3b2; font-weight: 600;"));
    layout->addWidget(title);

    m_gateButtons = {
        {GateKind::And, QStringLiteral("AND"), new QPushButton(QStringLiteral("AND"), this)},
        {GateKind::Or, QStringLiteral("OR"), new QPushButton(QStringLiteral("OR"), this)},
        {GateKind::Not, QStringLiteral("NOT"), new QPushButton(QStringLiteral("NOT"), this)},
        {GateKind::Nand, QStringLiteral("NAND"), new QPushButton(QStringLiteral("NAND"), this)},
        {GateKind::Xor, QStringLiteral("XOR"), new QPushButton(QStringLiteral("XOR"), this)},
    };

    for (GateButtonEntry& entry : m_gateButtons) {
        entry.button->setMinimumSize(88, 42);
        entry.button->setStyleSheet(AppTheme::gateButtonStylesheet(entry.kind));
        layout->addWidget(entry.button);
        // Event filter intercepts presses so we start a drag instead of a click.
        entry.button->installEventFilter(this);
    }

    layout->addStretch(1);
    setMinimumHeight(72);
    setStyleSheet(QStringLiteral("background-color: #1c212b; border-top: 1px solid #2a3140;"));
}

/**
 * @brief Enables or disables challenge-mode budget constraints on the palette.
 */
void GatePaletteWidget::setChallengeMode(const bool enabled)
{
    m_challengeMode = enabled;
    if (!enabled) {
        m_gateBudget.clear();
    }
    refreshAllButtons();
}

/**
 * @brief Updates remaining gate counts and disables exhausted buttons.
 */
void GatePaletteWidget::updateGateBudget(const QHash<GateKind, int>& remaining)
{
    m_gateBudget = remaining;
    refreshAllButtons();
}

/**
 * @brief Refreshes enabled state and label for one palette button.
 */
void GatePaletteWidget::refreshButtonState(GateButtonEntry& entry)
{
    if (entry.button == nullptr) {
        return;
    }

    // Sandbox: all gates enabled. Challenge: only budgeted kinds, with remaining counts.
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

/**
 * @brief Refreshes enabled state for every palette button.
 */
void GatePaletteWidget::refreshAllButtons()
{
    for (GateButtonEntry& entry : m_gateButtons) {
        refreshButtonState(entry);
    }
}

/**
 * @brief Finds the palette entry whose button matches the watched object.
 */
GatePaletteWidget::GateButtonEntry* GatePaletteWidget::findEntry(QObject* watched)
{
    for (GateButtonEntry& entry : m_gateButtons) {
        if (entry.button == watched) {
            return &entry;
        }
    }
    return nullptr;
}

/**
 * @brief Intercepts button press events to start a gate drag.
 */
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

/**
 * @brief Begins a drag-and-drop operation for the given gate kind.
 */
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
