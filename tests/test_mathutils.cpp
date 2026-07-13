#include "../src/common/MathUtils.h"
#include "../src/common/Constants.h"
#include "../src/common/Types.h"

#include <iostream>
#include <cmath>
#include <cassert>

namespace Snooker2D {
namespace Test {

// 测试辅助：浮点比较
bool near(double a, double b, double eps = 1e-9) {
    return std::abs(a - b) < eps;
}

bool vecNear(const Vector2D& a, const Vector2D& b, double eps = 1e-9) {
    return near(a.x, b.x, eps) && near(a.y, b.y, eps);
}

int passed = 0;
int failed = 0;

#define CHECK(cond, name) \
    do { \
        if (cond) { passed++; } \
        else { failed++; std::cerr << "  FAIL: " << name << std::endl; } \
    } while(0)

#define CHECK_NEAR(a, b, name) CHECK(near(a, b), name)
#define CHECK_VEC_NEAR(a, b, name) CHECK(vecNear(a, b), name)

// --- 向量运算 ---

void testDot() {
    CHECK_NEAR(MathUtils::dot(Vector2D(1, 0), Vector2D(0, 1)), 0.0, "dot-正交");
    CHECK_NEAR(MathUtils::dot(Vector2D(2, 0), Vector2D(3, 0)), 6.0, "dot-同向");
    CHECK_NEAR(MathUtils::dot(Vector2D(1, 2), Vector2D(3, 4)), 11.0, "dot-一般");
}

void testCross() {
    CHECK_NEAR(MathUtils::cross(Vector2D(1, 0), Vector2D(0, 1)), 1.0, "cross-正交");
    CHECK_NEAR(MathUtils::cross(Vector2D(0, 1), Vector2D(1, 0)), -1.0, "cross-反向");
    CHECK_NEAR(MathUtils::cross(Vector2D(2, 0), Vector2D(3, 0)), 0.0, "cross-平行");
}

void testLength() {
    CHECK_NEAR(MathUtils::length(Vector2D(3, 4)), 5.0, "length-345");
    CHECK_NEAR(MathUtils::length(Vector2D(0, 0)), 0.0, "length-零");
    CHECK_NEAR(MathUtils::length(Vector2D(-3, 0)), 3.0, "length-负");
}

void testLengthSquared() {
    CHECK_NEAR(MathUtils::lengthSquared(Vector2D(3, 4)), 25.0, "lengthSquared-345");
    CHECK_NEAR(MathUtils::lengthSquared(Vector2D(0, 0)), 0.0, "lengthSquared-零");
}

void testNormalize() {
    CHECK_VEC_NEAR(MathUtils::normalize(Vector2D(5, 0)), Vector2D(1, 0), "normalize-水平");
    CHECK_VEC_NEAR(MathUtils::normalize(Vector2D(0, 3)), Vector2D(0, 1), "normalize-垂直");
    CHECK_VEC_NEAR(MathUtils::normalize(Vector2D(0, 0)), Vector2D(0, 0), "normalize-零");
    Vector2D n = MathUtils::normalize(Vector2D(3, 4));
    CHECK(near(MathUtils::length(n), 1.0), "normalize-单位长度");
}

void testReflect() {
    // 垂直入射，法线向上 → 反射向上
    Vector2D r = MathUtils::reflect(Vector2D(1, -1), Vector2D(0, 1));
    CHECK_VEC_NEAR(r, Vector2D(1, 1), "reflect-垂直入射");

    // 水平撞击垂直墙
    r = MathUtils::reflect(Vector2D(-1, 0), Vector2D(1, 0));
    CHECK_VEC_NEAR(r, Vector2D(1, 0), "reflect-正撞墙壁");
}

// --- 几何 ---

void testDistance() {
    CHECK_NEAR(MathUtils::distance(Vector2D(0, 0), Vector2D(3, 4)), 5.0, "distance-345");
    CHECK_NEAR(MathUtils::distance(Vector2D(1, 2), Vector2D(1, 2)), 0.0, "distance-同点");
}

void testCircleOverlap() {
    CHECK(MathUtils::circleOverlap(Vector2D(0, 0), 2, Vector2D(2, 0), 1),
          "circleOverlap-重叠");
    CHECK(!MathUtils::circleOverlap(Vector2D(0, 0), 2, Vector2D(10, 0), 1),
          "circleOverlap-分离");
    // 刚好相切（严格 <，相切不算重叠）
    CHECK(!MathUtils::circleOverlap(Vector2D(0, 0), 2, Vector2D(3, 0), 1),
          "circleOverlap-相切");
}

void testClosestPointOnSegment() {
    // 点在线段投影范围内
    CHECK_VEC_NEAR(MathUtils::closestPointOnSegment(Vector2D(0, 0),
                     Vector2D(2, -1), Vector2D(2, 1)),
                   Vector2D(2, 0), "closest-中间");

    // 点在线段延长线上，应钳制到端点
    CHECK_VEC_NEAR(MathUtils::closestPointOnSegment(Vector2D(0, 0),
                     Vector2D(2, 2), Vector2D(4, 4)),
                   Vector2D(2, 2), "closest-端点前");

    // 点在线段另一端延长线
    CHECK_VEC_NEAR(MathUtils::closestPointOnSegment(Vector2D(5, 5),
                     Vector2D(0, 0), Vector2D(3, 3)),
                   Vector2D(3, 3), "closest-端点后");
}

// --- Vector2D 运算符 ---

void testVectorAdd() {
    Vector2D r = Vector2D(1, 2) + Vector2D(3, 4);
    CHECK_VEC_NEAR(r, Vector2D(4, 6), "operator+");
}

void testVectorSubtract() {
    Vector2D r = Vector2D(5, 3) - Vector2D(2, 1);
    CHECK_VEC_NEAR(r, Vector2D(3, 2), "operator-");
}

void testVectorNegate() {
    Vector2D r = -Vector2D(3, -4);
    CHECK_VEC_NEAR(r, Vector2D(-3, 4), "operator-一元");
}

void testVectorMultiply() {
    CHECK_VEC_NEAR(Vector2D(2, 3) * 2.0, Vector2D(4, 6), "operator*");
}

void testVectorDivide() {
    CHECK_VEC_NEAR(Vector2D(6, 8) / 2.0, Vector2D(3, 4), "operator/");
}

void testCompoundAssign() {
    Vector2D v(1, 2);
    v += Vector2D(3, 4);
    CHECK_VEC_NEAR(v, Vector2D(4, 6), "+=");

    v -= Vector2D(2, 1);
    CHECK_VEC_NEAR(v, Vector2D(2, 5), "-=");

    v *= 2.0;
    CHECK_VEC_NEAR(v, Vector2D(4, 10), "*=");

    v /= 2.0;
    CHECK_VEC_NEAR(v, Vector2D(2, 5), "/=");
}

void testCompare() {
    CHECK(Vector2D(1, 2) == Vector2D(1, 2), "==");
    CHECK(Vector2D(1, 2) != Vector2D(3, 4), "!=");
}

void testVectorDot() {
    CHECK_NEAR(Vector2D(1, 0).dot(Vector2D(0, 1)), 0.0, "Vec::dot-正交");
}

void testVectorCross() {
    CHECK_NEAR(Vector2D(1, 0).cross(Vector2D(0, 1)), 1.0, "Vec::cross-正交");
}

void testVectorLength() {
    CHECK_NEAR(Vector2D(3, 4).length(), 5.0, "Vec::length-345");
}

void testVectorNormalized() {
    CHECK_NEAR(Vector2D(5, 0).normalized().length(), 1.0, "Vec::normalized-单位");
}

// --- 类型辅助 ---

void testBallValue() {
    CHECK(ballValue(BallType::Red) == 1, "ballValue-Red");
    CHECK(ballValue(BallType::Yellow) == 2, "ballValue-Yellow");
    CHECK(ballValue(BallType::Green) == 3, "ballValue-Green");
    CHECK(ballValue(BallType::Brown) == 4, "ballValue-Brown");
    CHECK(ballValue(BallType::Blue) == 5, "ballValue-Blue");
    CHECK(ballValue(BallType::Pink) == 6, "ballValue-Pink");
    CHECK(ballValue(BallType::Black) == 7, "ballValue-Black");
    CHECK(ballValue(BallType::White) == 0, "ballValue-White");
}

void testIsColorBall() {
    CHECK(!isColorBall(BallType::White), "isColorBall-White");
    CHECK(!isColorBall(BallType::Red), "isColorBall-Red");
    CHECK(isColorBall(BallType::Blue), "isColorBall-Blue");
    CHECK(isColorBall(BallType::Black), "isColorBall-Black");
}

void testOppositePlayer() {
    CHECK(oppositePlayer(PlayerId::Player1) == PlayerId::Player2, "opposite-P1");
    CHECK(oppositePlayer(PlayerId::Player2) == PlayerId::Player1, "opposite-P2");
}

// --- 常量 ---

void testConstants() {
    CHECK(TABLE_WIDTH == 800.0, "TABLE_WIDTH");
    CHECK(TABLE_HEIGHT == 400.0, "TABLE_HEIGHT");
    CHECK(BALL_RADIUS == 10.0, "BALL_RADIUS");
    CHECK(RED_COUNT == 15, "RED_COUNT");
    CHECK(TOTAL_BALLS == 22, "TOTAL_BALLS");
    CHECK(MAX_POWER == 100, "MAX_POWER");
}

} // namespace Test
} // namespace Snooker2D

