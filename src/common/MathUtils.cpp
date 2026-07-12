#include "MathUtils.h"

namespace Snooker2D {
namespace MathUtils {

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
    return Vector2D(v.x - 2.0 * d * normal.x,
                    v.y - 2.0 * d * normal.y);
}

double distance(const Vector2D& a, const Vector2D& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool circleOverlap(const Vector2D& c1, double r1,
                   const Vector2D& c2, double r2) {
    double dx = c1.x - c2.x;
    double dy = c1.y - c2.y;
    double rsum = r1 + r2;
    return (dx * dx + dy * dy) < (rsum * rsum);
}

Vector2D closestPointOnSegment(const Vector2D& p,
                                const Vector2D& a,
                                const Vector2D& b) {
    Vector2D ab = b - a;
    double t = dot(p - a, ab) / dot(ab, ab);
    t = (t < 0.0) ? 0.0 : (t > 1.0 ? 1.0 : t);
    return a + ab * t;
}

} // namespace MathUtils

// Vector2D 成员函数

Vector2D Vector2D::operator+(const Vector2D& o) const {
    return Vector2D(x + o.x, y + o.y);
}

Vector2D Vector2D::operator-(const Vector2D& o) const {
    return Vector2D(x - o.x, y - o.y);
}

Vector2D Vector2D::operator-() const {
    return Vector2D(-x, -y);
}

Vector2D Vector2D::operator*(double s) const {
    return Vector2D(x * s, y * s);
}

Vector2D Vector2D::operator/(double s) const {
    return Vector2D(x / s, y / s);
}

Vector2D& Vector2D::operator+=(const Vector2D& o) {
    x += o.x; y += o.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& o) {
    x -= o.x; y -= o.y;
    return *this;
}

Vector2D& Vector2D::operator*=(double s) {
    x *= s; y *= s;
    return *this;
}

Vector2D& Vector2D::operator/=(double s) {
    x /= s; y /= s;
    return *this;
}

bool Vector2D::operator==(const Vector2D& o) const {
    return x == o.x && y == o.y;
}

bool Vector2D::operator!=(const Vector2D& o) const {
    return !(*this == o);
}

double Vector2D::dot(const Vector2D& o) const {
    return MathUtils::dot(*this, o);
}

double Vector2D::cross(const Vector2D& o) const {
    return MathUtils::cross(*this, o);
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
