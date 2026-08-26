/**
 * @file ChallengeDialog.cpp
 * @brief Modal dialog for selecting a challenge level from the catalog.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/ui/ChallengeDialog.h"

#include "digitallogic/challenge/ChallengeCatalog.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

namespace digitallogic::ui {

/**
 * @brief Constructs the challenge level picker dialog.
 */
ChallengeDialog::ChallengeDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("Challenge Mode"));
    resize(480, 320);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(QStringLiteral("Select a level to begin:"), this));

    m_levelList = new QListWidget(this);
    populateLevels();
    layout->addWidget(m_levelList, 1);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);

    // Accept only when a row is selected; store that index for the caller.
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (m_levelList->currentRow() >= 0) {
            m_selectedLevelIndex = m_levelList->currentRow();
            accept();
        }
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_levelList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        if (item != nullptr) {
            m_selectedLevelIndex = m_levelList->row(item);
            accept();
        }
    });

    if (m_levelList->count() > 0) {
        m_levelList->setCurrentRow(0);
    }
}

/**
 * @brief Populates the list widget from the challenge catalog.
 */
void ChallengeDialog::populateLevels()
{
    const QVector<ChallengeLevel>& levels = ChallengeCatalog::levels();
    for (const ChallengeLevel& level : levels) {
        auto* item = new QListWidgetItem(level.title, m_levelList);
        item->setToolTip(level.description);
        item->setData(Qt::UserRole, level.index);
    }
}

} // namespace digitallogic::ui
