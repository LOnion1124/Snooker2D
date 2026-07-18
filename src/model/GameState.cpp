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
    m_whiteBallPlacing = true;
    emit phaseChanged(m_phase);
    emit turnChanged(currentPlayer());
    emit whiteBallPlacingStarted();
}

void GameState::resetGame() {
    m_simulationRunning = false;
    m_whitePocketed = false;
    m_whiteBallPlacing = false;
    m_firstHitBall = BallType::White;
    m_anyBallHitCushion = false;
    m_preShotPocketed.clear();
    m_balls.clear();
    m_player1->resetScore();
    m_player2->resetScore();
    initBalls();
}

void GameState::initBalls() {
    const double hw = TABLE_WIDTH / 2.0;   // 400
    const double hh = TABLE_HEIGHT / 2.0;  // 200

    // 白球（D 区中央偏左）
    auto white = std::make_unique<Ball>(BallType::White);
    white->resetPosition(Vector2D(-TABLE_WIDTH * 0.35, 0.0)); // (-280, 0)
    m_balls.push_back(std::move(white));

    // 6 颗彩球（各自点位）
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
    black->resetPosition(Vector2D(TABLE_WIDTH * 0.39, 0.0)); // (300, 0)
    m_balls.push_back(std::move(black));

    // 15 颗红球（三角阵）
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

void GameState::performShot(double angle, double power,
                             double englishX, double englishY) {
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

    // 加塞：按击球力度转换为两类自旋，避免 0 力度时只原地空转。
    const double spinScale = power <= 0.0 ? 0.0 : power / 100.0;
    const double rollSpin = englishY * ENGLISH_TO_SPIN * spinScale;
    const double sideSpin = englishX * ENGLISH_TO_SPIN * spinScale;
    whiteBall->setSpin(rollSpin, sideSpin);

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

    // 在物理模拟中跟踪首次击中和碰库
    BallType firstHitThisFrame = BallType::White;
    bool cushionHitThisFrame = false;
    m_physics->step(1.0 / 60.0, ballPtrs, *m_table,
                    &firstHitThisFrame, &cushionHitThisFrame);

    // 仅记录整个击球过程的首次击中（首帧之后再击中不再覆盖）
    if (m_firstHitBall == BallType::White && firstHitThisFrame != BallType::White) {
        m_firstHitBall = firstHitThisFrame;
    }
    m_anyBallHitCushion = m_anyBallHitCushion || cushionHitThisFrame;

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
    BallType firstPocketedColor = BallType::White;

    for (size_t i = 0; i < m_balls.size(); ++i) {
        if (i < m_preShotPocketed.size() && m_preShotPocketed[i]) continue;
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
            if (firstPocketedColor == BallType::White) {
                firstPocketedColor = type;
            }
        }
    }

    // 确定当前应击球种
    BallType requiredType = BallType::Red;
    if (m_phase == GamePhase::ColorBall) {
        if (allRedsPocketed()) {
            // TODO: 最终彩球阶段按升序（黄→绿→棕→蓝→粉→黑）
            requiredType = BallType::Yellow;
        }
        // 红球还在时：交替阶段，"任意彩球" 合法，另做处理
    }

    // 构建 Ball* 列表供 Rules 使用
    std::vector<Ball*> ballPtrs;
    ballPtrs.reserve(m_balls.size());
    for (auto& b : m_balls) ballPtrs.push_back(b.get());
    Ball* whiteBall = nullptr;
    for (auto* b : ballPtrs) {
        if (b->type() == BallType::White && !b->isPocketed()) {
            whiteBall = b;
            break;
        }
    }

    // 犯规检测
    bool isFoul = false;
    FoulResult foulResult;

    // 白球落袋 → 犯规
    if (m_whitePocketed) {
        foulResult = m_rules->checkFoul(ballPtrs, whiteBall, m_firstHitBall,
                                         m_anyBallHitCushion, true, requiredType);
        isFoul = true;
    }
    // 空杆（未击中任何球），且无任何球落袋
    else if (m_firstHitBall == BallType::White && !anyBallPocketed) {
        foulResult.isFoul = true;
        foulResult.type = FoulType::MissedAll;
        foulResult.penaltyPoints = m_rules->calculatePenalty(FoulType::MissedAll, requiredType);
        isFoul = true;
    }
    // 先击中错误球（含交替阶段特殊处理）
    else {
        bool wrongBall = false;
        BallType foulBallType = m_firstHitBall;

        if (m_firstHitBall == BallType::White) {
            // 白球为首中但进了球（上面空杆分支已排除无进球情况）
            wrongBall = false;
        } else if (m_phase == GamePhase::ColorBall && !allRedsPocketed()) {
            // 交替阶段：红球不合法，彩球合法
            if (m_firstHitBall == BallType::Red) {
                wrongBall = true;
            } else if (isColorBall(m_firstHitBall)) {
                wrongBall = false; // 任意彩球合法
            } else {
                wrongBall = true;
            }
        } else if (m_firstHitBall != requiredType) {
            wrongBall = true;
        }

        if (wrongBall) {
            foulResult.isFoul = true;
            foulResult.type = FoulType::WrongBallFirst;
            foulResult.penaltyPoints = m_rules->calculatePenalty(FoulType::WrongBallFirst, foulBallType);
            isFoul = true;
        }
    }

    // 红球阶段彩球落袋 → 犯规（即使先碰了红球）
    if (!isFoul && m_phase == GamePhase::RedBall && colorPocketed) {
        foulResult.isFoul = true;
        foulResult.type = FoulType::ColorPocketed;
        foulResult.penaltyPoints = m_rules->calculatePenalty(FoulType::ColorPocketed, firstPocketedColor);
        isFoul = true;
    }
    // 无球碰库（有进球时豁免）
    if (!isFoul && !anyBallPocketed && !m_anyBallHitCushion) {
        foulResult.isFoul = true;
        foulResult.type = FoulType::NoBallHitCushion;
        foulResult.penaltyPoints = m_rules->calculatePenalty(FoulType::NoBallHitCushion, m_firstHitBall);
        isFoul = true;
    }

    // 处理犯规
    if (isFoul) {
        PlayerId opponentId = oppositePlayer(m_currentPlayerId);
        Player* opponent = (opponentId == PlayerId::Player1) ? m_player1.get() : m_player2.get();
        opponent->addScore(foulResult.penaltyPoints);
        emit foulOccurred(foulResult);

        m_phase = GamePhase::Foul;
        emit phaseChanged(m_phase);
        switchTurn();

        // 白球落袋 → 进入放置模式
        if (m_whitePocketed) {
            m_whiteBallPlacing = true;
            emit whiteBallPlacingStarted();
        }

        // 犯规时复位新落袋的彩球（红球保留）
        if (!allRedsPocketed()) {
            for (size_t i = 0; i < m_balls.size(); ++i) {
                if (i < m_preShotPocketed.size() && m_preShotPocketed[i]) continue;
                if (!m_balls[i]->isPocketed()) continue;
                if (m_balls[i]->type() == BallType::White) continue;
                if (isColorBall(m_balls[i]->type())) {
                    m_balls[i]->respot();
                }
            }
        }

        // 非白球落袋犯规 → 恢复为红球阶段（简化规则）
        if (!m_whitePocketed) {
            m_phase = GamePhase::RedBall;
            emit phaseChanged(m_phase);
        }
        // 白球落袋犯规 → 由 placeWhiteBall() 恢复阶段
        return;
    }

    // 未犯规，正常计分
    if (score > 0) {
        currentPlayer()->addScore(score);
    }

    // 红球未清空时，复位新落袋的彩球（交替阶段：彩球得分后回位）
    if (colorPocketed && !allRedsPocketed()) {
        for (size_t i = 0; i < m_balls.size(); ++i) {
            if (i < m_preShotPocketed.size() && m_preShotPocketed[i]) continue;
            if (!m_balls[i]->isPocketed()) continue;
            if (m_balls[i]->type() == BallType::White) continue;
            if (isColorBall(m_balls[i]->type())) {
                m_balls[i]->respot();
            }
        }
    }

    // 阶段转换
    if (allRedsPocketed()) {
        if (m_phase != GamePhase::ColorBall) {
            m_phase = GamePhase::ColorBall;
            emit phaseChanged(m_phase);
        }
    } else if (m_phase == GamePhase::ColorBall && colorPocketed) {
        m_phase = GamePhase::RedBall;
        emit phaseChanged(m_phase);
    } else if (m_phase == GamePhase::RedBall && redPocketed) {
        m_phase = GamePhase::ColorBall;
        emit phaseChanged(m_phase);
    } else if (m_phase != GamePhase::RedBall) {
        m_phase = GamePhase::RedBall;
        emit phaseChanged(m_phase);
    }

    // 无进球 → 切换回合（犯规分支已 return，不会走到这里）
    if (score == 0) {
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
    bool anyBallOnTable = false;
    for (const auto& ball : m_balls) {
        if (ball->type() == BallType::White) continue;
        if (ball->isOnTable()) {
            anyBallOnTable = true;
            break;
        }
    }
    if (!anyBallOnTable) {
        m_phase = GamePhase::GameOver;
        emit phaseChanged(m_phase);
        // 当前台面分高者胜出，或最后一杆玩家胜出
        Player* winner = nullptr;
        if (m_player1->score() > m_player2->score()) {
            winner = m_player1.get();
        } else if (m_player2->score() > m_player1->score()) {
            winner = m_player2.get();
        }
        emit gameOver(winner);
    }
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
