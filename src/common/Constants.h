#pragma once

namespace Snooker2D {

// 应用配置
constexpr const char* APP_NAME = "Snooker2D";
constexpr const char* APP_VERSION = "1.0.0";
constexpr int DEFAULT_WINDOW_WIDTH = 1280;
constexpr int DEFAULT_WINDOW_HEIGHT = 720;

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

// D 区（开球或白球落袋后放置区域）
constexpr double BAULK_LINE_X  = -TABLE_WIDTH * 0.3; // 开球线 X (中心坐标)
constexpr double D_RADIUS      = TABLE_HEIGHT * 0.35 / 2.0; // D 区半圆半径

// 物理
constexpr double DELTA_TIME            = 1.0 / 60.0;
constexpr double FRICTION_COEFFICIENT  = 0.99;
constexpr double COLLISION_RESTITUTION   = 0.88;
constexpr double CUSHION_RESTITUTION     = 0.77;
constexpr double MIN_VELOCITY            = 0.05;

// 游戏
constexpr int    MAX_POWER = 100;
constexpr double MAX_SPEED = 15.0;

// 加塞物理
constexpr double ENGLISH_TO_SPIN            = 50.0;  // english 到角速度转换
constexpr double BALL_BALL_TANGENT_FRICTION = 0.15;  // 球-球碰撞切线摩擦系数
constexpr double CUSHION_TANGENT_FRICTION   = 0.25;  // 球-库边碰撞切线摩擦系数
constexpr double SPIN_DECAY                 = 0.98;  // 自旋每帧衰减因子
constexpr double ROLLING_COUPLING           = 0.02;  // 滚动耦合强度

} // namespace Snooker2D
