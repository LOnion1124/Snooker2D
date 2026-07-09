#ifndef TYPES_H
#define TYPES_H

namespace Snooker2D {

// ============================================================================
// 二维向量
// ============================================================================
struct Vector2D {
    double x = 0.0;
    double y = 0.0;

    Vector2D() = default;
    Vector2D(double x, double y) : x(x), y(y) {}

    Vector2D operator+(const Vector2D& other) const;
    Vector2D operator-(const Vector2D& other) const;
    Vector2D operator*(double scalar) const;
    Vector2D operator/(double scalar) const;
    double dot(const Vector2D& other) const;
    double cross(const Vector2D& other) const;
    double length() const;
    double lengthSquared() const;
    Vector2D normalized() const;
};

// ============================================================================
// 球的类型（斯诺克 22 颗球）
// ============================================================================
enum class BallType {
    White = 0,  // 白球（母球）
    Red,        // 红球（×15）
    Yellow,     // 黄球（2 分）
    Green,      // 绿球（3 分）
    Brown,      // 棕球（4 分）
    Blue,       // 蓝球（5 分）
    Pink,       // 粉球（6 分）
    Black       // 黑球（7 分）
};

// ============================================================================
// 犯规类型
// ============================================================================
enum class FoulType {
    None = 0,
    MissedAll,          // 空杆（未击中任何目标球）
    WrongBallFirst,     // 首先击中的球不正确
    WhitePocketed,      // 白球落袋
    NoBallHitCushion,   // 无球碰库
    BallOffTable,       // 球飞出台面
    PushShot,           // 推杆
    Other               // 其他犯规
};

// ============================================================================
// 游戏阶段
// ============================================================================
enum class GamePhase {
    NotStarted,     // 未开始
    RedBall,        // 应击红球阶段
    ColorBall,      // 应击彩球阶段
    FreeBall,       // 自由球
    Foul,           // 犯规（等待确认）
    GameOver        // 比赛结束
};

// ============================================================================
// 玩家标识
// ============================================================================
enum class Player {
    Player1 = 0,
    Player2 = 1
};

} // namespace Snooker2D

#endif // TYPES_H
