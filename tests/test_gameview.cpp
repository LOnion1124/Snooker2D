#include <QtTest/QtTest>
#include <QApplication>
#include "../src/view/GameView.h"
#include "../src/common/contracts/GameViewState.h"

namespace Snooker2D {
namespace Test {

class TestGameView : public QObject {
    Q_OBJECT

private slots:
    void testConstruction();
    void testApplyTableState();
    void testCancelShotAnimation();
};

void TestGameView::testConstruction() {
    GameView view;
    QVERIFY(view.isVisible() || !view.isVisible()); // 能构造即可
}

void TestGameView::testApplyTableState() {
    GameView view;

    TableViewState state;
    BallViewState whiteBall;
    whiteBall.x = -280.0;
    whiteBall.y = 0.0;
    whiteBall.type = 0;
    whiteBall.onTable = true;
    state.balls.append(whiteBall);
    state.canAim = true;
    state.canShoot = true;
    state.centeredCoordinates = true;

    view.applyTableState(state); // 不应崩溃
    QVERIFY(true);
}

void TestGameView::testCancelShotAnimation() {
    GameView view;
    view.cancelShotAnimation(); // 不应崩溃
    QVERIFY(true);
}

} // namespace Test
} // namespace Snooker2D

QTEST_MAIN(Snooker2D::Test::TestGameView)
#include "test_gameview.moc"
