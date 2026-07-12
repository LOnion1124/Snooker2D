#ifndef PHYSICS_H
#define PHYSICS_H

#include <QObject>
#include <vector>

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
    void step(double deltaTime, std::vector<Ball*>& balls, const Table& table);

    // 检查所有球是否静止
    static bool allBallsStopped(const std::vector<Ball*>& balls);

private:
    void applyFriction(std::vector<Ball*>& balls, double deltaTime);
    void moveBalls(std::vector<Ball*>& balls, double deltaTime);
    void checkBallBallCollisions(std::vector<Ball*>& balls);
    void checkBallCushionCollisions(std::vector<Ball*>& balls, const Table& table);
    void checkPocketDetection(std::vector<Ball*>& balls, const Table& table);

    void resolveBallCollision(Ball& a, Ball& b);
    void resolveCushionCollision(Ball& ball, const Vector2D& closestPoint, double distance);

    // 硬边界约束 — 防止链式推出导致球越界
    static void applyHardConstraint(Ball& ball, double halfW, double halfH, double margin);
};

} // namespace Snooker2D

#endif // PHYSICS_H
