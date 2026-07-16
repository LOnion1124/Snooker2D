#include "GameInfoPanel.h"
#include "contracts/GameViewState.h"
#include "../common/Types.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

namespace Snooker2D {

GameInfoPanel::GameInfoPanel(QWidget* parent) : QWidget(parent) { setupUI(); }

void GameInfoPanel::applyGameInfoState(const GameInfoViewState& state) {
    m_playerIndicator->setText(state.currentPlayer > 0
        ? QStringLiteral("当前玩家: %1").arg(state.currentPlayer)
        : QStringLiteral("当前玩家: --"));

    const bool phaseEmpty = state.phaseText.isEmpty();
    m_phaseLabel->setText(phaseEmpty
        ? QStringLiteral("阶段: --")
        : QStringLiteral("阶段: %1").arg(state.phaseText));
    m_phaseLabel->setStyleSheet(phaseStyleSheet(state.phaseKind));

    m_messageLabel->setText(state.message);

    if (m_placementHintLabel) {
        m_placementHintLabel->setVisible(state.showWhiteBallPlacementHint);
    }
}

void GameInfoPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);

    m_playerIndicator = new QLabel(QStringLiteral("当前玩家: --"), this);
    QFont playerFont = m_playerIndicator->font();
    playerFont.setPointSize(12);
    playerFont.setBold(true);
    m_playerIndicator->setFont(playerFont);
    m_playerIndicator->setAlignment(Qt::AlignCenter);

    m_phaseLabel = new QLabel(QStringLiteral("阶段: --"), this);
    QFont phaseFont = m_phaseLabel->font();
    phaseFont.setPointSize(11);
    m_phaseLabel->setFont(phaseFont);
    m_phaseLabel->setAlignment(Qt::AlignCenter);

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

    m_messageLabel = new QLabel(QString(), this);
    m_messageLabel->setAlignment(Qt::AlignCenter);
    m_messageLabel->setWordWrap(true);

    layout->addWidget(m_playerIndicator);
    layout->addWidget(m_phaseLabel);
    layout->addWidget(m_placementHintLabel);
    layout->addWidget(m_messageLabel);
    layout->addStretch();
}

QString GameInfoPanel::phaseStyleSheet(int phaseKind) const {
    switch (static_cast<GamePhase>(phaseKind)) {
        case GamePhase::RedBall:   return QStringLiteral("color: #dc143c;");
        case GamePhase::ColorBall: return QStringLiteral("color: #4169e1;");
        case GamePhase::Foul:      return QStringLiteral("color: #dc143c; font-weight: bold;");
        case GamePhase::FreeBall:  return QStringLiteral("color: #ff9800;");
        case GamePhase::GameOver:  return QStringLiteral("color: #d4af37; font-weight: bold;");
        default: return QString();
    }
}

} // namespace Snooker2D
