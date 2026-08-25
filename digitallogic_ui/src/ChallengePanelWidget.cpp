/**
 * @file ChallengePanelWidget.cpp
 * @brief Top banner showing challenge title, description, and check/next/exit actions.
 * @author Seth Morgan
 * @date 2026-08-25
 */
#include "digitallogic/ui/ChallengePanelWidget.h"

#include "digitallogic/model/Gate.h"
#include "digitallogic/ui/AppTheme.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace digitallogic::ui {

ChallengePanelWidget::ChallengePanelWidget(QWidget* parent)
    : QWidget(parent)
{
    setStyleSheet(QStringLiteral("background-color: #262c38; border-bottom: 1px solid #2a3140;"));

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(12);

    auto* textColumn = new QVBoxLayout();
    m_titleLabel = new QLabel(this);
    m_titleLabel->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 15px; color: #eef1f6;"));
    m_descriptionLabel = new QLabel(this);
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setStyleSheet(QStringLiteral("color: #9aa3b2; font-size: 13px;"));
    textColumn->addWidget(m_titleLabel);
    textColumn->addWidget(m_descriptionLabel);
    layout->addLayout(textColumn, 1);

    m_checkButton = new QPushButton(QStringLiteral("Check Solution"), this);
    m_nextLevelButton = new QPushButton(QStringLiteral("Next Level"), this);
    m_exitButton = new QPushButton(QStringLiteral("Exit Challenge"), this);
    m_checkButton->setStyleSheet(AppTheme::primaryActionStylesheet());
    m_nextLevelButton->setStyleSheet(AppTheme::gateButtonStylesheet(GateKind::Xor));
    m_exitButton->setStyleSheet(AppTheme::toolbarButtonStylesheet());
    // Next Level stays hidden/disabled until the current level is solved.
    m_nextLevelButton->setVisible(false);
    m_nextLevelButton->setEnabled(false);

    layout->addWidget(m_checkButton);
    layout->addWidget(m_nextLevelButton);
    layout->addWidget(m_exitButton);

    connect(m_checkButton, &QPushButton::clicked, this, &ChallengePanelWidget::checkSolutionRequested);
    connect(m_nextLevelButton, &QPushButton::clicked, this, &ChallengePanelWidget::nextLevelRequested);
    connect(m_exitButton, &QPushButton::clicked, this, &ChallengePanelWidget::exitChallengeRequested);

    setVisible(false);
}

void ChallengePanelWidget::showLevel(const QString& title, const QString& description, const bool hasNextLevel)
{
    m_titleLabel->setText(title);
    m_descriptionLabel->setText(description);
    m_nextLevelButton->setVisible(hasNextLevel);
    m_nextLevelButton->setEnabled(false);
    setLevelSolved(false);
}

void ChallengePanelWidget::setLevelSolved(const bool solved)
{
    m_nextLevelButton->setEnabled(solved);
}

void ChallengePanelWidget::setVisibleForChallenge(const bool visible)
{
    setVisible(visible);
}

} // namespace digitallogic::ui
