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

} // namespace MathUtils
} // namespace Snooker2D
