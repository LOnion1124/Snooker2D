#pragma once

#include <QObject>
#include <QTimer>
#include "contracts/GameViewState.h"
#include "../common/Types.h"

namespace Snooker2D {

class GameState;

// ViewModel 层协调器。持有 Model，接收 View 命令，推送 ViewState
class GameSessionViewModel : public QObject {
    Q_OBJECT

public:
    explicit GameSessionViewModel(QObject* parent = nullptr);
    ~GameSessionViewModel() override;

    void start();

public slots:
    // View 命令（App 层 connect 到 View 的信号）
    void setAngle(double angle);
    void setPower(double power);
    void onShotAnimationFinished();
    void placeWhiteBall(double x, double y);
    void restart();

signals:
    // 状态推送（App 层 connect 到 View 的槽）
    void tableStateReady(const TableViewState& state);
    void cueStateReady(const CueViewState& state);
    void scoreStateReady(const ScoreViewState& state);
    void gameInfoStateReady(const GameInfoViewState& state);
    void shotAnimationCancelled();

private slots:
    void onModelPhaseChanged(GamePhase phase);
    void onModelTurnChanged();
    void onModelSimulationStarted();
    void onModelSimulationFinished();
    void onModelFoulOccurred(const struct FoulResult& result);
    void onModelWhiteBallPlacingStarted();
    void onModelWhiteBallPlaced();
    void onPlayerScoreChanged(int score);
    void onSimulationTick();

private:
    void pushTableState();
    void pushCueState();
    void pushScoreState();
    void pushGameInfoState();
    void pushAllStates();
    QString phaseTextFromEnum(GamePhase phase) const;

    GameState* m_gameState = nullptr;
    QTimer* m_simulationTimer = nullptr;

    // 击球控制状态
    double m_cueAngle  = 0.0;
    double m_cuePower  = 50.0;
    double m_englishX  = 0.0;
    double m_englishY  = 0.0;

    // 计分板消息
    QString m_foulMessage;
    QString m_statusMessage;
};

} // namespace Snooker2D
