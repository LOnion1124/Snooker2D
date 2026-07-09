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

void Physics::step(double deltaTime, std::vector<Ball*>& balls, const Table& table) {
    // 1. 移动球
    moveBalls(balls, deltaTime);

    // 2. 检测球-球碰撞并处理
    checkBallBallCollisions(balls);

    // 3. 检测球-库边碰撞并处理
    checkBallCushionCollisions(balls, table);

    // 4. 检测袋口
    checkPocketDetection(balls, table);

    // 5. 施加摩擦力减速
    applyFriction(balls, deltaTime);
}

bool Physics::allBallsStopped(const std::vector<Ball*>& balls) {
    for (const auto* ball : balls) {
        if (!ball->isPocketed() && ball->velocity().length() > MIN_VELOCITY) {
            return false;
        }
    }
    return true;
}

// ----------------------------------------------------------------------------
// Private 实现
// ----------------------------------------------------------------------------

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

void Physics::checkBallBallCollisions(std::vector<Ball*>& balls) {
    size_t n = balls.size();
    for (size_t i = 0; i < n; ++i) {
        if (balls[i]->isPocketed()) continue;
        for (size_t j = i + 1; j < n; ++j) {
            if (balls[j]->isPocketed()) continue;
            if (MathUtils::circleOverlap(balls[i]->position(), BALL_RADIUS,
                                         balls[j]->position(), BALL_RADIUS)) {
                resolveBallCollision(*balls[i], *balls[j]);
            }
        }
    }
}

void Physics::checkBallCushionCollisions(std::vector<Ball*>& balls, const Table& table) {
    for (auto* ball : balls) {
        if (ball->isPocketed()) continue;
        for (const auto& cushion : table.cushions()) {
            // TODO: 实现球-线段碰撞检测
            (void)cushion;
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

void Physics::resolveBallCollision(Ball& a, Ball& b) {
    // 弹性碰撞处理（等质量简化）
    Vector2D normal = b.position() - a.position();
    normal = normal.normalized();
    Vector2D relVel = a.velocity() - b.velocity();
    double velAlongNormal = relVel.dot(normal);
    if (velAlongNormal > 0) return; // 球正在分离

    double restitution = COLLISION_RESTITUTION;
    double impulseScalar = -(1.0 + restitution) * velAlongNormal / 2.0;

    Vector2D impulse = normal * impulseScalar;
    a.setVelocity(a.velocity() + impulse);
    b.setVelocity(b.velocity() - impulse);
}

void Physics::resolveCushionCollision(Ball& ball, const Cushion& cushion) {
    // TODO: 实现球-库边反弹
    (void)ball;
    (void)cushion;
}

} // namespace Snooker2D
