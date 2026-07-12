#pragma once

namespace Snooker2D {

// 球桌尺寸（像素）
constexpr double TABLE_WIDTH  = 800.0;
constexpr double TABLE_HEIGHT = 400.0;

// 球参数
constexpr double BALL_RADIUS = 10.0;
constexpr double BALL_MASS   = 1.0;
constexpr int    RED_COUNT   = 15;
constexpr int    TOTAL_BALLS = 22;

// 袋口
constexpr double POCKET_RADIUS = 16.0;

// 物理
constexpr double DELTA_TIME            = 1.0 / 60.0;
constexpr double FRICTION_COEFFICIENT  = 0.99;
constexpr double COLLISION_RESTITUTION   = 0.88;
constexpr double CUSHION_RESTITUTION     = 0.77;
constexpr double MIN_VELOCITY            = 0.05;

// 游戏
constexpr int    MAX_POWER = 100;
constexpr double MAX_SPEED = 15.0;

} // namespace Snooker2D