int main() {
    using namespace Snooker2D::Test;

    std::cout << "=== MathUtils 单元测试 ===" << std::endl << std::endl;

    std::cout << "[dot]" << std::endl;
    testDot();

    std::cout << "[cross]" << std::endl;
    testCross();

    std::cout << "[length]" << std::endl;
    testLength();

    std::cout << "[lengthSquared]" << std::endl;
    testLengthSquared();

    std::cout << "[normalize]" << std::endl;
    testNormalize();

    std::cout << "[reflect]" << std::endl;
    testReflect();

    std::cout << "[distance]" << std::endl;
    testDistance();

    std::cout << "[circleOverlap]" << std::endl;
    testCircleOverlap();

    std::cout << "[closestPointOnSegment]" << std::endl;
    testClosestPointOnSegment();

    std::cout << std::endl << "=== Vector2D 运算符 ===" << std::endl << std::endl;

    std::cout << "[+/ -/ 一元- / * /  /]" << std::endl;
    testVectorAdd();
    testVectorSubtract();
    testVectorNegate();
    testVectorMultiply();
    testVectorDivide();

    std::cout << "[+= / -= / *= /  /=]" << std::endl;
    testCompoundAssign();

    std::cout << "[== / !=]" << std::endl;
    testCompare();

    std::cout << "[dot / cross / length / normalized]" << std::endl;
    testVectorDot();
    testVectorCross();
    testVectorLength();
    testVectorNormalized();

    std::cout << std::endl << "=== Types 辅助函数 ===" << std::endl << std::endl;

    std::cout << "[ballValue]" << std::endl;
    testBallValue();

    std::cout << "[isColorBall]" << std::endl;
    testIsColorBall();

    std::cout << "[oppositePlayer]" << std::endl;
    testOppositePlayer();

    std::cout << std::endl << "=== 常量校验 ===" << std::endl << std::endl;
    testConstants();

    std::cout << std::endl;
    std::cout << "通过: " << passed << std::endl;
    std::cout << "失败: " << failed << std::endl;

    return (failed == 0) ? 0 : 1;
}
