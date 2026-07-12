#include "MathUtils.h"

namespace Snooker2D {
namespace MathUtils {

// ============================================================================
// 向量运算
// ============================================================================

double dot(const Vector2D& a, const Vector2D& b) {
    return a.x * b.x + a.y * b.y;
}

double cross(const Vector2D& a, const Vector2D& b) {
    return a.x * b.y - a.y * b.x;
}

double length(const Vector2D& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

double lengthSquared(const Vector2D& v) {
    return v.x * v.x + v.y * v.y;
}

Vector2D normalize(const Vector2D& v) {
    double len = length(v);
    if (len < 1e-9) return Vector2D(0.0, 0.0);
    return Vector2D(v.x / len, v.y / len);
}

Vector2D reflect(const Vector2D& v, const Vector2D& normal) {
    double d = dot(v, normal);
    return Vector2D(v.x - 2.0 * d * normal.x, v.y - 2.0 * d * normal.y);
}

Vector2D perpendicular(const Vector2D& v) {
    return Vector2D(-v.y, v.x);
}

// ============================================================================
// 几何运算
// ============================================================================

double distance(const Vector2D& a, const Vector2D& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool circleOverlap(const Vector2D& c1, double r1, const Vector2D& c2, double r2) {
    double dx = c1.x - c2.x;
    double dy = c1.y - c2.y;
    double rsum = r1 + r2;
    return (dx * dx + dy * dy) < (rsum * rsum);
}

} // namespace MathUtils

// ============================================================================
// Vector2D 成员函数实现
// ============================================================================

// --- 算数运算符 ---

Vector2D Vector2D::operator+(const Vector2D& other) const {
    return Vector2D(x + other.x, y + other.y);
}

Vector2D Vector2D::operator-(const Vector2D& other) const {
    return Vector2D(x - other.x, y - other.y);
}

Vector2D Vector2D::operator-() const {
    return Vector2D(-x, -y);
}

Vector2D Vector2D::operator*(double scalar) const {
    return Vector2D(x * scalar, y * scalar);
}

Vector2D Vector2D::operator/(double scalar) const {
    return Vector2D(x / scalar, y / scalar);
}

// --- 复合赋值运算符 ---

Vector2D& Vector2D::operator+=(const Vector2D& other) {
    x += other.x; y += other.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& other) {
    x -= other.x; y -= other.y;
    return *this;
}

Vector2D& Vector2D::operator*=(double scalar) {
    x *= scalar; y *= scalar;
    return *this;
}

Vector2D& Vector2D::operator/=(double scalar) {
    x /= scalar; y /= scalar;
    return *this;
}

// --- 比较运算符 ---

bool Vector2D::operator==(const Vector2D& other) const {
    return x == other.x && y == other.y;
}

bool Vector2D::operator!=(const Vector2D& other) const {
    return !(*this == other);
}

// --- 向量运算（委托给 MathUtils）---

double Vector2D::dot(const Vector2D& other) const {
    return MathUtils::dot(*this, other);
}

double Vector2D::cross(const Vector2D& other) const {
    return MathUtils::cross(*this, other);
}

double Vector2D::length() const {
    return MathUtils::length(*this);
}

double Vector2D::lengthSquared() const {
    return MathUtils::lengthSquared(*this);
}

Vector2D Vector2D::normalized() const {
    return MathUtils::normalize(*this);
}

} // namespace Snooker2D
