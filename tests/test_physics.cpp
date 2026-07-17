#include "../src/model/Ball.h"
#include "../src/model/Physics.h"
#include "../src/model/Table.h"
#include "../src/common/Constants.h"
#include "../src/common/Types.h"

#include "test_utils.h"

#include <cmath>
#include <iostream>
#include <vector>

namespace Snooker2D {
namespace Test {

void testTopCushionOvershootReflectsInward() {
    Table table;
    Physics physics;
    Ball ball(BallType::White);
    ball.resetPosition(Vector2D(100.0, -TABLE_HEIGHT / 2.0 + BALL_RADIUS + 1.0));
    ball.setVelocity(Vector2D(10.0, -15.0));

    std::vector<Ball*> balls{&ball};
    bool cushionHit = false;
    physics.step(DELTA_TIME, balls, table, nullptr, &cushionHit);

    CHECK(cushionHit, "top-overshoot-cushion-hit");
    CHECK(ball.position().y >= -TABLE_HEIGHT / 2.0 + BALL_RADIUS
              || near(ball.position().y, -TABLE_HEIGHT / 2.0 + BALL_RADIUS),
          "top-overshoot-position-corrected");
    CHECK(ball.velocity().y > 0.0, "top-overshoot-reflected-inward");
}

void testRightCushionOvershootReflectsInward() {
    Table table;
    Physics physics;
    Ball ball(BallType::White);
    ball.resetPosition(Vector2D(TABLE_WIDTH / 2.0 - BALL_RADIUS - 1.0, 80.0));
    ball.setVelocity(Vector2D(15.0, 5.0));

    std::vector<Ball*> balls{&ball};
    bool cushionHit = false;
    physics.step(DELTA_TIME, balls, table, nullptr, &cushionHit);

    CHECK(cushionHit, "right-overshoot-cushion-hit");
    CHECK(ball.position().x <= TABLE_WIDTH / 2.0 - BALL_RADIUS
              || near(ball.position().x, TABLE_WIDTH / 2.0 - BALL_RADIUS),
          "right-overshoot-position-corrected");
    CHECK(ball.velocity().x < 0.0, "right-overshoot-reflected-inward");
}

void testSideSpinChangesCushionTangentVelocity() {
    Table table;
    Physics physics;
    Ball plain(BallType::White);
    Ball spun(BallType::White);

    plain.resetPosition(Vector2D(TABLE_WIDTH / 2.0 - BALL_RADIUS - 1.0, 80.0));
    plain.setVelocity(Vector2D(15.0, 5.0));

    spun.resetPosition(Vector2D(TABLE_WIDTH / 2.0 - BALL_RADIUS - 1.0, 80.0));
    spun.setVelocity(Vector2D(15.0, 5.0));
    spun.setSideSpin(ENGLISH_TO_SPIN);

    std::vector<Ball*> plainBalls{&plain};
    std::vector<Ball*> spunBalls{&spun};
    physics.step(DELTA_TIME, plainBalls, table);
    physics.step(DELTA_TIME, spunBalls, table);

    CHECK(std::abs(spun.velocity().y - plain.velocity().y) > 0.5,
          "side-spin-changes-cushion-tangent-velocity");
    CHECK(spun.velocity().x < 0.0, "side-spin-still-reflects-inward");
}

void testFollowAndDrawChangeCueBallAfterObjectCollision() {
    Table table;
    Physics physics;
    Ball follow(BallType::White);
    Ball followTarget(BallType::Red);
    Ball draw(BallType::White);
    Ball drawTarget(BallType::Red);

    follow.resetPosition(Vector2D(-30.0, 0.0));
    follow.setVelocity(Vector2D(10.0, 0.0));
    follow.setAngularVelocity(ENGLISH_TO_SPIN);
    followTarget.resetPosition(Vector2D(-11.0, 0.0));

    draw.resetPosition(Vector2D(-30.0, 40.0));
    draw.setVelocity(Vector2D(10.0, 0.0));
    draw.setAngularVelocity(-ENGLISH_TO_SPIN);
    drawTarget.resetPosition(Vector2D(-11.0, 40.0));

    std::vector<Ball*> followBalls{&follow, &followTarget};
    std::vector<Ball*> drawBalls{&draw, &drawTarget};
    physics.step(DELTA_TIME, followBalls, table);
    physics.step(DELTA_TIME, drawBalls, table);

    CHECK(follow.velocity().x > 0.0, "follow-spin-cue-ball-continues-forward");
    CHECK(follow.velocity().x < 1.2, "follow-spin-does-not-surge-forward");
    CHECK(draw.velocity().x < 0.0, "draw-spin-cue-ball-pulls-back");
    CHECK(follow.velocity().x > draw.velocity().x, "follow-draw-collision-difference");
}

void testFollowSpinDoesNotOverAccelerateWhileRolling() {
    Table table;
    Physics physics;
    Ball ball(BallType::White);
    ball.resetPosition(Vector2D(-100.0, 0.0));
    ball.setVelocity(Vector2D(5.0, 0.0));
    ball.setAngularVelocity(ENGLISH_TO_SPIN);

    std::vector<Ball*> balls{&ball};
    for (int i = 0; i < 30; ++i) {
        physics.step(DELTA_TIME, balls, table);
    }

    CHECK(ball.velocity().x > 4.0, "follow-spin-keeps-ball-moving");
    CHECK(ball.velocity().x < 6.0, "follow-spin-rolling-coupling-is-bounded");
}

void testSmallResidualSpinIsCleared() {
    Table table;
    Physics physics;
    Ball ball(BallType::White);
    ball.resetPosition(Vector2D(0.0, 0.0));
    ball.setAngularVelocity(0.04);
    ball.setSideSpin(-0.04);

    std::vector<Ball*> balls{&ball};
    CHECK(Physics::allBallsStopped(balls), "small-residual-spin-counts-as-stopped");
    physics.step(DELTA_TIME, balls, table);

    CHECK(ball.angularVelocity() == 0.0, "small-roll-spin-cleared");
    CHECK(ball.sideSpin() == 0.0, "small-side-spin-cleared");
}

} // namespace Test
} // namespace Snooker2D

int main() {
    using namespace Snooker2D::Test;

    std::cout << "=== Physics 单元测试 ===" << std::endl << std::endl;

    std::cout << "[cushion overshoot]" << std::endl;
    testTopCushionOvershootReflectsInward();
    testRightCushionOvershootReflectsInward();

    std::cout << "[english physics]" << std::endl;
    testSideSpinChangesCushionTangentVelocity();
    testFollowAndDrawChangeCueBallAfterObjectCollision();
    testFollowSpinDoesNotOverAccelerateWhileRolling();
    testSmallResidualSpinIsCleared();

    std::cout << std::endl;
    std::cout << "通过: " << passed << std::endl;
    std::cout << "失败: " << failed << std::endl;

    return failed == 0 ? 0 : 1;
}
