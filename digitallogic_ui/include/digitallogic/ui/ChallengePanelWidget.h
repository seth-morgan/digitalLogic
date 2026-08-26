/**
 * @file ChallengePanelWidget.h
 * @brief Declares the in-game panel for challenge level info and actions.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include <QWidget>

class QLabel;
class QPushButton;

namespace digitallogic::ui {

/**
 * @brief In-game panel showing challenge level info and actions.
 */
class ChallengePanelWidget final : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs the in-game challenge info and action panel.
     */
    explicit ChallengePanelWidget(QWidget* parent = nullptr);

    /**
     * @brief Displays title, description, and next-level availability for a level.
     */
    void showLevel(const QString& title, const QString& description, bool hasNextLevel);
    /**
     * @brief Enables or disables the Next Level action after a solve check.
     */
    // Enables Next Level only after a successful check.
    void setLevelSolved(bool solved);
    /**
     * @brief Shows or hides the panel for challenge mode.
     */
    void setVisibleForChallenge(bool visible);

signals:
    void checkSolutionRequested();
    void nextLevelRequested();
    void exitChallengeRequested();

private:
    QLabel* m_titleLabel{nullptr};
    QLabel* m_descriptionLabel{nullptr};
    QPushButton* m_checkButton{nullptr};
    QPushButton* m_nextLevelButton{nullptr};
    QPushButton* m_exitButton{nullptr};
};

} // namespace digitallogic::ui
