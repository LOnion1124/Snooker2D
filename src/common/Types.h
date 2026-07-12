#pragma once

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

    // 算数运算符
    Vector2D operator+(const Vector2D& other) const;
    Vector2D operator-(const Vector2D& other) const;
    Vector2D operator-() const;                  // 一元取反
    Vector2D operator*(double scalar) const;
    Vector2D operator/(double scalar) const;

    // 复合赋值运算符
    Vector2D& operator+=(const Vector2D& other);
    Vector2D& operator-=(const Vector2D& other);
    Vector2D& operator*=(double scalar);
    Vector2D& operator/=(double scalar);

    // 比较运算符
    bool operator==(const Vector2D& other) const;
    bool operator!=(const Vector2D& other) const;

    // 向量运算（委托给 MathUtils）
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
    Red,        // 红球（×15，1 分）
    Yellow,     // 黄球（2 分）
    Green,      // 绿球（3 分）
    Brown,      // 棕球（4 分）
    Blue,       // 蓝球（5 分）
    Pink,       // 粉球（6 分）
    Black       // 黑球（7 分）
};

/// 返回彩球对应的分值。红球=1，白球=0（母球无分）
inline constexpr int ballValue(BallType type) {
    switch (type) {
        case BallType::Red:    return 1;
        case BallType::Yellow: return 2;
        case BallType::Green:  return 3;
        case BallType::Brown:  return 4;
        case BallType::Blue:   return 5;
        case BallType::Pink:   return 6;
        case BallType::Black:  return 7;
        default:               return 0;  // White
    }
}

/// 判断是否为彩球（黄/绿/棕/蓝/粉/黑），区别于红球和白球
inline constexpr bool isColorBall(BallType type) {
    return type >= BallType::Yellow && type <= BallType::Black;
}

// ============================================================================
// 犯规类型
// ============================================================================
enum class FoulType {
    None = 0,
    MissedAll,          // 空杆（未击中任何目标球）
    WrongBallFirst,     // 首先击中的球类型不正确
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
enum class PlayerId {
    Player1 = 0,
    Player2 = 1
};

/// 返回另一方玩家
inline constexpr PlayerId oppositePlayer(PlayerId p) {
    return (p == PlayerId::Player1) ? PlayerId::Player2 : PlayerId::Player1;
}

} // namespace Snooker2D

#endif // TYPES_H
