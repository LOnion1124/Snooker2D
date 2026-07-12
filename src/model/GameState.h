#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QObject>
#include <vector>
#include <memory>
#include "../common/Types.h"

namespace Snooker2D {

class Ball;
class Table;
class Player;
class Physics;
class Rules;

class GameState : public QObject {
    Q_OBJECT

public:
    explicit GameState(QObject* parent = nullptr);
    ~GameState() override;

    // 游戏生命周期
    void startNewGame();
    void resetGame();

    // 击球流程
    void performShot(double angle, double power);
    bool isSimulationRunning() const { return m_simulationRunning; }
    void updateSimulation(); // 每帧调用

    // 状态查询
    GamePhase currentPhase() const { return m_phase; }
    Player* currentPlayer() const;
    Player* player1() const { return m_player1.get(); }
    Player* player2() const { return m_player2.get(); }
    const std::vector<std::unique_ptr<Ball>>& balls() const { return m_balls; }
    const Table& table() const { return *m_table; }

    // 白球放置（白球落袋后）
    bool isWhiteBallPlacing() const { return m_whiteBallPlacing; }
    bool isValidPlacement(Vector2D pos) const;
    void placeWhiteBall(Vector2D pos);

    // 犯规确认与自由球
    void confirmFoul();
    void selectFreeBall(BallType color);

signals:
    void phaseChanged(GamePhase newPhase);
    void turnChanged(Player* newPlayer);
    void simulationStarted();
    void simulationFinished();
    void foulOccurred(const struct FoulResult& result);
    void gameOver(Player* winner);
    void whiteBallPlacingStarted();
    void whiteBallPlaced();

private:
    void initBalls();
    void switchTurn();
    void checkGameOver();
    bool allRedsPocketed() const;
    void handlePostShot();

    // 状态
    GamePhase m_phase = GamePhase::NotStarted;
    PlayerId m_currentPlayerId = PlayerId::Player1;
    bool m_simulationRunning = false;

    // 击球跟踪
    BallType m_firstHitBall = BallType::White;
    bool m_anyBallHitCushion = false;
    bool m_whitePocketed = false;
    bool m_whiteBallPlacing = false;
    std::vector<bool> m_preShotPocketed; // 击球前各球是否已落袋

    // 核心组件
    std::unique_ptr<Table> m_table;
    std::unique_ptr<Physics> m_physics;
    std::unique_ptr<Rules> m_rules;
    std::unique_ptr<Player> m_player1;
    std::unique_ptr<Player> m_player2;
    std::vector<std::unique_ptr<Ball>> m_balls; // 拥有所有球
};

} // namespace Snooker2D

#endif // GAMESTATE_H
