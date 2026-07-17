#pragma once

#include <iostream>
#include <cmath>
#include "../src/common/Types.h"

namespace Snooker2D {
namespace Test {

inline int passed = 0;
inline int failed = 0;

#define CHECK(cond, name) \
    do { \
        if (cond) { (passed)++; } \
        else { (failed)++; std::cerr << "  FAIL: " << name << std::endl; } \
    } while(0)

inline bool near(double a, double b, double eps = 1e-9) {
    return std::abs(a - b) < eps;
}

inline bool vecNear(const Vector2D& a, const Vector2D& b, double eps = 1e-9) {
    return near(a.x, b.x, eps) && near(a.y, b.y, eps);
}

#define CHECK_NEAR(a, b, name) CHECK(::Snooker2D::Test::near(a, b), name)
#define CHECK_VEC_NEAR(a, b, name) CHECK(::Snooker2D::Test::vecNear(a, b), name)

} // namespace Test
} // namespace Snooker2D
