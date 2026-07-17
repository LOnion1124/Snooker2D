#include "../src/model/Ball.h"
#include "../src/model/Physics.h"
#include "../src/model/Table.h"
#include "../src/common/Constants.h"
#include "../src/common/Types.h"

#include <cmath>
#include <iostream>
#include <vector>

namespace Snooker2D {
namespace Test {

int passed = 0;
int failed = 0;

#define CHECK(cond, name) \
    do { \
        if (cond) { passed++; } \
        else { failed++; std::cerr << "  FAIL: " << name << std::endl; } \
    } while (0)

bool near(double a, double b, double eps = 1e-9) {
    return std::abs(a - b) < eps;
}

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

} // namespace Test
} // namespace Snooker2D

int main() {
    using namespace Snooker2D::Test;

    std::cout << "=== Physics 单元测试 ===" << std::endl << std::endl;

    std::cout << "[cushion overshoot]" << std::endl;
    testTopCushionOvershootReflectsInward();
    testRightCushionOvershootReflectsInward();

    std::cout << std::endl;
    std::cout << "通过: " << passed << std::endl;
    std::cout << "失败: " << failed << std::endl;

    return failed == 0 ? 0 : 1;
}
