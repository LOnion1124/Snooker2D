#include "Physics.h"
#include "Ball.h"
#include "Table.h"
#include "../common/Types.h"
#include "../common/Constants.h"
#include "../common/MathUtils.h"

#include <algorithm>
#include <cmath>

namespace Snooker2D {

namespace {

constexpr double SPIN_STOP_THRESHOLD = 0.05;
constexpr double MAX_ROLLING_COUPLING_PER_FRAME = MAX_SPEED * 0.002;
constexpr double MAX_FOLLOW_KICK_RATIO = 0.04;
constexpr double MAX_DRAW_KICK_RATIO = 0.09;

double clampRollKick(double kick, double normalImpulseMagnitude) {
    const double maxFollowKick = normalImpulseMagnitude * MAX_FOLLOW_KICK_RATIO;
    const double maxDrawKick = normalImpulseMagnitude * MAX_DRAW_KICK_RATIO;
    return std::clamp(kick, -maxDrawKick, maxFollowKick);
}

Vector2D cushionInwardNormal(const Cushion& cushion) {
    const double dx = cushion.p2.x - cushion.p1.x;
    const double dy = cushion.p2.y - cushion.p1.y;

    if (std::abs(dx) >= std::abs(dy)) {
        return cushion.p1.y < 0.0 ? Vector2D(0.0, 1.0)
                                  : Vector2D(0.0, -1.0);
    }

    return cushion.p1.x < 0.0 ? Vector2D(1.0, 0.0)
                              : Vector2D(-1.0, 0.0);
}

} // namespace

Physics::Physics(QObject* parent)
    : QObject(parent)
{
}

void Physics::step(double deltaTime, std::vector<Ball*>& balls, const Table& table,
                   BallType* outFirstHit, bool* outCushionHit) {
    // 移动球
    moveBalls(balls, deltaTime);

    // 检测球-球碰撞并处理（同时跟踪白球首次击中）
    checkBallBallCollisions(balls, outFirstHit);

    // 检测球-库边碰撞并处理（同时跟踪碰库事件）
    checkBallCushionCollisions(balls, table, outCushionHit);

    // 检测袋口
    checkPocketDetection(balls, table);

    // 施加摩擦力减速
    applyFriction(balls, deltaTime);

    // 硬边界约束，防止链式推出越界
    double hw = table.width() / 2.0;
    double hh = table.height() / 2.0;
    double margin = BALL_RADIUS;
    for (auto* ball : balls) {
        if (ball->isPocketed()) continue;
        applyHardConstraint(*ball, hw, hh, margin);
    }

    // 把出界无动量的球直接回拉
    checkPullBackBalls(balls, table);
}

bool Physics::allBallsStopped(const std::vector<Ball*>& balls) {
    for (const auto* ball : balls) {
        if (!ball->isPocketed()) {
            if (ball->velocity().length() > MIN_VELOCITY) return false;
            if (std::abs(ball->angularVelocity()) > SPIN_STOP_THRESHOLD) return false;
            if (std::abs(ball->sideSpin()) > SPIN_STOP_THRESHOLD) return false;
        }
    }
    return true;
}

// 私有方法实现

void Physics::applyFriction(std::vector<Ball*>& balls, double deltaTime) {
    for (auto* ball : balls) {
        if (ball->isPocketed()) continue;

        // 线速度摩擦
        Vector2D vel = ball->velocity();
        double speed = vel.length();
        if (speed < MIN_VELOCITY) {
            ball->setVelocity(Vector2D(0.0, 0.0));
        } else {
            double friction = 1.0 - (1.0 - FRICTION_COEFFICIENT) * deltaTime * 60.0;
            ball->setVelocity(vel * friction);
        }

        // 上/下塞滚动耦合：把剩余纵向自旋逐步转成沿当前方向的速度。
        double rollSpin = ball->angularVelocity();
        if (speed > MIN_VELOCITY && std::abs(rollSpin) > SPIN_STOP_THRESHOLD) {
            Vector2D forward = vel.normalized();
            double slipSpeed = rollSpin * BALL_RADIUS - speed;
            double coupling = slipSpeed * ROLLING_COUPLING * deltaTime * 60.0;
            double maxCoupling = MAX_ROLLING_COUPLING_PER_FRAME * deltaTime * 60.0;
            coupling = std::clamp(coupling, -maxCoupling, maxCoupling);
            ball->setVelocity(ball->velocity() + forward * coupling);
            rollSpin -= coupling / BALL_RADIUS;
        }

        // 两类自旋自然衰减。
        const double spinDecay = std::pow(SPIN_DECAY, deltaTime * 60.0);
        rollSpin *= spinDecay;
        double sideSpin = ball->sideSpin() * spinDecay;

        if (std::abs(rollSpin) <= SPIN_STOP_THRESHOLD) rollSpin = 0.0;
        if (std::abs(sideSpin) <= SPIN_STOP_THRESHOLD) sideSpin = 0.0;
        if (speed < MIN_VELOCITY && std::abs(rollSpin) <= SPIN_STOP_THRESHOLD) rollSpin = 0.0;

        ball->setSpin(rollSpin, sideSpin);
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
                resolveCushionCollision(*ball, closest, cushionInwardNormal(cushion));
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
    const Vector2D preVelocityA = a.velocity();
    const Vector2D preVelocityB = b.velocity();
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

    // 位置分离，将两球沿法线各推开 overlap/2
    if (overlap > 0.0) {
        Vector2D correction = normal * (overlap * 0.5);
        a.setPosition(a.position() - correction);
        b.setPosition(b.position() + correction);
    }

    // 法线方向速度响应，等质量弹性碰撞（带恢复系数）
    Vector2D relVel = a.velocity() - b.velocity();
    double velAlongNormal = relVel.dot(normal);
    if (velAlongNormal <= 0) return; // 正在分离或静止，跳过

    double restitution = COLLISION_RESTITUTION;
    double impulseScalar = -(1.0 + restitution) * velAlongNormal * 0.5;

    Vector2D impulse = normal * impulseScalar;
    a.setVelocity(a.velocity() + impulse);
    b.setVelocity(b.velocity() - impulse);

    // 上/下塞响应：跟球继续前进，缩杆沿来球方向回拉。
    const double normalImpulseMagnitude = std::abs(impulseScalar);
    if (preVelocityA.length() > MIN_VELOCITY && std::abs(a.angularVelocity()) > SPIN_STOP_THRESHOLD) {
        double kick = a.angularVelocity() * BALL_RADIUS * ROLLING_COUPLING;
        kick = clampRollKick(kick, normalImpulseMagnitude);
        a.setVelocity(a.velocity() + preVelocityA.normalized() * kick);
        a.setAngularVelocity(a.angularVelocity() - kick / BALL_RADIUS);
    }
    if (preVelocityB.length() > MIN_VELOCITY && std::abs(b.angularVelocity()) > SPIN_STOP_THRESHOLD) {
        double kick = b.angularVelocity() * BALL_RADIUS * ROLLING_COUPLING;
        kick = clampRollKick(kick, normalImpulseMagnitude);
        b.setVelocity(b.velocity() + preVelocityB.normalized() * kick);
        b.setAngularVelocity(b.angularVelocity() - kick / BALL_RADIUS);
    }

    // 左/右塞切线摩擦响应。
    Vector2D tangent = MathUtils::tangent(normal);
    double vtA = a.velocity().dot(tangent);
    double vtB = b.velocity().dot(tangent);
    double spinContribution = (a.sideSpin() + b.sideSpin()) * BALL_RADIUS;
    double vtRel = (vtA - vtB) + spinContribution;

    if (std::abs(vtRel) > 1e-6) {
        double frictionImpulse = -vtRel * BALL_BALL_TANGENT_FRICTION;

        // 摩擦锥约束: |ft| ≤ μ * |fn|
        double maxFriction = normalImpulseMagnitude * BALL_BALL_TANGENT_FRICTION;
        frictionImpulse = std::clamp(frictionImpulse, -maxFriction, maxFriction);

        Vector2D tangentImpulse = tangent * frictionImpulse;

        a.setVelocity(a.velocity() + tangentImpulse);
        b.setVelocity(b.velocity() - tangentImpulse);

        double angularChange = frictionImpulse / (0.4 * BALL_RADIUS);
        a.setSideSpin(a.sideSpin() - angularChange);
        b.setSideSpin(b.sideSpin() - angularChange);
    }

    // 跟踪白球首次击中的球
    if (ioFirstHit && *ioFirstHit == BallType::White) {
        if (a.type() == BallType::White && b.type() != BallType::White) {
            *ioFirstHit = b.type();
        } else if (b.type() == BallType::White && a.type() != BallType::White) {
            *ioFirstHit = a.type();
        }
    }
}

void Physics::resolveCushionCollision(Ball& ball, const Vector2D& closestPoint,
                                      const Vector2D& inwardNormal) {
    const double signedDistance = MathUtils::dot(ball.position() - closestPoint, inwardNormal);

    // 位置修正，沿台内法线推出至刚好接触
    double overlap = BALL_RADIUS - signedDistance;
    if (overlap > 0.0) {
        ball.setPosition(ball.position() + inwardNormal * overlap);
    }

    // 速度反射（带库边恢复系数）
    double velAlongNormal = MathUtils::dot(ball.velocity(), inwardNormal);
    if (velAlongNormal < 0.0) {
        Vector2D reflected = ball.velocity() - inwardNormal * ((1.0 + CUSHION_RESTITUTION) * velAlongNormal);
        ball.setVelocity(reflected);

        // 切线方向摩擦（加塞）— 仅左右塞影响库边反弹角度。
        Vector2D tangent = MathUtils::tangent(inwardNormal);
        double vt = ball.velocity().dot(tangent);
        double spinVt = ball.sideSpin() * BALL_RADIUS;
        double slipSpeed = vt - spinVt;

        if (std::abs(slipSpeed) > 1e-6) {
            double frictionImpulse = -slipSpeed * CUSHION_TANGENT_FRICTION;

            // 摩擦锥约束: |ft| ≤ μ * |fn|
            double normalImpulseMag = (1.0 + CUSHION_RESTITUTION) * std::abs(velAlongNormal);
            double maxFriction = normalImpulseMag * CUSHION_TANGENT_FRICTION;
            frictionImpulse = std::clamp(frictionImpulse, -maxFriction, maxFriction);

            ball.setVelocity(ball.velocity() + tangent * frictionImpulse);

            double angularChange = frictionImpulse / (0.4 * BALL_RADIUS);
            ball.setSideSpin(ball.sideSpin() + angularChange);
        }
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
