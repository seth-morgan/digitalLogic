#include "digitallogic/ui/AppTheme.h"

namespace digitallogic::ui {

QColor AppTheme::background()
{
    return QColor(QStringLiteral("#12151c"));
}

QColor AppTheme::surface()
{
    return QColor(QStringLiteral("#1c212b"));
}

QColor AppTheme::surfaceElevated()
{
    return QColor(QStringLiteral("#262c38"));
}

QColor AppTheme::textPrimary()
{
    return QColor(QStringLiteral("#eef1f6"));
}

QColor AppTheme::textMuted()
{
    return QColor(QStringLiteral("#9aa3b2"));
}

QColor AppTheme::gridLine()
{
    return QColor(QStringLiteral("#2a3140"));
}

QColor AppTheme::wireIdle()
{
    return QColor(QStringLiteral("#5c6678"));
}

QColor AppTheme::componentFill()
{
    return QColor(QStringLiteral("#313846"));
}

QColor AppTheme::componentBorder()
{
    return QColor(QStringLiteral("#5c6678"));
}

QColor AppTheme::selection()
{
    return QColor(QStringLiteral("#fbbf24"));
}

QColor AppTheme::targetAccent()
{
    return QColor(QStringLiteral("#f59e0b"));
}

QColor AppTheme::signalTrue()
{
    return QColor(QStringLiteral("#22c55e"));
}

QColor AppTheme::signalFalse()
{
    return QColor(QStringLiteral("#ef4444"));
}

QColor AppTheme::gateAccent(const GateKind kind)
{
    switch (kind) {
    case GateKind::And:
        return QColor(QStringLiteral("#3b82f6"));
    case GateKind::Or:
        return QColor(QStringLiteral("#a855f7"));
    case GateKind::Not:
        return QColor(QStringLiteral("#f97316"));
    case GateKind::Nand:
        return QColor(QStringLiteral("#14b8a6"));
    case GateKind::Xor:
        return QColor(QStringLiteral("#ec4899"));
    default:
        return QColor(QStringLiteral("#64748b"));
    }
}

QString AppTheme::applicationStylesheet()
{
    return QStringLiteral(
        "QMainWindow, QWidget { background-color: #12151c; color: #eef1f6; }"
        "QToolBar { background: #1c212b; border: none; spacing: 8px; padding: 6px; }"
        "QStatusBar { background: #1c212b; color: #9aa3b2; border-top: 1px solid #2a3140; }"
        "QLabel { color: #eef1f6; }"
        "QDialog { background-color: #1c212b; color: #eef1f6; }"
        "QListWidget { background: #262c38; border: 1px solid #3b4354; border-radius: 8px; color: #eef1f6; }"
        "QListWidget::item { padding: 8px; border-radius: 6px; }"
        "QListWidget::item:selected { background: #3b82f6; }"
        "QDialogButtonBox QPushButton { min-height: 32px; padding: 6px 16px; border-radius: 8px; }");
}

QString AppTheme::toolbarButtonStylesheet()
{
    return QStringLiteral(
        "QToolButton, QPushButton {"
        "  background-color: #313846;"
        "  color: #eef1f6;"
        "  border: none;"
        "  border-bottom: 3px solid #1a1f28;"
        "  border-radius: 8px;"
        "  padding: 8px 14px;"
        "  font-weight: 600;"
        "}"
        "QToolButton:hover, QPushButton:hover {"
        "  background-color: #3d4657;"
        "  margin-top: -1px;"
        "}"
        "QToolButton:pressed, QPushButton:pressed {"
        "  background-color: #262c38;"
        "  border-bottom-width: 1px;"
        "  padding-top: 2px;"
        "}");
}

QString AppTheme::primaryActionStylesheet()
{
    return QStringLiteral(
        "QPushButton {"
        "  background-color: #22c55e;"
        "  color: #052e16;"
        "  border: none;"
        "  border-bottom: 4px solid #15803d;"
        "  border-radius: 10px;"
        "  padding: 10px 18px;"
        "  font-weight: 700;"
        "}"
        "QPushButton:hover { background-color: #4ade80; margin-top: -1px; }"
        "QPushButton:pressed { background-color: #16a34a; border-bottom-width: 1px; padding-top: 3px; }"
        "QPushButton:disabled { background-color: #313846; color: #6b7280; border-bottom-color: #1a1f28; }");
}

QString AppTheme::gateButtonStylesheet(const GateKind kind)
{
    const QColor accent = gateAccent(kind);
    const QColor shadow = accent.darker(140);
    const QColor hover = accent.lighter(110);

    return QStringLiteral(
               "QPushButton {"
               "  background-color: %1;"
               "  color: white;"
               "  border: none;"
               "  border-bottom: 4px solid %2;"
               "  border-radius: 10px;"
               "  font-weight: 700;"
               "  font-size: 13px;"
               "}"
               "QPushButton:hover { background-color: %3; margin-top: -2px; }"
               "QPushButton:pressed { background-color: %2; border-bottom-width: 1px; padding-top: 3px; }"
               "QPushButton:disabled { background-color: #313846; color: #6b7280; border-bottom-color: #1a1f28; }")
        .arg(accent.name(), shadow.name(), hover.name());
}

} // namespace digitallogic::ui
