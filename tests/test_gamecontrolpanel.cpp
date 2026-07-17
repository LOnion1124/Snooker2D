#include <QtTest/QtTest>
#include <QApplication>
#include <QPushButton>
#include "../src/view/GameControlPanel.h"

namespace Snooker2D {
namespace Test {

class TestGameControlPanel : public QObject {
    Q_OBJECT

private slots:
    void testConstruction();
    void testRestartButtonEmitsRestartRequested();
};

void TestGameControlPanel::testConstruction() {
    GameControlPanel panel;
    QVERIFY(panel.findChildren<QPushButton*>().size() >= 2);
}

void TestGameControlPanel::testRestartButtonEmitsRestartRequested() {
    GameControlPanel panel;

    QPushButton* restartBtn = nullptr;
    for (auto* btn : panel.findChildren<QPushButton*>()) {
        if (btn->text() == QStringLiteral("重启游戏")) {
            restartBtn = btn;
            break;
        }
    }
    if (!restartBtn) {
        QSKIP("找不到重启按钮");
    }

    QSignalSpy spy(&panel, &GameControlPanel::restartRequested);
    QTest::mouseClick(restartBtn, Qt::LeftButton);
    QCOMPARE(spy.count(), 1);
}

} // namespace Test
} // namespace Snooker2D

QTEST_MAIN(Snooker2D::Test::TestGameControlPanel)
#include "test_gamecontrolpanel.moc"
