#pragma once

#include <QWidget>

class QGraphicsOpacityEffect;
class QLabel;

namespace digitallogic::ui {

/**
 * @brief Full-window flash overlay shown when a challenge level is completed.
 */
class WinCelebrationOverlay final : public QWidget {
    Q_OBJECT

public:
    explicit WinCelebrationOverlay(QWidget* parent = nullptr);

    void play();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void finishCelebration();

    QLabel* m_banner{nullptr};
    QGraphicsOpacityEffect* m_opacityEffect{nullptr};
    qreal m_flashStrength{0.0};
    bool m_active{false};
};

} // namespace digitallogic::ui
