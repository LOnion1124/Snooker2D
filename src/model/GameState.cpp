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

    // === 1. 白球 (D 区中央偏左) ===
    auto white = std::make_unique<Ball>(BallType::White);
    white->resetPosition(Vector2D(-TABLE_WIDTH * 0.35, 0.0)); // (-280, 0)
    m_balls.push_back(std::move(white));

    // === 2. 6 颗彩球（各自点位） ===
    // 黄 = 开球线右端, 绿 = 开球线左端, 棕 = 开球线中央
    // 蓝 = 台面中心, 粉 = 3/4 台长处, 黑 = 7/8 台长处
    auto yellow = std::make_unique<Ball>(BallType::Yellow);
    yellow->resetPosition(Vector2D(BAULK_LINE_X,  D_RADIUS));
    m_balls.push_back(std::move(yellow));

    auto green = std::make_unique<Ball>(BallType::Green);
    green->resetPosition(Vector2D(BAULK_LINE_X, -D_RADIUS));
    m_balls.push_back(std::move(green));

    auto brown = std::make_unique<Ball>(BallType::Brown);
    brown->resetPosition(Vector2D(BAULK_LINE_X, 0.0));
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
    if (m_simulationRunning || m_phase == GamePhase::GameOver || m_whiteBallPlacing) return;

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

    // 快照击球前各球落袋状态
    m_preShotPocketed.resize(m_balls.size());
    for (size_t i = 0; i < m_balls.size(); ++i) {
        m_preShotPocketed[i] = m_balls[i]->isPocketed();
    }

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

    // 跟踪白球是否落袋
    if (!m_whitePocketed) {
        for (auto& b : m_balls) {
            if (b->type() == BallType::White && b->isPocketed()) {
                m_whitePocketed = true;
                break;
            }
        }
    }

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
    if (m_phase == GamePhase::GameOver) return;

    // 计算本次击球新落袋的非白球
    int score = 0;
    bool anyBallPocketed = false;
    bool redPocketed = false;
    bool colorPocketed = false;

    for (size_t i = 0; i < m_balls.size(); ++i) {
        if (i < m_preShotPocketed.size() && m_preShotPocketed[i]) continue; // 之前就已落袋
        if (!m_balls[i]->isPocketed()) continue;

        anyBallPocketed = true;
        BallType type = m_balls[i]->type();
        if (type == BallType::White) continue;

        score += ballValue(type);
        if (type == BallType::Red) {
            redPocketed = true;
        }
        if (isColorBall(type)) {
            colorPocketed = true;
        }
    }

    // 犯规（白球落袋）→ 对手得分，切换回合，进入白球放置
    if (m_whitePocketed) {
        // TODO: 完整犯规判罚复用 Rules 模块
        PlayerId opponent = oppositePlayer(m_currentPlayerId);
        Player* opponentPlayer = (opponent == PlayerId::Player1) ? m_player1.get() : m_player2.get();
        opponentPlayer->addScore(4); // 最低罚 4 分
        m_phase = GamePhase::Foul;
        emit phaseChanged(m_phase);
        switchTurn();
        m_whiteBallPlacing = true;
        emit whiteBallPlacingStarted();
        return;
    }

    // 进球加分
    if (score > 0) {
        currentPlayer()->addScore(score);
    }

    // 阶段转换
    if (m_phase == GamePhase::RedBall) {
        // 红球阶段：进红球 → 下一杆击彩球；进彩球 → 犯规（暂简单切换回合）
        if (redPocketed) {
            if (allRedsPocketed()) {
                // 最后一颗红球已进，永久进入彩球阶段
                m_phase = GamePhase::ColorBall;
                emit phaseChanged(m_phase);
            } else {
                // 还有红球，下一杆必须击彩球（交替）
                m_phase = GamePhase::ColorBall;
                emit phaseChanged(m_phase);
            }
        }
    } else if (m_phase == GamePhase::ColorBall) {
        // 彩球阶段：进了彩球，若红球还有则回到红球阶段
        if (colorPocketed && !allRedsPocketed()) {
            m_phase = GamePhase::RedBall;
            emit phaseChanged(m_phase);
        }
        // 红球清空后一直留在彩球阶段
    }

    // 无进球 → 切换回合
    if (score == 0 && !m_whitePocketed) {
        switchTurn();
    }

    checkGameOver();
}

bool GameState::isValidPlacement(Vector2D pos) const {
    // D 区判定：x <= 开球线 且 在开球线中点半圆内
    Vector2D dCenter(BAULK_LINE_X, 0.0);
    double dx = pos.x - dCenter.x;
    double dy = pos.y - dCenter.y;
    if (pos.x > BAULK_LINE_X) return false;
    if (dx * dx + dy * dy > D_RADIUS * D_RADIUS) return false;

    // 不与任何在台球重叠
    for (const auto& ball : m_balls) {
        if (ball->type() == BallType::White) continue;
        if (!ball->isOnTable()) continue;
        Vector2D diff = pos - ball->position();
        double minDist = 2.0 * BALL_RADIUS;
        if (diff.x * diff.x + diff.y * diff.y < minDist * minDist) {
            return false;
        }
    }
    return true;
}

void GameState::placeWhiteBall(Vector2D pos) {
    if (!m_whiteBallPlacing) return;
    if (!isValidPlacement(pos)) return;

    for (auto& ball : m_balls) {
        if (ball->type() != BallType::White) continue;
        ball->resetPosition(pos);
        break;
    }

    m_whiteBallPlacing = false;

    // 恢复适当的击球阶段
    if (allRedsPocketed() && m_phase != GamePhase::NotStarted) {
        m_phase = GamePhase::ColorBall;
    } else {
        m_phase = GamePhase::RedBall;
    }
    emit phaseChanged(m_phase);
    emit whiteBallPlaced();
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
