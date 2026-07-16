#include "GameControlPanel.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace Snooker2D {

GameControlPanel::GameControlPanel(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void GameControlPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto* titleLabel = new QLabel(QStringLiteral("游戏控制"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(11);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);

    m_settingsButton = new QPushButton(QStringLiteral("设置"), this);
    m_settingsButton->setCursor(Qt::PointingHandCursor);
    m_settingsButton->setToolTip(QStringLiteral("设置功能暂未开放"));
    m_settingsButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "background-color: #455a64;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "padding: 8px 12px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #546e7a; }"
        "QPushButton:pressed { background-color: #37474f; }"
    ));

    m_restartButton = new QPushButton(QStringLiteral("重启游戏"), this);
    m_restartButton->setCursor(Qt::PointingHandCursor);
    m_restartButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "background-color: #3f7f5f;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "padding: 8px 12px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #4f9a73; }"
        "QPushButton:pressed { background-color: #34694f; }"
    ));
    connect(m_restartButton, &QPushButton::clicked,
            this, &GameControlPanel::restartRequested);

    buttonLayout->addWidget(m_settingsButton);
    buttonLayout->addWidget(m_restartButton);
    layout->addWidget(titleLabel);
    layout->addLayout(buttonLayout);
}

} // namespace Snooker2D
