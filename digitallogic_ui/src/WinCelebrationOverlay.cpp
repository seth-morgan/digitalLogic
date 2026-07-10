#include "digitallogic/ui/WinCelebrationOverlay.h"

#include "digitallogic/ui/AppTheme.h"

#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QPainter>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>

namespace digitallogic::ui {

WinCelebrationOverlay::WinCelebrationOverlay(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setVisible(false);
    setAutoFillBackground(false);

    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    m_banner = new QLabel(QStringLiteral("LEVEL COMPLETE!"), this);
    m_banner->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_banner->setAlignment(Qt::AlignCenter);
    m_banner->setStyleSheet(QStringLiteral(
        "font-size: 42px; font-weight: 900; letter-spacing: 2px;"
        "color: #fef08a; background: transparent;"));
    layout->addWidget(m_banner);

    m_opacityEffect = new QGraphicsOpacityEffect(m_banner);
    m_banner->setGraphicsEffect(m_opacityEffect);
    m_opacityEffect->setOpacity(0.0);
}

void WinCelebrationOverlay::play()
{
    raise();
    setVisible(true);
    m_active = true;
    m_flashStrength = 1.0;
    m_opacityEffect->setOpacity(0.0);

    auto* fadeIn = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    fadeIn->setDuration(450);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::OutBack);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

    QTimer::singleShot(1600, this, [this]() {
        auto* fadeOut = new QPropertyAnimation(m_opacityEffect, "opacity", this);
        fadeOut->setDuration(500);
        fadeOut->setStartValue(m_opacityEffect->opacity());
        fadeOut->setEndValue(0.0);
        connect(fadeOut, &QPropertyAnimation::finished, this, &WinCelebrationOverlay::finishCelebration);
        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    });

    auto* flashTimer = new QTimer(this);
    connect(flashTimer, &QTimer::timeout, this, [this, flashTimer]() {
        m_flashStrength = qMax(0.0, m_flashStrength - 0.08);
        update();
        if (m_flashStrength <= 0.0) {
            flashTimer->stop();
            flashTimer->deleteLater();
        }
    });
    flashTimer->start(40);

    update();
}

void WinCelebrationOverlay::finishCelebration()
{
    m_active = false;
    setVisible(false);
}

void WinCelebrationOverlay::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (parentWidget() != nullptr) {
        setGeometry(parentWidget()->rect());
    }
}

void WinCelebrationOverlay::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    if (!m_active || m_flashStrength <= 0.0) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QColor flash = AppTheme::signalTrue();
    flash.setAlphaF(m_flashStrength * 0.35);
    painter.fillRect(rect(), flash);
}

} // namespace digitallogic::ui
