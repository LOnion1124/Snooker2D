#include "ScoreBoard.h"
#include "../common/Types.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QFont>
#include <QTimer>

namespace Snooker2D {

ScoreBoard::ScoreBoard(QWidget* parent) : QWidget(parent) {
    setupUI();

    // 犯规提示 3 秒后自动隐藏
    m_foulTimer = new QTimer(this);
    m_foulTimer->setSingleShot(true);
    m_foulTimer->setInterval(3000);
    connect(m_foulTimer, &QTimer::timeout, this, &ScoreBoard::onFoulTimerTimeout);
}

void ScoreBoard::applyScoreState(const ScoreViewState& state) {
    m_state = state;
    m_hasState = true;
    m_foulVisible = state.foulType != static_cast<int>(FoulType::None);
    refreshTexts();

    // 有犯规消息时启动 3 秒隐藏定时器
    if (state.foulType != static_cast<int>(FoulType::None)) {
        m_foulTimer->start();
    }
}

void ScoreBoard::setLanguage(UiLanguage language) {
    if (m_language == language) return;
    m_language = language;
    refreshTexts();
}

void ScoreBoard::onFoulTimerTimeout() {
    m_foulVisible = false;
    m_foulLabel->clear();
}

void ScoreBoard::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    m_titleLabel = new QLabel(this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_player1NameLabel = new QLabel(this);
    m_player1NameLabel->setAlignment(Qt::AlignCenter);
    m_player1ScoreLabel = new QLabel(this);
    QFont scoreFont = m_player1ScoreLabel->font();
    scoreFont.setPointSize(24);
    scoreFont.setBold(true);
    m_player1ScoreLabel->setFont(scoreFont);
    m_player1ScoreLabel->setAlignment(Qt::AlignCenter);
    m_player1BreakLabel = new QLabel(this);
    m_player1BreakLabel->setAlignment(Qt::AlignCenter);

    m_player2NameLabel = new QLabel(this);
    m_player2NameLabel->setAlignment(Qt::AlignCenter);
    m_player2ScoreLabel = new QLabel(this);
    m_player2ScoreLabel->setFont(scoreFont);
    m_player2ScoreLabel->setAlignment(Qt::AlignCenter);
    m_player2BreakLabel = new QLabel(this);
    m_player2BreakLabel->setAlignment(Qt::AlignCenter);

    m_foulLabel = new QLabel(this);
    m_foulLabel->setStyleSheet(QStringLiteral("color: red; font-weight: bold;"));
    m_foulLabel->setAlignment(Qt::AlignCenter);
    m_foulLabel->setWordWrap(true);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);

    auto* player1Layout = new QVBoxLayout();
    player1Layout->setSpacing(4);
    player1Layout->addWidget(m_player1NameLabel);
    player1Layout->addWidget(m_player1ScoreLabel);
    player1Layout->addWidget(m_player1BreakLabel);

    auto* player2Layout = new QVBoxLayout();
    player2Layout->setSpacing(4);
    player2Layout->addWidget(m_player2NameLabel);
    player2Layout->addWidget(m_player2ScoreLabel);
    player2Layout->addWidget(m_player2BreakLabel);

    auto* playersLayout = new QHBoxLayout();
    playersLayout->setSpacing(18);
    playersLayout->addLayout(player1Layout, 1);
    playersLayout->addLayout(player2Layout, 1);

    layout->addWidget(m_titleLabel);
    layout->addLayout(playersLayout);
    layout->addWidget(m_foulLabel);
    layout->addWidget(m_statusLabel);
    refreshTexts();
}

void ScoreBoard::refreshTexts() {
    const bool english = m_language == UiLanguage::English;
    m_titleLabel->setText(english ? QStringLiteral("Scoreboard") : QStringLiteral("计分板"));
    m_player1NameLabel->setText(english ? QStringLiteral("Player 1") : QStringLiteral("玩家 1"));
    m_player2NameLabel->setText(english ? QStringLiteral("Player 2") : QStringLiteral("玩家 2"));

    m_player1ScoreLabel->setText(QString::number(m_state.player1Score));
    m_player2ScoreLabel->setText(QString::number(m_state.player2Score));
    m_player1BreakLabel->setText(english
        ? QStringLiteral("Break: %1").arg(m_state.player1Break)
        : QStringLiteral("单杆: %1").arg(m_state.player1Break));
    m_player2BreakLabel->setText(english
        ? QStringLiteral("Break: %1").arg(m_state.player2Break)
        : QStringLiteral("单杆: %1").arg(m_state.player2Break));

    if (m_foulVisible && m_state.foulType != static_cast<int>(FoulType::None)) {
        const QString playerName = (m_state.foulingPlayer == 2)
            ? (english ? QStringLiteral("Player 2") : QStringLiteral("玩家 2"))
            : (english ? QStringLiteral("Player 1") : QStringLiteral("玩家 1"));
        const QString foulMessage = translatedFoulText(
            m_state.foulType, m_state.foulPenaltyPoints, m_language);
        m_foulLabel->setText(english
            ? QStringLiteral("%1 foul: %2").arg(playerName, foulMessage)
            : QStringLiteral("%1 犯规: %2").arg(playerName, foulMessage));
    } else {
        m_foulLabel->clear();
    }

    if (!m_hasState) {
        m_statusLabel->setText(english ? QStringLiteral("Waiting to start...") : QStringLiteral("等待开始..."));
        return;
    }
    m_statusLabel->setText(translatedMessage(m_state.statusMessage, m_language));
}

} // namespace Snooker2D
