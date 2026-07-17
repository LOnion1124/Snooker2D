#include "../src/model/GameState.h"
#include "../src/common/Constants.h"
#include "../src/common/Types.h"

#include <iostream>

namespace Snooker2D {
namespace Test {

int passed = 0;
int failed = 0;

#define CHECK(cond, name) \
    do { \
        if (cond) { passed++; } \
        else { failed++; std::cerr << "  FAIL: " << name << std::endl; } \
    } while (0)

void testOpeningRequiresWhiteBallPlacement() {
    GameState state;
    state.startNewGame();

    CHECK(state.currentPhase() == GamePhase::RedBall, "opening-phase-red-ball");
    CHECK(state.isWhiteBallPlacing(), "opening-white-ball-placement-started");
}

void testOpeningPlacementKeepsRedBallPhase() {
    GameState state;
    state.startNewGame();

    state.placeWhiteBall(Vector2D(BAULK_LINE_X - D_RADIUS * 0.5, 0.0));

    CHECK(!state.isWhiteBallPlacing(), "opening-white-ball-placement-finished");
    CHECK(state.currentPhase() == GamePhase::RedBall, "opening-placement-keeps-red-ball-phase");
}

} // namespace Test
} // namespace Snooker2D

int main() {
    using namespace Snooker2D::Test;

    std::cout << "=== GameState 单元测试 ===" << std::endl << std::endl;

    std::cout << "[opening placement]" << std::endl;
    testOpeningRequiresWhiteBallPlacement();
    testOpeningPlacementKeepsRedBallPhase();

    std::cout << std::endl;
    std::cout << "通过: " << passed << std::endl;
    std::cout << "失败: " << failed << std::endl;

    return failed == 0 ? 0 : 1;
}
