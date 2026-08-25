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
    explicit ChallengePanelWidget(QWidget* parent = nullptr);

    void showLevel(const QString& title, const QString& description, bool hasNextLevel);
    // Enables Next Level only after a successful check.
    void setLevelSolved(bool solved);
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
