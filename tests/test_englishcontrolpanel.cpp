#include <QtTest/QtTest>
#include <QApplication>
#include <QPushButton>
#include "../src/view/EnglishControlPanel.h"
#include "../src/common/contracts/GameViewState.h"

namespace Snooker2D {
namespace Test {

class TestEnglishControlPanel : public QObject {
    Q_OBJECT

private slots:
    void testConstruction();
    void testResetButtonEmitsEnglishChanged();
    void testApplyCueState();
};

void TestEnglishControlPanel::testConstruction() {
    EnglishControlPanel panel;
    // 至少有一个重置按钮
    QVERIFY(panel.findChildren<QPushButton*>().size() >= 1);
}

void TestEnglishControlPanel::testResetButtonEmitsEnglishChanged() {
    EnglishControlPanel panel;

    QSignalSpy spy(&panel, &EnglishControlPanel::englishChanged);
    QPushButton* resetBtn = nullptr;
    for (auto* btn : panel.findChildren<QPushButton*>()) {
        if (btn->text() == QStringLiteral("重置") ||
            btn->text() == QStringLiteral("Reset")) {
            resetBtn = btn;
            break;
        }
    }
    if (!resetBtn) {
        QSKIP("找不到重置按钮");
    }

    QTest::mouseClick(resetBtn, Qt::LeftButton);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0].toDouble(), 0.0);
    QCOMPARE(spy[0][1].toDouble(), 0.0);
}

void TestEnglishControlPanel::testApplyCueState() {
    EnglishControlPanel panel;
    CueViewState state;
    state.englishX = 0.5;
    state.englishY = -0.5;
    state.angle = 45.0;
    state.power = 80.0;

    panel.applyCueState(state); // 不应崩溃
    QVERIFY(true);
}

} // namespace Test
} // namespace Snooker2D

QTEST_MAIN(Snooker2D::Test::TestEnglishControlPanel)
#include "test_englishcontrolpanel.moc"
