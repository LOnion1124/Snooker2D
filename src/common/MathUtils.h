#pragma once

#include "Types.h"
#include <cmath>

namespace Snooker2D {
namespace MathUtils {

// 向量运算
double dot(const Vector2D& a, const Vector2D& b);
double cross(const Vector2D& a, const Vector2D& b);
double length(const Vector2D& v);
double lengthSquared(const Vector2D& v);
Vector2D normalize(const Vector2D& v);
Vector2D reflect(const Vector2D& v, const Vector2D& normal);

// 几何
double distance(const Vector2D& a, const Vector2D& b);
bool circleOverlap(const Vector2D& c1, double r1,
                   const Vector2D& c2, double r2);

// 点到线段最近点（用于球-库边碰撞检测）
Vector2D closestPointOnSegment(const Vector2D& p,
                                const Vector2D& a,
                                const Vector2D& b);

// 向量逆时针旋转 90°（用于加塞物理中从法线求切线方向）
Vector2D tangent(const Vector2D& v);

} // namespace MathUtils
} // namespace Snooker2D
