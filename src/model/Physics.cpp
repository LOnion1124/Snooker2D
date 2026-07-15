#include "Physics.h"
#include "Ball.h"
#include "Table.h"
#include "../common/Types.h"
#include "../common/Constants.h"
#include "../common/MathUtils.h"

#include <cmath>

namespace Snooker2D {

Physics::Physics(QObject* parent)
    : QObject(parent)
{
}

void Physics::step(double deltaTime, std::vector<Ball*>& balls, const Table& table,
                   BallType* outFirstHit, bool* outCushionHit) {
    // 1. 移动球
    moveBalls(balls, deltaTime);

    // 2. 检测球-球碰撞并处理（同时跟踪白球首次击中）
    checkBallBallCollisions(balls, outFirstHit);

    // 3. 检测球-库边碰撞并处理（同时跟踪碰库事件）
    checkBallCushionCollisions(balls, table, outCushionHit);

    // 4. 检测袋口
    checkPocketDetection(balls, table);

    // 5. 施加摩擦力减速
    applyFriction(balls, deltaTime);

    // 6. 硬边界约束 — 防止链式推出越界
    double hw = table.width() / 2.0;
    double hh = table.height() / 2.0;
    double margin = BALL_RADIUS * 0.5;
    for (auto* ball : balls) {
        if (ball->isPocketed()) continue;
        applyHardConstraint(*ball, hw, hh, margin);
    }

    // 7. 把出界无动量的球直接回拉
    checkPullBackBalls(balls, table);
}

bool Physics::allBallsStopped(const std::vector<Ball*>& balls) {
    for (const auto* ball : balls) {
        if (!ball->isPocketed() && ball->velocity().length() > MIN_VELOCITY) {
            return false;
        }
    }
    return true;
}

// 私有方法实现

void Physics::applyFriction(std::vector<Ball*>& balls, double deltaTime) {
    for (auto* ball : balls) {
        if (ball->isPocketed()) continue;
        Vector2D vel = ball->velocity();
        double speed = vel.length();
        if (speed < MIN_VELOCITY) {
            ball->setVelocity(Vector2D(0.0, 0.0));
        } else {
            // 简单线性摩擦减速
            double friction = 1.0 - (1.0 - FRICTION_COEFFICIENT) * deltaTime * 60.0;
            ball->setVelocity(vel * friction);
        }
    }
}

void Physics::moveBalls(std::vector<Ball*>& balls, double deltaTime) {
    for (auto* ball : balls) {
        if (ball->isPocketed()) continue;
        Vector2D pos = ball->position();
        Vector2D vel = ball->velocity();
        ball->setPosition(pos + vel * deltaTime * 60.0);
    }
}

void Physics::checkBallBallCollisions(std::vector<Ball*>& balls, BallType* ioFirstHit) {
    size_t n = balls.size();
    for (size_t i = 0; i < n; ++i) {
        if (balls[i]->isPocketed()) continue;
        for (size_t j = i + 1; j < n; ++j) {
            if (balls[j]->isPocketed()) continue;
            if (MathUtils::circleOverlap(balls[i]->position(), BALL_RADIUS,
                                         balls[j]->position(), BALL_RADIUS)) {
                resolveBallCollision(*balls[i], *balls[j], ioFirstHit);
            }
        }
    }
}

void Physics::checkBallCushionCollisions(std::vector<Ball*>& balls, const Table& table, bool* outCushionHit) {
    for (auto* ball : balls) {
        if (ball->isPocketed()) continue;
        for (const auto& cushion : table.cushions()) {
            Vector2D closest = MathUtils::closestPointOnSegment(
                ball->position(), cushion.p1, cushion.p2);
            double dist = MathUtils::distance(ball->position(), closest);
            if (dist < BALL_RADIUS) {
                if (outCushionHit) *outCushionHit = true;
                resolveCushionCollision(*ball, closest, dist);
            }
        }
    }
}

void Physics::checkPocketDetection(std::vector<Ball*>& balls, const Table& table) {
    for (auto* ball : balls) {
        if (ball->isPocketed()) continue;
        for (const auto& pocket : table.pockets()) {
            double dist = MathUtils::distance(ball->position(), pocket.position);
            if (dist < pocket.radius) {
                ball->setPocketed(true);
                break;
            }
        }
    }
}

void Physics::resolveBallCollision(Ball& a, Ball& b, BallType* ioFirstHit) {
    Vector2D normal = b.position() - a.position();
    double dist = normal.length();
    double overlap = 2.0 * BALL_RADIUS - dist;

    // 防零除：两球完全重合时选默认方向
    if (dist < 1e-9) {
        normal = Vector2D(1.0, 0.0);
        overlap = 2.0 * BALL_RADIUS;
    } else {
        normal = normal.normalized();
    }

    // 1. 位置分离 — 将两球沿法线各推开 overlap/2
    if (overlap > 0.0) {
        Vector2D correction = normal * (overlap * 0.5);
        a.setPosition(a.position() - correction);
        b.setPosition(b.position() + correction);
    }

    // 2. 速度响应 — 等质量弹性碰撞（带恢复系数）
    Vector2D relVel = a.velocity() - b.velocity();
    double velAlongNormal = relVel.dot(normal);
    if (velAlongNormal <= 0) return; // 正在分离或静止，跳过

    double restitution = COLLISION_RESTITUTION;
    double impulseScalar = -(1.0 + restitution) * velAlongNormal * 0.5;

    Vector2D impulse = normal * impulseScalar;
    a.setVelocity(a.velocity() + impulse);
    b.setVelocity(b.velocity() - impulse);

    // 跟踪白球首次击中的球
    if (ioFirstHit && *ioFirstHit == BallType::White) {
        if (a.type() == BallType::White && b.type() != BallType::White) {
            *ioFirstHit = b.type();
        } else if (b.type() == BallType::White && a.type() != BallType::White) {
            *ioFirstHit = a.type();
        }
    }
}

void Physics::resolveCushionCollision(Ball& ball, const Vector2D& closestPoint, double distance) {
    Vector2D normal = ball.position() - closestPoint;
    double normalLen = normal.length();

    // 防零除：球心正好在库边上（极罕见），选向上为默认法线
    if (normalLen < 1e-9) {
        normal = Vector2D(0.0, -1.0);
        normalLen = 1.0;
    } else {
        normal = normal.normalized();
    }

    // 1. 位置修正 — 沿法线推出至刚好接触
    double overlap = BALL_RADIUS - distance;
    if (overlap > 0.0) {
        ball.setPosition(ball.position() + normal * overlap);
    }

    // 2. 速度反射（带库边恢复系数）
    double velAlongNormal = MathUtils::dot(ball.velocity(), normal);
    if (velAlongNormal < 0.0) {
        Vector2D reflected = ball.velocity() - normal * ((1.0 + CUSHION_RESTITUTION) * velAlongNormal);
        ball.setVelocity(reflected);
    }
}

void Physics::applyHardConstraint(Ball& ball, double halfW, double halfH, double margin) {
    Vector2D pos = ball.position();
    Vector2D vel = ball.velocity();
    double clampX = halfW - margin;
    double clampY = halfH - margin;
    bool clamped = false;

    if (pos.x > clampX) {
        pos.x = clampX;
        if (vel.x > 0.0) vel.x = 0.0;
        clamped = true;
    } else if (pos.x < -clampX) {
        pos.x = -clampX;
        if (vel.x < 0.0) vel.x = 0.0;
        clamped = true;
    }

    if (pos.y > clampY) {
        pos.y = clampY;
        if (vel.y > 0.0) vel.y = 0.0;
        clamped = true;
    } else if (pos.y < -clampY) {
        pos.y = -clampY;
        if (vel.y < 0.0) vel.y = 0.0;
        clamped = true;
    }

    if (clamped) {
        ball.setPosition(pos);
        ball.setVelocity(vel);
    }
}

void Physics::checkPullBackBalls(std::vector<Ball*>& balls, const Table& table) {
    double halfW = table.width() / 2.0;
    double halfH = table.height() / 2.0;
    double limitX = halfW - BALL_RADIUS;
    double limitY = halfH - BALL_RADIUS;
    double speedThreshold = MIN_VELOCITY * 2.0;

    for (auto* ball : balls) {
        if (ball->isPocketed()) continue;
        if (ball->velocity().length() > speedThreshold) continue;

        Vector2D pos = ball->position();
        Vector2D vel = ball->velocity();
        bool clamped = false;

        if (pos.x > limitX) {
            pos.x = limitX;
            if (vel.x > 0.0) vel.x = 0.0;
            clamped = true;
        } else if (pos.x < -limitX) {
            pos.x = -limitX;
            if (vel.x < 0.0) vel.x = 0.0;
            clamped = true;
        }
        if (pos.y > limitY) {
            pos.y = limitY;
            if (vel.y > 0.0) vel.y = 0.0;
            clamped = true;
        } else if (pos.y < -limitY) {
            pos.y = -limitY;
            if (vel.y < 0.0) vel.y = 0.0;
            clamped = true;
        }

        if (clamped) {
            ball->setPosition(pos);
            ball->setVelocity(vel);
        }
    }
}

} // namespace Snooker2D
