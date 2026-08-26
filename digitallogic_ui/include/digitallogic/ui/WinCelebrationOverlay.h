/**
 * @file WinCelebrationOverlay.h
 * @brief Declares the full-window flash overlay shown on challenge completion.
 * @author Seth Morgan
 * @date 2026-08-25
 */
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
    /**
     * @brief Constructs the full-window win celebration overlay.
     */
    explicit WinCelebrationOverlay(QWidget* parent = nullptr);

    /**
     * @brief Plays the celebration flash and banner animation.
     */
    void play();

protected:
    /**
     * @brief Keeps overlay children sized to the parent window.
     */
    void resizeEvent(QResizeEvent* event) override;
    /**
     * @brief Paints the full-window color flash during celebration.
     */
    void paintEvent(QPaintEvent* event) override;

private:
    /**
     * @brief Hides the overlay and resets celebration state when finished.
     */
    void finishCelebration();

    QLabel* m_banner{nullptr};
    QGraphicsOpacityEffect* m_opacityEffect{nullptr};
    // 0..1 strength of the full-window color flash during the celebration.
    qreal m_flashStrength{0.0};
    bool m_active{false};
};

} // namespace digitallogic::ui
