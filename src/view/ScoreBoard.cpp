#include "ScoreBoard.h"

#include <QLabel>
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
    m_player1ScoreLabel->setText(QString::number(state.player1Score));
    m_player2ScoreLabel->setText(QString::number(state.player2Score));
    m_player1BreakLabel->setText(QString("单杆: %1").arg(state.player1Break));
    m_player2BreakLabel->setText(QString("单杆: %1").arg(state.player2Break));
    m_foulLabel->setText(state.foulMessage);
    m_statusLabel->setText(state.statusMessage);

    // 有犯规消息时拼上中文犯规者名，启动 3 秒隐藏定时器
    if (!state.foulMessage.isEmpty()) {
        const QString playerName = (state.foulingPlayer == 2)
            ? QStringLiteral("玩家 2")
            : QStringLiteral("玩家 1");
        m_foulLabel->setText(
            QStringLiteral("%1 犯规: %2").arg(playerName, state.foulMessage));
        m_foulTimer->start();
    }
}

void ScoreBoard::onFoulTimerTimeout() {
    m_foulLabel->clear();
}

void ScoreBoard::setupUI() {
    auto* layout = new QVBoxLayout(this);

    m_titleLabel = new QLabel("计分板", this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_player1NameLabel = new QLabel("玩家 1", this);
    m_player1ScoreLabel = new QLabel("0", this);
    QFont scoreFont = m_player1ScoreLabel->font();
    scoreFont.setPointSize(24);
    scoreFont.setBold(true);
    m_player1ScoreLabel->setFont(scoreFont);
    m_player1BreakLabel = new QLabel("单杆: 0", this);

    m_player2NameLabel = new QLabel("玩家 2", this);
    m_player2ScoreLabel = new QLabel("0", this);
    m_player2ScoreLabel->setFont(scoreFont);
    m_player2BreakLabel = new QLabel("单杆: 0", this);

    m_foulLabel = new QLabel("", this);
    m_foulLabel->setStyleSheet("color: red; font-weight: bold;");
    m_foulLabel->setAlignment(Qt::AlignCenter);

    m_statusLabel = new QLabel("等待开始...", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_titleLabel);
    layout->addSpacing(10);
    layout->addWidget(m_player1NameLabel);
    layout->addWidget(m_player1ScoreLabel);
    layout->addWidget(m_player1BreakLabel);
    layout->addSpacing(15);
    layout->addWidget(m_player2NameLabel);
    layout->addWidget(m_player2ScoreLabel);
    layout->addWidget(m_player2BreakLabel);
    layout->addSpacing(15);
    layout->addWidget(m_foulLabel);
    layout->addWidget(m_statusLabel);
    layout->addStretch();
}

} // namespace Snooker2D
