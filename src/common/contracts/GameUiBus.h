#pragma once

#include <QObject>
#include "GameViewState.h"

namespace Snooker2D {

// ---------------------------------------------------------------------------
// GameUiBus — View 与 ViewModel 之间的唯一通信契约
//
// View  只通过此 Bus 接收 ViewState 并发送请求信号
// ViewModel 只通过此 Bus 推送 ViewState 并处理请求信号
//
// 此 Bus 不含任何业务逻辑，仅仅是信号/槽契约载体。
// ---------------------------------------------------------------------------
class GameUiBus : public QObject {
    Q_OBJECT

public:
    explicit GameUiBus(QObject* parent = nullptr) : QObject(parent) {}

signals:
    // ===== ViewModel → View =====
    void tableStateChanged(const TableViewState& state);
    void cueStateChanged(const CueViewState& state);
    void scoreStateChanged(const ScoreViewState& state);
    void gameInfoStateChanged(const GameInfoViewState& state);
    void shotAnimationCancelled();

    // ===== View → ViewModel =====
    void cueAngleRequested(double angle);
    void cuePowerRequested(double power);
    void shotRequested();
    void shotAnimationFinished();
    void whiteBallPlacementRequested(double x, double y);
    void restartRequested();
};

} // namespace Snooker2D
