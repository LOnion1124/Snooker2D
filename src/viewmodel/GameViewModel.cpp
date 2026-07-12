#include "GameViewModel.h"
#include "../model/GameState.h"
#include "../model/Player.h"
#include "../model/Ball.h"
#include "../model/Rules.h"

namespace Snooker2D {

GameViewModel::GameViewModel(QObject* parent)
    : QObject(parent)
{
    m_simulationTimer = new QTimer(this);
    m_simulationTimer->setInterval(1000 / 60); // ~60 FPS
    connect(m_simulationTimer, &QTimer::timeout,
            this, &GameViewModel::onSimulationTick);
}

void GameViewModel::setGameState(GameState* gameState) {
    if (m_gameState) {
        // 断开旧连接
        disconnect(m_gameState, nullptr, this, nullptr);
    }

    m_gameState = gameState;
    if (!m_gameState) return;

    // 连接 Model 信号到 ViewModel
    connect(m_gameState, &GameState::phaseChanged,
            this, &GameViewModel::onModelPhaseChanged);
    connect(m_gameState, &GameState::turnChanged,
            this, &GameViewModel::onModelTurnChanged);
    connect(m_gameState, &GameState::simulationStarted,
            this, &GameViewModel::onModelSimulationStarted);
    connect(m_gameState, &GameState::simulationFinished,
            this, &GameViewModel::onModelSimulationFinished);
    connect(m_gameState, &GameState::foulOccurred,
            this, &GameViewModel::onModelFoulOccurred);
    connect(m_gameState, &GameState::whiteBallPlacingStarted,
            this, &GameViewModel::onModelWhiteBallPlacingStarted);
    connect(m_gameState, &GameState::whiteBallPlaced,
            this, &GameViewModel::onModelWhiteBallPlaced);

    // 连接玩家分数变化
    connect(m_gameState->player1(), &Player::scoreChanged,
            this, &GameViewModel::onPlayerScoreChanged);
    connect(m_gameState->player2(), &Player::scoreChanged,
            this, &GameViewModel::onPlayerScoreChanged);

    refreshBallPositions();
    refreshScores();
}

void GameViewModel::shoot() {
    if (m_gameState) {
        m_gameState->performShot(m_cueAngle, m_cuePower);
    }
}

void GameViewModel::setAngle(double angle) {
    if (m_cueAngle != angle) {
        m_cueAngle = angle;
        emit cueAngleChanged();
    }
}

void GameViewModel::setPower(double power) {
    if (m_cuePower != power) {
        m_cuePower = power;
        emit cuePowerChanged();
    }
}

void GameViewModel::confirmFoul() {
    if (m_gameState) {
        m_gameState->confirmFoul();
    }
}

void GameViewModel::selectFreeBall(int ballTypeInt) {
    if (m_gameState) {
        m_gameState->selectFreeBall(static_cast<BallType>(ballTypeInt));
    }
}

void GameViewModel::refreshBallPositions() {
    m_ballPositions.clear();
    if (!m_gameState) return;

    for (const auto& ball : m_gameState->balls()) {
        QVariantMap ballData;
        ballData["x"] = ball->position().x;
        ballData["y"] = ball->position().y;
        ballData["type"] = static_cast<int>(ball->type());
        ballData["pocketed"] = ball->isPocketed();
        ballData["onTable"] = ball->isOnTable();
        m_ballPositions.append(ballData);
    }
    emit ballPositionsChanged();
}

void GameViewModel::refreshScores() {
    if (!m_gameState) return;
    int p1 = m_gameState->player1()->score();
    int p2 = m_gameState->player2()->score();
    if (m_player1Score != p1) {
        m_player1Score = p1;
        emit player1ScoreChanged();
    }
    if (m_player2Score != p2) {
        m_player2Score = p2;
        emit player2ScoreChanged();
    }
}

// --- Model 信号回调 ---

void GameViewModel::onModelPhaseChanged(GamePhase phase) {
    switch (phase) {
        case GamePhase::NotStarted: m_baseGamePhase = "未开始"; break;
        case GamePhase::RedBall:    m_baseGamePhase = "请击红球"; break;
        case GamePhase::ColorBall:  m_baseGamePhase = "请击彩球"; break;
        case GamePhase::FreeBall:   m_baseGamePhase = "自由球"; break;
        case GamePhase::Foul:       m_baseGamePhase = "犯规"; break;
        case GamePhase::GameOver:   m_baseGamePhase = "比赛结束"; break;
    }
    m_gamePhase = m_baseGamePhase;
    // 阶段切换时球桌状态可能已变化（如新游戏开局摆球）, 同步刷新
    refreshBallPositions();
    emit gamePhaseChanged();
}

void GameViewModel::onModelTurnChanged() {
    if (m_gameState) {
        m_currentPlayer = (m_gameState->currentPlayer() == m_gameState->player1()) ? 1 : 2;
        emit currentPlayerChanged();
    }
}

void GameViewModel::onModelSimulationStarted() {
    m_simulationTimer->start();
    m_gamePhase = m_baseGamePhase + " (模拟中...)";
    emit gamePhaseChanged();
}

void GameViewModel::onSimulationTick() {
    if (!m_gameState || !m_gameState->isSimulationRunning()) {
        m_simulationTimer->stop();
        return;
    }
    m_gameState->updateSimulation();
    refreshBallPositions();
}

void GameViewModel::onModelSimulationFinished() {
    m_simulationTimer->stop();
    m_gamePhase = m_baseGamePhase;
    emit gamePhaseChanged();
    refreshBallPositions();
    refreshScores();
}

void GameViewModel::onModelFoulOccurred(const FoulResult& result) {
    emit foulOccurred(result.description);
    refreshScores();
}

void GameViewModel::onPlayerScoreChanged(int /*score*/) {
    refreshScores();
}

void GameViewModel::confirmWhiteBallPlacement(double x, double y) {
    if (m_gameState && m_isPlacingWhiteBall) {
        m_gameState->placeWhiteBall(Vector2D(x, y));
    }
}

void GameViewModel::onModelWhiteBallPlacingStarted() {
    m_isPlacingWhiteBall = true;
    emit whiteBallPlacingChanged();
}

void GameViewModel::onModelWhiteBallPlaced() {
    m_isPlacingWhiteBall = false;
    emit whiteBallPlacingChanged();
    refreshBallPositions();
}

} // namespace Snooker2D
