#include "../src/model/Rules.h"
#include "../src/model/Ball.h"
#include "../src/common/Constants.h"
#include "../src/common/Types.h"

#include "test_utils.h"

#include <iostream>
#include <vector>

namespace Snooker2D {
namespace Test {

void testRequiredBallType() {
    CHECK(Rules::requiredBallType(true, false) == BallType::Red,
          "requiredBallType-reds-remaining");
    CHECK(Rules::requiredBallType(false, false) == BallType::Yellow,
          "requiredBallType-no-reds");
    CHECK(Rules::requiredBallType(true, true) == BallType::Red,
          "requiredBallType-color-nomination");
}

void testCalculatePenalty() {
    // 白球最低罚 4 分
    CHECK(Rules::calculatePenalty(FoulType::WhitePocketed, BallType::White) == 4,
          "penalty-white-min-4");
    // 红球罚 4 分（低于最低罚分，取最低）
    CHECK(Rules::calculatePenalty(FoulType::WrongBallFirst, BallType::Red) == 4,
          "penalty-red-min-4");
    // 各彩球罚分
    CHECK(Rules::calculatePenalty(FoulType::MissedAll, BallType::Yellow) == 4,
          "penalty-yellow-4");
    CHECK(Rules::calculatePenalty(FoulType::MissedAll, BallType::Green) == 4,
          "penalty-green-4");
    CHECK(Rules::calculatePenalty(FoulType::MissedAll, BallType::Brown) == 4,
          "penalty-brown-4");
    CHECK(Rules::calculatePenalty(FoulType::MissedAll, BallType::Blue) == 5,
          "penalty-blue-5");
    CHECK(Rules::calculatePenalty(FoulType::MissedAll, BallType::Pink) == 6,
          "penalty-pink-6");
    CHECK(Rules::calculatePenalty(FoulType::MissedAll, BallType::Black) == 7,
          "penalty-black-7");
}

void testHasValidTarget() {
    Ball red(BallType::Red);
    Ball black(BallType::Black);
    std::vector<Ball*> balls{&red, &black};

    CHECK(Rules::hasValidTarget(balls, BallType::Red),
          "hasValidTarget-red-found");
    CHECK(Rules::hasValidTarget(balls, BallType::Black),
          "hasValidTarget-black-found");
    CHECK(!Rules::hasValidTarget(balls, BallType::Yellow),
          "hasValidTarget-yellow-not-found");

    // 红球落袋
    red.setPocketed(true);
    CHECK(!Rules::hasValidTarget(balls, BallType::Red),
          "hasValidTarget-red-all-pocketed");
}

void testCheckFoulWhitePocketed() {
    Ball white(BallType::White);
    Ball red(BallType::Red);
    std::vector<Ball*> balls{&white, &red};
    Rules rules;

    FoulResult r = rules.checkFoul(balls, &white, BallType::Red, true, true, BallType::Red);
    CHECK(r.isFoul, "foul-white-pocketed-flag");
    CHECK(r.type == FoulType::WhitePocketed, "foul-white-pocketed-type");
    CHECK(r.penaltyPoints == 4, "foul-white-pocketed-penalty");
}

void testCheckFoulMissedAll() {
    Ball white(BallType::White);
    Ball red(BallType::Red);
    std::vector<Ball*> balls{&white, &red};
    Rules rules;

    FoulResult r = rules.checkFoul(balls, &white, BallType::White, true, false, BallType::Red);
    CHECK(r.isFoul, "foul-missed-all-flag");
    CHECK(r.type == FoulType::MissedAll, "foul-missed-all-type");
}

void testCheckFoulWrongBallFirst() {
    Ball white(BallType::White);
    Ball red(BallType::Red);
    Ball black(BallType::Black);
    std::vector<Ball*> balls{&white, &red, &black};
    Rules rules;

    // 应打红球，先击中黑球 → 犯规
    FoulResult r = rules.checkFoul(balls, &white, BallType::Black, true, false, BallType::Red);
    CHECK(r.isFoul, "foul-wrong-ball-first-flag");
    CHECK(r.type == FoulType::WrongBallFirst, "foul-wrong-ball-first-type");
}

void testCheckFoulNoBallHitCushion() {
    Ball white(BallType::White);
    Ball red(BallType::Red);
    std::vector<Ball*> balls{&white, &red};
    Rules rules;

    // 击中红球了但无碰库 → 犯规
    FoulResult r = rules.checkFoul(balls, &white, BallType::Red, false, false, BallType::Red);
    CHECK(r.isFoul, "foul-no-cushion-flag");
    CHECK(r.type == FoulType::NoBallHitCushion, "foul-no-cushion-type");
}

void testCheckFoulNoFoul() {
    Ball white(BallType::White);
    Ball red(BallType::Red);
    std::vector<Ball*> balls{&white, &red};
    Rules rules;

    // 合法击球：击中红球、有碰库、白球未落袋
    FoulResult r = rules.checkFoul(balls, &white, BallType::Red, true, false, BallType::Red);
    CHECK(!r.isFoul, "valid-shot-no-foul");
    CHECK(r.type == FoulType::None, "valid-shot-type-none");
    CHECK(r.penaltyPoints == 0, "valid-shot-penalty-zero");
}

} // namespace Test
} // namespace Snooker2D

int main() {
    using namespace Snooker2D::Test;

    std::cout << "=== Rules 单元测试 ===" << std::endl << std::endl;

    std::cout << "[requiredBallType]" << std::endl;
    testRequiredBallType();

    std::cout << "[calculatePenalty]" << std::endl;
    testCalculatePenalty();

    std::cout << "[hasValidTarget]" << std::endl;
    testHasValidTarget();

    std::cout << "[checkFoul]" << std::endl;
    testCheckFoulWhitePocketed();
    testCheckFoulMissedAll();
    testCheckFoulWrongBallFirst();
    testCheckFoulNoBallHitCushion();
    testCheckFoulNoFoul();

    std::cout << std::endl;
    std::cout << "通过: " << passed << std::endl;
    std::cout << "失败: " << failed << std::endl;

    return failed == 0 ? 0 : 1;
}
