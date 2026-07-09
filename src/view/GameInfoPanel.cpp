#include "GameInfoPanel.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

namespace Snooker2D {

GameInfoPanel::GameInfoPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void GameInfoPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);

    // 当前玩家指示
    m_playerIndicator = new QLabel("当前玩家: --", this);
    QFont playerFont = m_playerIndicator->font();
    playerFont.setPointSize(12);
    playerFont.setBold(true);
    m_playerIndicator->setFont(playerFont);
    m_playerIndicator->setAlignment(Qt::AlignCenter);

    // 阶段提示
    m_phaseLabel = new QLabel("阶段: --", this);
    QFont phaseFont = m_phaseLabel->font();
    phaseFont.setPointSize(11);
    m_phaseLabel->setFont(phaseFont);
    m_phaseLabel->setAlignment(Qt::AlignCenter);

    // 其他消息
    m_messageLabel = new QLabel("", this);
    m_messageLabel->setAlignment(Qt::AlignCenter);
    m_messageLabel->setWordWrap(true);

    layout->addWidget(m_playerIndicator);
    layout->addWidget(m_phaseLabel);
    layout->addWidget(m_messageLabel);
    layout->addStretch();
}

void GameInfoPanel::setCurrentPlayer(int playerNumber) {
    m_playerIndicator->setText(QString("当前玩家: %1").arg(playerNumber));
}

void GameInfoPanel::setPhase(const QString& phaseText) {
    m_phaseLabel->setText(QString("阶段: %1").arg(phaseText));
}

void GameInfoPanel::setMessage(const QString& message) {
    m_messageLabel->setText(message);
}

} // namespace Snooker2D
