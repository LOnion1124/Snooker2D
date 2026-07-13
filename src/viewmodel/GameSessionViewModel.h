#pragma once

#include <QObject>
#include "contracts/GameUiBus.h"
#include "contracts/GameViewState.h"
#include "../common/Types.h"

namespace Snooker2D {

class GameState;
class GameViewModel;
class CueControlViewModel;
class ScoreViewModel;

// ViewModel 层协调器
// 持有 Bus、Model、所有子 ViewModel，统一处理 View 请求和状态推送
class GameSessionViewModel : public QObject {
    Q_OBJECT

public:
    explicit GameSessionViewModel(QObject* parent = nullptr);
    ~GameSessionViewModel() override;

    GameUiBus* bus() const { return m_bus; }
    void start();

private slots:
    // View 请求
    void onCueAngleRequested(double angle);
    void onCuePowerRequested(double power);
    void onShotAnimationFinished();
    void onWhiteBallPlacementRequested(double x, double y);
    void onRestartRequested();

    // Model 信号
    void onModelPhaseChanged(GamePhase phase);
    void onModelTurnChanged();
    void onModelSimulationStarted();
    void onModelSimulationFinished();
    void onModelFoulOccurred(const struct FoulResult& result);
    void onModelWhiteBallPlacingStarted();
    void onModelWhiteBallPlaced();
    void onPlayerScoreChanged(int score);

private:
    // 状态推送
    void pushTableState();
    void pushCueState();
    void pushScoreState();
    void pushGameInfoState();
    void pushAllStates();

    QString phaseTextFromEnum(GamePhase phase) const;

    GameUiBus* m_bus = nullptr;
    GameState* m_gameState = nullptr;
    GameViewModel* m_gameViewModel = nullptr;
    CueControlViewModel* m_cueViewModel = nullptr;
    ScoreViewModel* m_scoreViewModel = nullptr;
};

} // namespace Snooker2D
