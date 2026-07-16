#include "GameInfoPanel.h"
#include "contracts/GameViewState.h"
#include "../common/Types.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

namespace Snooker2D {

GameInfoPanel::GameInfoPanel(QWidget* parent) : QWidget(parent) { setupUI(); }

void GameInfoPanel::applyGameInfoState(const GameInfoViewState& state) {
    m_state = state;
    m_hasState = true;
    refreshTexts();
    m_phaseLabel->setStyleSheet(phaseStyleSheet(state.phaseKind));

    if (m_placementHintLabel) {
        m_placementHintLabel->setVisible(state.showWhiteBallPlacementHint);
    }
}

void GameInfoPanel::setLanguage(UiLanguage language) {
    if (m_language == language) return;
    m_language = language;
    refreshTexts();
}

void GameInfoPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);

    m_playerIndicator = new QLabel(this);
    QFont playerFont = m_playerIndicator->font();
    playerFont.setPointSize(12);
    playerFont.setBold(true);
    m_playerIndicator->setFont(playerFont);
    m_playerIndicator->setAlignment(Qt::AlignCenter);

    m_phaseLabel = new QLabel(this);
    QFont phaseFont = m_phaseLabel->font();
    phaseFont.setPointSize(11);
    m_phaseLabel->setFont(phaseFont);
    m_phaseLabel->setAlignment(Qt::AlignCenter);

    m_placementHintLabel = new QLabel(this);
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
    refreshTexts();
}

void GameInfoPanel::refreshTexts() {
    const bool english = m_language == UiLanguage::English;
    const int currentPlayer = m_hasState ? m_state.currentPlayer : 0;

    m_playerIndicator->setText(currentPlayer > 0
        ? (english
            ? QStringLiteral("Current player: %1").arg(currentPlayer)
            : QStringLiteral("当前玩家: %1").arg(currentPlayer))
        : (english ? QStringLiteral("Current player: --") : QStringLiteral("当前玩家: --")));

    QString phaseText;
    if (m_hasState) {
        const bool isSimulating = m_state.phaseText.endsWith(QStringLiteral(" (模拟中...)"));
        phaseText = english
            ? translatedPhaseText(m_state.phaseKind, isSimulating, m_language)
            : m_state.phaseText;
    }
    m_phaseLabel->setText(phaseText.isEmpty()
        ? (english ? QStringLiteral("Phase: --") : QStringLiteral("阶段: --"))
        : (english
            ? QStringLiteral("Phase: %1").arg(phaseText)
            : QStringLiteral("阶段: %1").arg(phaseText)));

    m_placementHintLabel->setText(english
        ? QStringLiteral("Click inside the D to place the cue ball")
        : QStringLiteral("点击 D 区放置白球"));
    m_messageLabel->setText(translatedMessage(m_state.message, m_language));
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
