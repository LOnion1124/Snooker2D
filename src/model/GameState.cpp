#include "GameState.h"
#include "Ball.h"
#include "Table.h"
#include "Player.h"
#include "Physics.h"
#include "Rules.h"
#include "../common/Constants.h"
#include <cmath>

namespace Snooker2D {

GameState::GameState(QObject* parent)
    : QObject(parent)
    , m_table(std::make_unique<Table>())
    , m_physics(std::make_unique<Physics>())
    , m_rules(std::make_unique<Rules>())
    , m_player1(std::make_unique<Player>("Player 1"))
    , m_player2(std::make_unique<Player>("Player 2"))
{
}

GameState::~GameState() = default;

void GameState::startNewGame() {
    resetGame();
    m_currentPlayerId = PlayerId::Player1;
    m_phase = GamePhase::RedBall;
    emit phaseChanged(m_phase);
    emit turnChanged(currentPlayer());
}

void GameState::resetGame() {
    m_simulationRunning = false;
    m_balls.clear();
    m_player1->resetScore();
    m_player2->resetScore();
    initBalls();
}

void GameState::initBalls() {
    const double hw = TABLE_WIDTH / 2.0;   // 400
    const double hh = TABLE_HEIGHT / 2.0;  // 200

    // --- 开球线与 D 区 ---
    // 开球线距左侧库边 1/5 台长: -400 + 800/5 = -240
    const double baulkLine = -TABLE_WIDTH * 0.3;

    // D 区半圆半径（约 0.35 × 半台高）
    const double dRadius = hh * 0.35;

    // === 1. 白球 (D 区中央偏左) ===
    auto white = std::make_unique<Ball>(BallType::White);
    white->resetPosition(Vector2D(-TABLE_WIDTH * 0.35, 0.0)); // (-280, 0)
    m_balls.push_back(std::move(white));

    // === 2. 6 颗彩球（各自点位） ===
    // 黄 = 开球线右端, 绿 = 开球线左端, 棕 = 开球线中央
    // 蓝 = 台面中心, 粉 = 3/4 台长处, 黑 = 7/8 台长处
    auto yellow = std::make_unique<Ball>(BallType::Yellow);
    yellow->resetPosition(Vector2D(baulkLine,  dRadius));
    m_balls.push_back(std::move(yellow));

    auto green = std::make_unique<Ball>(BallType::Green);
    green->resetPosition(Vector2D(baulkLine, -dRadius));
    m_balls.push_back(std::move(green));

    auto brown = std::make_unique<Ball>(BallType::Brown);
    brown->resetPosition(Vector2D(baulkLine, 0.0));
    m_balls.push_back(std::move(brown));

    auto blue = std::make_unique<Ball>(BallType::Blue);
    blue->resetPosition(Vector2D(0.0, 0.0));
    m_balls.push_back(std::move(blue));

    auto pink = std::make_unique<Ball>(BallType::Pink);
    pink->resetPosition(Vector2D(TABLE_WIDTH * 0.25, 0.0)); // (200, 0)
    m_balls.push_back(std::move(pink));

    auto black = std::make_unique<Ball>(BallType::Black);
    black->resetPosition(Vector2D(TABLE_WIDTH * 0.375, 0.0)); // (300, 0)
    m_balls.push_back(std::move(black));

    // === 3. 15 颗红球（三角阵） ===
    // 顶点紧贴粉球后方: apexX = 200 + 2*BALL_RADIUS
    // 行间距 = sqrt(3) * BALL_RADIUS（正六边形密排）
    const double apexX = TABLE_WIDTH * 0.25 + 2.0 * BALL_RADIUS;
    const double rowSpacing = std::sqrt(3.0) * BALL_RADIUS;

    for (int row = 0; row < 5; ++row) {
        int ballsInRow = row + 1;
        double rowX = apexX + row * rowSpacing;
        for (int col = 0; col < ballsInRow; ++col) {
            double yOffset = (col - row * 0.5) * 2.0 * BALL_RADIUS;
            auto red = std::make_unique<Ball>(BallType::Red);
            red->resetPosition(Vector2D(rowX, yOffset));
            m_balls.push_back(std::move(red));
        }
    }
}

void GameState::performShot(double angle, double power) {
    if (m_simulationRunning || m_phase == GamePhase::GameOver) return;

    Ball* whiteBall = nullptr;
    for (auto& ball : m_balls) {
        if (ball->type() == BallType::White && !ball->isPocketed()) {
            whiteBall = ball.get();
            break;
        }
    }
    if (!whiteBall) return;

    double rad = angle * 3.14159265358979323846 / 180.0;
    double speed = (power / 100.0) * MAX_SPEED;
    whiteBall->setVelocity(Vector2D(std::cos(rad) * speed, -std::sin(rad) * speed));

    m_firstHitBall = BallType::White;
    m_anyBallHitCushion = false;
    m_whitePocketed = false;
    m_simulationRunning = true;
    emit simulationStarted();
}

void GameState::updateSimulation() {
    if (!m_simulationRunning) return;

    // 转换为 raw pointer 列表给 Physics
    std::vector<Ball*> ballPtrs;
    ballPtrs.reserve(m_balls.size());
    for (auto& b : m_balls) ballPtrs.push_back(b.get());

    m_physics->step(1.0 / 60.0, ballPtrs, *m_table);

    // TODO: 跟踪首次碰撞、碰库、白球落袋

    if (Physics::allBallsStopped(ballPtrs)) {
        m_simulationRunning = false;
        emit simulationFinished();
        handlePostShot();
    }
}

void GameState::switchTurn() {
    m_currentPlayerId = (m_currentPlayerId == PlayerId::Player1) ?
                        PlayerId::Player2 : PlayerId::Player1;
    currentPlayer()->resetBreak();
    emit turnChanged(currentPlayer());
}

Player* GameState::currentPlayer() const {
    return m_currentPlayerId == PlayerId::Player1 ? m_player1.get() : m_player2.get();
}

void GameState::handlePostShot() {
    // TODO: 检测犯规、更新比分、决定下一阶段
    // 临时：直接切换回合
    if (m_phase != GamePhase::GameOver) {
        switchTurn();
    }
}

void GameState::confirmFoul() {
    // TODO: 对手获得罚分，白球 D 区重置
}

void GameState::selectFreeBall(BallType color) {
    // TODO: 自由球逻辑
    (void)color;
}

void GameState::checkGameOver() {
    // TODO: 判定台面是否清空、分差是否不可追
}

bool GameState::allRedsPocketed() const {
    for (const auto& ball : m_balls) {
        if (ball->type() == BallType::Red && !ball->isPocketed()) {
            return false;
        }
    }
    return true;
}

} // namespace Snooker2D
