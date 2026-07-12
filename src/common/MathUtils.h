#pragma once

#ifndef MATHUTILS_H
#define MATHUTILS_H

#include "Types.h"
#include <cmath>

namespace Snooker2D {
namespace MathUtils {

// ============================================================================
// 向量运算
// ============================================================================
double dot(const Vector2D& a, const Vector2D& b);
double cross(const Vector2D& a, const Vector2D& b);
double length(const Vector2D& v);
double lengthSquared(const Vector2D& v);
Vector2D normalize(const Vector2D& v);
Vector2D reflect(const Vector2D& v, const Vector2D& normal);
Vector2D perpendicular(const Vector2D& v);   // 逆时针旋转 90°

// ============================================================================
// 几何运算
// ============================================================================
double distance(const Vector2D& a, const Vector2D& b);
bool circleOverlap(const Vector2D& c1, double r1, const Vector2D& c2, double r2);

// ============================================================================
// 通用工具
// ============================================================================
/// 线性插值。t=0 返回 a，t=1 返回 b
inline double lerp(double a, double b, double t) {
    return a + (b - a) * t;
}

/// 将 value 限制在 [lo, hi] 范围内
inline double clamp(double value, double lo, double hi) {
    return (value < lo) ? lo : (value > hi) ? hi : value;
}

/// 角度转弧度
inline constexpr double degToRad(double degrees) {
    return degrees * 0.017453292519943295;  // π / 180
}

/// 弧度转角度
inline constexpr double radToDeg(double radians) {
    return radians * 57.29577951308232;     // 180 / π
}

} // namespace MathUtils
} // namespace Snooker2D

#endif // MATHUTILS_H
