#ifndef PHYSICS_H
#define PHYSICS_H

#include <QObject>
#include <vector>

#include "../common/Types.h"

namespace Snooker2D {

class Ball;
class Table;

struct Vector2D;

class Physics : public QObject {
    Q_OBJECT

public:
    explicit Physics(QObject* parent = nullptr);
    ~Physics() override = default;

    // 执行一帧物理模拟（固定时间步长）
    // outFirstHit: 如果白球首次碰到非白球，传出该球类型（仅首次，传入 White 表示未决）
    // outCushionHit: 传出本帧是否有任何球碰库
    void step(double deltaTime, std::vector<Ball*>& balls, const Table& table,
              BallType* outFirstHit = nullptr, bool* outCushionHit = nullptr);

    // 检查所有球是否静止
    static bool allBallsStopped(const std::vector<Ball*>& balls);

private:
    void applyFriction(std::vector<Ball*>& balls, double deltaTime);
    void moveBalls(std::vector<Ball*>& balls, double deltaTime);
    void checkBallBallCollisions(std::vector<Ball*>& balls, BallType* ioFirstHit = nullptr);
    void checkBallCushionCollisions(std::vector<Ball*>& balls, const Table& table, bool* outCushionHit = nullptr);
    void checkPocketDetection(std::vector<Ball*>& balls, const Table& table);

    void resolveBallCollision(Ball& a, Ball& b, BallType* ioFirstHit = nullptr);
    void resolveCushionCollision(Ball& ball, const Vector2D& closestPoint,
                                 const Vector2D& inwardNormal);

    // 硬边界约束，防止链式推出导致球越界
    static void applyHardConstraint(Ball& ball, double halfW, double halfH, double margin);
    static void checkPullBackBalls(std::vector<Ball*>& balls, const Table& table);
};

} // namespace Snooker2D

#endif // PHYSICS_H
