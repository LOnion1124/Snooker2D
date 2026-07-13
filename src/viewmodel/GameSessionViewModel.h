#ifndef GAMESESSIONVIEWMODEL_H
#define GAMESESSIONVIEWMODEL_H

#include <QObject>
#include "contracts/GameUiBus.h"
#include "contracts/GameViewState.h"
#include "../common/Types.h"

namespace Snooker2D {

class GameState;
class GameViewModel;
class CueControlViewModel;
class ScoreViewModel;

// ---------------------------------------------------------------------------
// GameSessionViewModel — ViewModel 层协调器
//
// 职责：
//   1. 持有 GameUiBus（View ↔ ViewModel 唯一通信通道）
//   2. 持有 GameState 及所有子 ViewModel
//   3. 统一向 Bus 推送 ViewState
//   4. 统一处理来自 Bus 的 View 请求
// ---------------------------------------------------------------------------
class GameSessionViewModel : public QObject {
    Q_OBJECT

public:
    explicit GameSessionViewModel(QObject* parent = nullptr);
    ~GameSessionViewModel() override;

    // 获取 Bus，供 App 层注入到 View
    GameUiBus* bus() const { return m_bus; }

    // 启动新游戏
    void start();

private slots:
    // Bus 请求处理（View → ViewModel）
    void onCueAngleRequested(double angle);
    void onCuePowerRequested(double power);
    void onShotAnimationFinished();
    void onWhiteBallPlacementRequested(double x, double y);
    void onRestartRequested();

    // Model 信号回调
    void onModelPhaseChanged(GamePhase phase);
    void onModelTurnChanged();
    void onModelSimulationStarted();
    void onModelSimulationFinished();
    void onModelFoulOccurred(const struct FoulResult& result);
    void onModelWhiteBallPlacingStarted();
    void onModelWhiteBallPlaced();
    void onPlayerScoreChanged(int score);

private:
    // 状态推送（ViewModel → View）
    void pushTableState();
    void pushCueState();
    void pushScoreState();
    void pushGameInfoState();
    void pushAllStates();

    QString phaseTextFromEnum(GamePhase phase) const;

    GameUiBus* m_bus = nullptr;

    // Model
    GameState* m_gameState = nullptr;

    // 子 ViewModel（保留现有实现，作为内部组件）
    GameViewModel* m_gameViewModel = nullptr;
    CueControlViewModel* m_cueViewModel = nullptr;
    ScoreViewModel* m_scoreViewModel = nullptr;
};

} // namespace Snooker2D

#endif // GAMESESSIONVIEWMODEL_H
