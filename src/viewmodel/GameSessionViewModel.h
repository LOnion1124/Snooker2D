#pragma once

#include <QObject>
#include <QTimer>
#include "contracts/GameViewState.h"
#include "../common/Types.h"

namespace Snooker2D {

class GameState;

class GameSessionViewModel : public QObject {
    Q_OBJECT

public:
    explicit GameSessionViewModel(QObject* parent = nullptr);
    ~GameSessionViewModel() override;

    void start();

public slots:
    // View 命令
    void setAngle(double angle);
    void setPower(double power);
    void setEnglish(double englishX, double englishY);
    void onShotAnimationFinished();
    void placeWhiteBall(double x, double y);
    void restart();

signals:
    // ViewModel → View
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

    GameState* m_gameState = nullptr;
    QTimer* m_simulationTimer = nullptr;

    double m_cueAngle  = 0.0;
    double m_cuePower  = 50.0;
    double m_englishX  = 0.0;
    double m_englishY  = 0.0;

    QString m_statusMessage;
    FoulType m_foulType = FoulType::None;
    int     m_foulPenaltyPoints = 0;
    int     m_foulingPlayer = 0;
};

} // namespace Snooker2D
