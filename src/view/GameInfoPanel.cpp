#include "GameInfoPanel.h"
#include "../viewmodel/GameViewModel.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

namespace Snooker2D {

GameInfoPanel::GameInfoPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void GameInfoPanel::setViewModel(GameViewModel* viewModel) {
    if (m_viewModel) {
        disconnect(m_viewModel, nullptr, this, nullptr);
    }

    m_viewModel = viewModel;
    if (!m_viewModel) {
        updateWhiteBallPlacementHint();
        updateMessage(QString());
        return;
    }

    connect(m_viewModel, &GameViewModel::currentPlayerChanged,
            this, &GameInfoPanel::updateCurrentPlayer);
    connect(m_viewModel, &GameViewModel::gamePhaseChanged,
            this, &GameInfoPanel::updatePhase);
    connect(m_viewModel, &GameViewModel::whiteBallPlacingChanged,
            this, &GameInfoPanel::updateWhiteBallPlacementHint);
    connect(m_viewModel, &GameViewModel::foulOccurred,
            this, &GameInfoPanel::updateMessage);
    connect(m_viewModel, &GameViewModel::gameOver,
            this, [this](int winner) {
        updateMessage(QStringLiteral("玩家 %1 获胜！").arg(winner));
    });

    updateCurrentPlayer();
    updatePhase();
    updateWhiteBallPlacementHint();
}

void GameInfoPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);

    // 当前玩家指示
    m_playerIndicator = new QLabel(QStringLiteral("当前玩家: --"), this);
    QFont playerFont = m_playerIndicator->font();
    playerFont.setPointSize(12);
    playerFont.setBold(true);
    m_playerIndicator->setFont(playerFont);
    m_playerIndicator->setAlignment(Qt::AlignCenter);

    // 阶段提示
    m_phaseLabel = new QLabel(QStringLiteral("阶段: --"), this);
    QFont phaseFont = m_phaseLabel->font();
    phaseFont.setPointSize(11);
    m_phaseLabel->setFont(phaseFont);
    m_phaseLabel->setAlignment(Qt::AlignCenter);

    // 白球放置提示
    m_placementHintLabel = new QLabel(QStringLiteral("点击 D 区放置白球"), this);
    QFont placementFont = m_placementHintLabel->font();
    placementFont.setPointSize(10);
    placementFont.setBold(true);
    m_placementHintLabel->setFont(placementFont);
    m_placementHintLabel->setAlignment(Qt::AlignCenter);
    m_placementHintLabel->setWordWrap(true);
    m_placementHintLabel->setStyleSheet(QStringLiteral(
        "color: #dfffe0;"
        "background-color: rgba(0, 120, 0, 70);"
        "border: 1px solid rgba(180, 255, 180, 150);"
        "border-radius: 4px;"
        "padding: 6px;"
    ));
    m_placementHintLabel->hide();

    // 其他消息
    m_messageLabel = new QLabel(QString(), this);
    m_messageLabel->setAlignment(Qt::AlignCenter);
    m_messageLabel->setWordWrap(true);

    layout->addWidget(m_playerIndicator);
    layout->addWidget(m_phaseLabel);
    layout->addWidget(m_placementHintLabel);
    layout->addWidget(m_messageLabel);
    layout->addStretch();
}

void GameInfoPanel::updateCurrentPlayer() {
    const int playerNumber = m_viewModel ? m_viewModel->currentPlayer() : 0;
    m_playerIndicator->setText(playerNumber > 0
                                   ? QStringLiteral("当前玩家: %1").arg(playerNumber)
                                   : QStringLiteral("当前玩家: --"));
}

void GameInfoPanel::updatePhase() {
    const QString phaseText = m_viewModel ? m_viewModel->gamePhase() : QString();
    m_phaseLabel->setText(phaseText.isEmpty()
                              ? QStringLiteral("阶段: --")
                              : QStringLiteral("阶段: %1").arg(phaseText));
    m_phaseLabel->setStyleSheet(phaseStyleSheet(phaseText));
}

void GameInfoPanel::updateMessage(const QString& message) {
    m_messageLabel->setText(message);
}

void GameInfoPanel::updateWhiteBallPlacementHint() {
    if (!m_placementHintLabel) {
        return;
    }

    const bool isPlacingWhiteBall = m_viewModel && m_viewModel->isPlacingWhiteBall();
    m_placementHintLabel->setVisible(isPlacingWhiteBall);
}

QString GameInfoPanel::phaseStyleSheet(const QString& phaseText) const {
    if (phaseText.startsWith(QStringLiteral("请击红球"))) {
        return QStringLiteral("color: #dc143c;");
    }
    if (phaseText.startsWith(QStringLiteral("请击彩球"))) {
        return QStringLiteral("color: #4169e1;");
    }
    if (phaseText.startsWith(QStringLiteral("犯规"))) {
        return QStringLiteral("color: #dc143c; font-weight: bold;");
    }
    if (phaseText.startsWith(QStringLiteral("自由球"))) {
        return QStringLiteral("color: #ff9800;");
    }
    if (phaseText.startsWith(QStringLiteral("比赛结束"))) {
        return QStringLiteral("color: #d4af37; font-weight: bold;");
    }
    return QString();
}

} // namespace Snooker2D
