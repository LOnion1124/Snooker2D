#include "../src/model/Player.h"

#include "test_utils.h"

#include <iostream>

namespace Snooker2D {
namespace Test {

void testConstruction() {
    Player p(QStringLiteral("玩家A"));
    CHECK(p.name() == QStringLiteral("玩家A"), "player-name-constructor");
    CHECK(p.score() == 0, "player-score-default-zero");
    CHECK(p.currentBreak() == 0, "player-break-default-zero");
}

void testSetName() {
    Player p(QStringLiteral("旧名"));
    bool emitted = false;
    QString emittedName;
    QObject::connect(&p, &Player::nameChanged,
        [&](const QString& n) { emitted = true; emittedName = n; });

    p.setName(QStringLiteral("新名"));
    CHECK(p.name() == QStringLiteral("新名"), "setName-updates");
    CHECK(emitted, "setName-emits-signal");
    CHECK(emittedName == QStringLiteral("新名"), "setName-signal-value");
}

void testAddScore() {
    Player p(QStringLiteral("玩家"));
    bool emitted = false;
    QObject::connect(&p, &Player::scoreChanged, [&]() { emitted = true; });

    p.addScore(5);
    CHECK(p.score() == 5, "addScore-updates-score");
    CHECK(p.currentBreak() == 5, "addScore-updates-break");
    CHECK(emitted, "addScore-emits-scoreChanged");

    p.addScore(3);
    CHECK(p.score() == 8, "addScore-accumulates-score");
    CHECK(p.currentBreak() == 8, "addScore-accumulates-break");
}

void testResetBreak() {
    Player p(QStringLiteral("玩家"));
    p.addScore(5);
    CHECK(p.currentBreak() == 5, "pre-resetBreak-break");

    p.resetBreak();
    CHECK(p.currentBreak() == 0, "resetBreak-zeroes-break");
    CHECK(p.score() == 5, "resetBreak-preserves-score");
}

void testResetScore() {
    Player p(QStringLiteral("玩家"));
    p.addScore(10);

    bool emitted = false;
    int emittedScore = -1;
    QObject::connect(&p, &Player::scoreChanged,
        [&](int s) { emitted = true; emittedScore = s; });

    p.resetScore();
    CHECK(p.score() == 0, "resetScore-zeroes-score");
    CHECK(p.currentBreak() == 0, "resetScore-zeroes-break");
    CHECK(emitted, "resetScore-emits-scoreChanged");
    CHECK(emittedScore == 0, "resetScore-signal-value-zero");
}

void testAddScoreAfterReset() {
    Player p(QStringLiteral("玩家"));
    p.addScore(5);
    p.resetBreak();
    p.addScore(3);
    CHECK(p.score() == 8, "after-resetBreak-score-accumulates");
    CHECK(p.currentBreak() == 3, "after-resetBreak-break-restarts");

    p.resetScore();
    p.addScore(7);
    CHECK(p.score() == 7, "after-resetScore-score-restarts");
    CHECK(p.currentBreak() == 7, "after-resetScore-break-restarts");
}

} // namespace Test
} // namespace Snooker2D

int main() {
    using namespace Snooker2D::Test;

    std::cout << "=== Player 单元测试 ===" << std::endl << std::endl;

    std::cout << "[construction/setName]" << std::endl;
    testConstruction();
    testSetName();

    std::cout << "[score/break]" << std::endl;
    testAddScore();
    testResetBreak();
    testResetScore();
    testAddScoreAfterReset();

    std::cout << std::endl;
    std::cout << "通过: " << passed << std::endl;
    std::cout << "失败: " << failed << std::endl;

    return failed == 0 ? 0 : 1;
}
