#include <QtTest/QtTest>
#include "../src/viewmodel/GameSessionViewModel.h"
#include "../src/common/contracts/GameViewState.h"
#include "../src/common/Types.h"

namespace Snooker2D {
namespace Test {

class TestGameSessionViewModel : public QObject {
    Q_OBJECT

private slots:
    void testStartEmitsAllSignals();
    void testSetAngleNormalizes();
    void testSetPowerClips();
    void testSetEnglishClips();
    void testEnglishOnlyEmitsWhenChanged();
    void testRestartResetsState();
};

void TestGameSessionViewModel::testStartEmitsAllSignals() {
    GameSessionViewModel vm;

    QSignalSpy tableSpy(&vm, &GameSessionViewModel::tableStateReady);
    QSignalSpy cueSpy(&vm, &GameSessionViewModel::cueStateReady);
    QSignalSpy scoreSpy(&vm, &GameSessionViewModel::scoreStateReady);
    QSignalSpy gameInfoSpy(&vm, &GameSessionViewModel::gameInfoStateReady);

    vm.start();

    // start() 调用 pushAllStates → 每个信号至少发射一次
    QVERIFY(tableSpy.count() >= 1);
    QVERIFY(cueSpy.count() >= 1);
    QVERIFY(scoreSpy.count() >= 1);
    QVERIFY(gameInfoSpy.count() >= 1);

    // 核实 cueState 初始值
    auto cue = cueSpy[0][0].value<CueViewState>();
    QCOMPARE(cue.angle, 0.0);
    QCOMPARE(cue.power, 50.0);
    QCOMPARE(cue.englishX, 0.0);
    QCOMPARE(cue.englishY, 0.0);
}

void TestGameSessionViewModel::testSetAngleNormalizes() {
    GameSessionViewModel vm;

    QSignalSpy cueSpy(&vm, &GameSessionViewModel::cueStateReady);

    vm.setAngle(45.0);
    QCOMPARE(cueSpy.count(), 1);
    auto c1 = cueSpy[0][0].value<CueViewState>();
    QCOMPARE(c1.angle, 45.0);

    // 超过 360 → 应归一化
    vm.setAngle(420.0);
    auto c2 = cueSpy[1][0].value<CueViewState>();
    QCOMPARE(c2.angle, 60.0);

    // 负角度 → 应归一化
    vm.setAngle(-90.0);
    auto c3 = cueSpy[2][0].value<CueViewState>();
    QCOMPARE(c3.angle, 270.0);
}

void TestGameSessionViewModel::testSetPowerClips() {
    GameSessionViewModel vm;

    QSignalSpy cueSpy(&vm, &GameSessionViewModel::cueStateReady);

    vm.setPower(80.0);
    auto c1 = cueSpy[0][0].value<CueViewState>();
    QCOMPARE(c1.power, 80.0);

    // 超过 100 → 钳位
    vm.setPower(150.0);
    auto c2 = cueSpy[1][0].value<CueViewState>();
    QCOMPARE(c2.power, 100.0);

    // 负数 → 钳位
    vm.setPower(-20.0);
    auto c3 = cueSpy[2][0].value<CueViewState>();
    QCOMPARE(c3.power, 0.0);
}

void TestGameSessionViewModel::testSetEnglishClips() {
    GameSessionViewModel vm;

    QSignalSpy cueSpy(&vm, &GameSessionViewModel::cueStateReady);

    vm.setEnglish(0.5, -0.8);
    auto c1 = cueSpy[0][0].value<CueViewState>();
    QCOMPARE(c1.englishX, 0.5);
    QCOMPARE(c1.englishY, -0.8);

    // 超出范围 → 钳位
    vm.setEnglish(2.0, -2.0);
    auto c2 = cueSpy[1][0].value<CueViewState>();
    QCOMPARE(c2.englishX, 1.0);
    QCOMPARE(c2.englishY, -1.0);
}

void TestGameSessionViewModel::testEnglishOnlyEmitsWhenChanged() {
    GameSessionViewModel vm;

    QSignalSpy cueSpy(&vm, &GameSessionViewModel::cueStateReady);

    vm.setEnglish(0.3, 0.3);
    QCOMPARE(cueSpy.count(), 1);

    // 相同值不重复发射
    vm.setEnglish(0.3, 0.3);
    QCOMPARE(cueSpy.count(), 1);
}

void TestGameSessionViewModel::testRestartResetsState() {
    GameSessionViewModel vm;

    vm.setAngle(90.0);
    vm.setPower(100.0);
    vm.setEnglish(0.5, -0.5);

    QSignalSpy cueSpy(&vm, &GameSessionViewModel::cueStateReady);
    vm.restart();

    auto c = cueSpy[0][0].value<CueViewState>();
    QCOMPARE(c.angle, 0.0);
    QCOMPARE(c.power, 50.0);
    QCOMPARE(c.englishX, 0.0);
    QCOMPARE(c.englishY, 0.0);
}

} // namespace Test
} // namespace Snooker2D

QTEST_MAIN(Snooker2D::Test::TestGameSessionViewModel)
#include "test_viewmodel.moc"
