#pragma once

namespace Snooker2D {

struct Vector2D {
    double x = 0.0;
    double y = 0.0;

    Vector2D() = default;
    Vector2D(double x, double y) : x(x), y(y) {}

    Vector2D operator+(const Vector2D& o) const;
    Vector2D operator-(const Vector2D& o) const;
    Vector2D operator-() const;
    Vector2D operator*(double s) const;
    Vector2D operator/(double s) const;

    Vector2D& operator+=(const Vector2D& o);
    Vector2D& operator-=(const Vector2D& o);
    Vector2D& operator*=(double s);
    Vector2D& operator/=(double s);

    bool operator==(const Vector2D& o) const;
    bool operator!=(const Vector2D& o) const;

    double dot(const Vector2D& o) const;
    double cross(const Vector2D& o) const;
    double length() const;
    double lengthSquared() const;
    Vector2D normalized() const;
};

// 球类型
enum class BallType {
    White = 0,
    Red, Yellow, Green, Brown, Blue, Pink, Black
};

inline constexpr int ballValue(BallType t) {
    switch (t) {
        case BallType::Red:    return 1;
        case BallType::Yellow: return 2;
        case BallType::Green:  return 3;
        case BallType::Brown:  return 4;
        case BallType::Blue:   return 5;
        case BallType::Pink:   return 6;
        case BallType::Black:  return 7;
        default:               return 0;
    }
}

inline constexpr bool isColorBall(BallType t) {
    return t >= BallType::Yellow && t <= BallType::Black;
}

// 犯规类型
enum class FoulType {
    None = 0,
    MissedAll,
    WrongBallFirst,
    WhitePocketed,
    NoBallHitCushion,
    BallOffTable,
    PushShot,
    Other
};

// 游戏阶段
enum class GamePhase {
    NotStarted,
    RedBall,
    ColorBall,
    FreeBall,
    Foul,
    GameOver
};

// 玩家
enum class PlayerId {
    Player1 = 0,
    Player2 = 1
};

inline constexpr PlayerId oppositePlayer(PlayerId p) {
    return (p == PlayerId::Player1) ? PlayerId::Player2 : PlayerId::Player1;
}

} // namespace Snooker2D
