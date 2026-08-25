/**
 * @file ChallengeDialog.h
 * @brief Declares the dialog used to pick a challenge level.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#pragma once

#include <QDialog>

class QListWidget;

namespace digitallogic::ui {

/**
 * @brief Dialog for selecting a challenge level to play.
 */
class ChallengeDialog final : public QDialog {
    Q_OBJECT

public:
    explicit ChallengeDialog(QWidget* parent = nullptr);

    // -1 until the user accepts a selection from the list.
    [[nodiscard]] int selectedLevelIndex() const noexcept { return m_selectedLevelIndex; }

private:
    void populateLevels();

    QListWidget* m_levelList{nullptr};
    int m_selectedLevelIndex{-1};
};

} // namespace digitallogic::ui
