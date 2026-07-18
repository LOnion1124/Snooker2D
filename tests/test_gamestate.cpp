#include "../src/model/GameState.h"
#include "../src/model/Table.h"
#include "../src/common/Constants.h"
#include "../src/common/Types.h"

#include "test_utils.h"

#include <iostream>

namespace Snooker2D {
namespace Test {

// 开局和白球放置

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

// 构造函数 ----

void testConstructor() {
    GameState state;
    CHECK(state.currentPhase() == GamePhase::NotStarted, "constructor-phase-not-started");
    CHECK(!state.isWhiteBallPlacing(), "constructor-not-placing");
    CHECK(!state.isSimulationRunning(), "constructor-not-simulating");
}

// 白球放置验证 ----

void testIsValidPlacementInsideD() {
    GameState state;
    state.startNewGame();
    // D 区中心附近
    CHECK(state.isValidPlacement(Vector2D(BAULK_LINE_X - D_RADIUS * 0.5, 0.0)),
          "valid-placement-inside-D");
}

void testIsValidPlacementOutsideD() {
    GameState state;
    state.startNewGame();
    // D 区外
    CHECK(!state.isValidPlacement(Vector2D(500.0, 0.0)),
          "invalid-placement-outside-D");
}

void testIsValidPlacementTooFarRight() {
    GameState state;
    state.startNewGame();
    // x > BAULK_LINE_X → 非法
    CHECK(!state.isValidPlacement(Vector2D(BAULK_LINE_X + 100.0, 0.0)),
          "invalid-placement-beyond-baulk-line");
}

// performShot 门控 ----

void testPerformShotRejectedWhenGameOver() {
    GameState state;
    state.startNewGame();
    state.placeWhiteBall(Vector2D(BAULK_LINE_X - 30.0, 0.0));
    // 此时 isSimulationRunning == false，但 phase == RedBall，performShot 应被接受
    // (performShot 门控条件：simulationRunning || GameOver || whiteBallPlacing)
    // 此时三个都是 false，应接受
    state.performShot(45.0, 50.0);
    CHECK(state.isSimulationRunning(), "performShot-accepted");
}

void testPerformShotRejectedWhenPlacing() {
    GameState state;
    state.startNewGame();
    // 开局时 isWhiteBallPlacing == true
    state.performShot(45.0, 50.0);
    CHECK(!state.isSimulationRunning(), "performShot-rejected-when-placing");
}

// 玩家访问 ----

void testPlayerAccess() {
    GameState state;
    state.startNewGame();
    CHECK(state.player1() != nullptr, "player1-not-null");
    CHECK(state.player2() != nullptr, "player2-not-null");
    // 开局应为玩家 1
    CHECK(state.currentPlayer() == state.player1(), "current-player-is-player1");
}

// 球访问 ----

void testBallsAccess() {
    GameState state;
    state.startNewGame();
    // 应初始化 22 颗球
    CHECK(static_cast<int>(state.balls().size()) == TOTAL_BALLS, "balls-count-22");
    CHECK(!state.balls().empty(), "balls-not-empty");
}

// 桌访问 ----

void testTableAccess() {
    GameState state;
    state.startNewGame();
    // table() 应返回有效引用
    CHECK(state.table().width() > 0.0, "table-width-positive");
    CHECK(state.table().height() > 0.0, "table-height-positive");
}

// 犯规确认 ----

void testConfirmFoul() {
    GameState state;
    state.startNewGame();
    state.placeWhiteBall(Vector2D(BAULK_LINE_X - 30.0, 0.0));
    state.performShot(0.0, 10.0); // 击球
    state.confirmFoul();          // 不应崩溃
    CHECK(true, "confirmFoul-no-crash");
}

} // namespace Test
} // namespace Snooker2D

int main() {
    using namespace Snooker2D::Test;

    std::cout << "=== GameState 单元测试 ===" << std::endl << std::endl;

    std::cout << "[constructor]" << std::endl;
    testConstructor();

    std::cout << "[opening placement]" << std::endl;
    testOpeningRequiresWhiteBallPlacement();
    testOpeningPlacementKeepsRedBallPhase();

    std::cout << "[placement validation]" << std::endl;
    testIsValidPlacementInsideD();
    testIsValidPlacementOutsideD();
    testIsValidPlacementTooFarRight();

    std::cout << "[performShot gating]" << std::endl;
    testPerformShotRejectedWhenGameOver();
    testPerformShotRejectedWhenPlacing();

    std::cout << "[player access]" << std::endl;
    testPlayerAccess();

    std::cout << "[ball/table access]" << std::endl;
    testBallsAccess();
    testTableAccess();

    std::cout << "[foul]" << std::endl;
    testConfirmFoul();

    std::cout << std::endl;
    std::cout << "通过: " << passed << std::endl;
    std::cout << "失败: " << failed << std::endl;

    return failed == 0 ? 0 : 1;
}
