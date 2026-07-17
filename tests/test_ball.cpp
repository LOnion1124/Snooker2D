#include "../src/model/Ball.h"
#include "../src/common/Constants.h"
#include "../src/common/Types.h"

#include "test_utils.h"

#include <iostream>

namespace Snooker2D {
namespace Test {

void testConstruction() {
    Ball white(BallType::White);
    Ball red(BallType::Red);
    Ball black(BallType::Black);

    CHECK(white.type() == BallType::White, "ball-type-white");
    CHECK(red.type() == BallType::Red, "ball-type-red");
    CHECK(black.type() == BallType::Black, "ball-type-black");
}

void testPointValue() {
    CHECK(Ball(BallType::White).pointValue() == 0, "pointValue-white");
    CHECK(Ball(BallType::Red).pointValue() == 1, "pointValue-red");
    CHECK(Ball(BallType::Yellow).pointValue() == 2, "pointValue-yellow");
    CHECK(Ball(BallType::Green).pointValue() == 3, "pointValue-green");
    CHECK(Ball(BallType::Brown).pointValue() == 4, "pointValue-brown");
    CHECK(Ball(BallType::Blue).pointValue() == 5, "pointValue-blue");
    CHECK(Ball(BallType::Pink).pointValue() == 6, "pointValue-pink");
    CHECK(Ball(BallType::Black).pointValue() == 7, "pointValue-black");
}

void testDefaultState() {
    Ball ball(BallType::Red);
    CHECK(vecNear(ball.position(), Vector2D(0, 0)), "ball-default-position");
    CHECK(vecNear(ball.velocity(), Vector2D(0, 0)), "ball-default-velocity");
    CHECK(near(ball.angularVelocity(), 0.0), "ball-default-angular-velocity");
    CHECK(near(ball.sideSpin(), 0.0), "ball-default-side-spin");
    CHECK(!ball.isPocketed(), "ball-default-not-pocketed");
    CHECK(ball.isOnTable(), "ball-default-on-table");
}

void testSetPosition() {
    Ball ball(BallType::Red);
    bool emitted = false;
    QObject::connect(&ball, &Ball::positionChanged, [&]() { emitted = true; });

    ball.setPosition(Vector2D(100, 200));
    CHECK(vecNear(ball.position(), Vector2D(100, 200)), "setPosition-updates-position");
    CHECK(emitted, "setPosition-emits-positionChanged");
}

void testSetVelocity() {
    Ball ball(BallType::Red);
    ball.setVelocity(Vector2D(5, -3));
    CHECK(vecNear(ball.velocity(), Vector2D(5, -3)), "setVelocity-updates");
}

void testSetAngularVelocity() {
    Ball ball(BallType::Red);
    ball.setAngularVelocity(30.0);
    CHECK(near(ball.angularVelocity(), 30.0), "setAngularVelocity-positive");

    ball.setAngularVelocity(-20.0);
    CHECK(near(ball.angularVelocity(), -20.0), "setAngularVelocity-negative");
}

void testSetSideSpin() {
    Ball ball(BallType::Red);
    ball.setSideSpin(15.0);
    CHECK(near(ball.sideSpin(), 15.0), "setSideSpin-positive");

    ball.setSideSpin(-10.0);
    CHECK(near(ball.sideSpin(), -10.0), "setSideSpin-negative");
}

void testSetSpin() {
    Ball ball(BallType::Red);
    ball.setSpin(20.0, -5.0);
    CHECK(near(ball.angularVelocity(), 20.0), "setSpin-roll");
    CHECK(near(ball.sideSpin(), -5.0), "setSpin-side");
}

void testSetPocketed() {
    Ball ball(BallType::Red);
    ball.setVelocity(Vector2D(10, 10));
    ball.setAngularVelocity(5.0);
    ball.setSideSpin(3.0);

    bool pocketedSignal = false;
    QObject::connect(&ball, &Ball::pocketed, [&]() { pocketedSignal = true; });

    ball.setPocketed(true);
    CHECK(ball.isPocketed(), "setPocketed-true-pocketed");
    CHECK(!ball.isOnTable(), "setPocketed-true-off-table");
    CHECK(vecNear(ball.velocity(), Vector2D(0, 0)), "setPocketed-clears-velocity");
    CHECK(near(ball.angularVelocity(), 0.0), "setPocketed-clears-roll-spin");
    CHECK(near(ball.sideSpin(), 0.0), "setPocketed-clears-side-spin");
    CHECK(pocketedSignal, "setPocketed-emits-pocketed");

    // 再设回 false
    ball.setPocketed(false);
    CHECK(!ball.isPocketed(), "setPocketed-false");
}

void testSetOnTable() {
    Ball ball(BallType::Red);
    ball.setOnTable(false);
    CHECK(!ball.isOnTable(), "setOnTable-false");

    ball.setOnTable(true);
    CHECK(ball.isOnTable(), "setOnTable-true");
}

void testResetPosition() {
    Ball ball(BallType::Red);
    ball.setVelocity(Vector2D(5, 5));
    ball.setAngularVelocity(10.0);
    ball.setSideSpin(5.0);

    bool resetSignal = false;
    QObject::connect(&ball, &Ball::ballReset, [&]() { resetSignal = true; });

    ball.resetPosition(Vector2D(100, 200));
    CHECK(vecNear(ball.position(), Vector2D(100, 200)), "resetPosition-updates-position");
    CHECK(vecNear(ball.velocity(), Vector2D(0, 0)), "resetPosition-clears-velocity");
    CHECK(near(ball.angularVelocity(), 0.0), "resetPosition-clears-roll-spin");
    CHECK(near(ball.sideSpin(), 0.0), "resetPosition-clears-side-spin");
    CHECK(!ball.isPocketed(), "resetPosition-not-pocketed");
    CHECK(ball.isOnTable(), "resetPosition-on-table");
    CHECK(resetSignal, "resetPosition-emits-ballReset");
}

void testRespot() {
    Ball ball(BallType::Red);
    ball.resetPosition(Vector2D(100, 200)); // 设置初始位置
    ball.setPosition(Vector2D(50, 50));
    ball.setVelocity(Vector2D(3, 3));
    ball.setAngularVelocity(5.0);

    bool resetSignal = false;
    QObject::connect(&ball, &Ball::ballReset, [&]() { resetSignal = true; });

    ball.respot();
    CHECK(vecNear(ball.position(), Vector2D(100, 200)), "respot-returns-to-initial");
    CHECK(vecNear(ball.velocity(), Vector2D(0, 0)), "respot-clears-velocity");
    CHECK(near(ball.angularVelocity(), 0.0), "respot-clears-spin");
    CHECK(!ball.isPocketed(), "respot-not-pocketed");
    CHECK(ball.isOnTable(), "respot-on-table");
    CHECK(resetSignal, "respot-emits-ballReset");
}

} // namespace Test
} // namespace Snooker2D

int main() {
    using namespace Snooker2D::Test;

    std::cout << "=== Ball 单元测试 ===" << std::endl << std::endl;

    std::cout << "[construction]" << std::endl;
    testConstruction();
    testPointValue();
    testDefaultState();

    std::cout << "[getter/setter]" << std::endl;
    testSetPosition();
    testSetVelocity();
    testSetAngularVelocity();
    testSetSideSpin();
    testSetSpin();

    std::cout << "[pocketed/onTable]" << std::endl;
    testSetPocketed();
    testSetOnTable();

    std::cout << "[reset/respot]" << std::endl;
    testResetPosition();
    testRespot();

    std::cout << std::endl;
    std::cout << "通过: " << passed << std::endl;
    std::cout << "失败: " << failed << std::endl;

    return failed == 0 ? 0 : 1;
}
