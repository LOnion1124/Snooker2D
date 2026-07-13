#pragma once

#include <QObject>
#include <QTimer>
#include "contracts/GameUiBus.h"
#include "contracts/GameViewState.h"
#include "../common/Types.h"

namespace Snooker2D {

class GameState;

// ViewModel 层协调器
// 持有 Bus、Model，直接管理游戏状态转换和 UI 状态推送
// 不依赖中间子 ViewModel
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

    // 定时器
    void onSimulationTick();

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

    // 物理模拟定时器
    QTimer* m_simulationTimer = nullptr;

    // 击球控制状态（原 CueControlViewModel）
    double m_cueAngle = 0.0;
    double m_cuePower = 50.0;
    double m_englishX = 0.0;
    double m_englishY = 0.0;

    // 计分板消息（原 ScoreViewModel）
    QString m_foulMessage;
    QString m_statusMessage;
};

} // namespace Snooker2D
