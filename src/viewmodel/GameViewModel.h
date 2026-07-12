#ifndef GAMEVIEWMODEL_H
#define GAMEVIEWMODEL_H

#include <QObject>
#include <QTimer>
#include <QVariantList>
#include "../common/Types.h"

namespace Snooker2D {

class GameState;

class GameViewModel : public QObject {
    Q_OBJECT

    // 暴露给 View 的属性
    Q_PROPERTY(QVariantList ballPositions READ ballPositions NOTIFY ballPositionsChanged)
    Q_PROPERTY(int currentPlayer READ currentPlayer NOTIFY currentPlayerChanged)
    Q_PROPERTY(QString gamePhase READ gamePhase NOTIFY gamePhaseChanged)
    Q_PROPERTY(int player1Score READ player1Score NOTIFY player1ScoreChanged)
    Q_PROPERTY(int player2Score READ player2Score NOTIFY player2ScoreChanged)
    Q_PROPERTY(double cueAngle READ cueAngle NOTIFY cueAngleChanged)
    Q_PROPERTY(double cuePower READ cuePower NOTIFY cuePowerChanged)
    Q_PROPERTY(bool isPlacingWhiteBall READ isPlacingWhiteBall NOTIFY whiteBallPlacingChanged)

public:
    explicit GameViewModel(QObject* parent = nullptr);
    ~GameViewModel() override = default;

    // 绑定 Model
    void setGameState(GameState* gameState);

    // 属性访问器
    QVariantList ballPositions() const { return m_ballPositions; }
    int currentPlayer() const { return m_currentPlayer; }
    QString gamePhase() const { return m_gamePhase; }
    int player1Score() const { return m_player1Score; }
    int player2Score() const { return m_player2Score; }
    double cueAngle() const { return m_cueAngle; }
    double cuePower() const { return m_cuePower; }
    bool isPlacingWhiteBall() const { return m_isPlacingWhiteBall; }

    // 命令（Q_INVOKABLE 供 QML / 信号绑定使用）
    Q_INVOKABLE void shoot();
    Q_INVOKABLE void setAngle(double angle);
    Q_INVOKABLE void setPower(double power);
    Q_INVOKABLE void confirmFoul();
    Q_INVOKABLE void selectFreeBall(int ballTypeInt);
    Q_INVOKABLE void confirmWhiteBallPlacement(double x, double y);

signals:
    void ballPositionsChanged();
    void currentPlayerChanged();
    void gamePhaseChanged();
    void player1ScoreChanged();
    void player2ScoreChanged();
    void cueAngleChanged();
    void cuePowerChanged();

    void foulOccurred(const QString& description);
    void gameOver(int winnerPlayer);
    void whiteBallPlacingChanged();

private slots:
    void onModelPhaseChanged(GamePhase phase);
    void onModelTurnChanged();
    void onModelSimulationStarted();
    void onModelSimulationFinished();
    void onSimulationTick();
    void onModelFoulOccurred(const struct FoulResult& result);
    void onPlayerScoreChanged(int score);
    void onModelWhiteBallPlacingStarted();
    void onModelWhiteBallPlaced();

private:
    void refreshBallPositions();
    void refreshScores();

    GameState* m_gameState = nullptr;
    QTimer* m_simulationTimer = nullptr;

    // 属性缓存
    QVariantList m_ballPositions;
    int m_currentPlayer = 0;
    QString m_gamePhase;
    QString m_baseGamePhase; // 模拟中追加提示时保存基准阶段文本
    int m_player1Score = 0;
    int m_player2Score = 0;
    double m_cueAngle = 0.0;
    double m_cuePower = 50.0;
    bool m_isPlacingWhiteBall = false;
};

} // namespace Snooker2D

#endif // GAMEVIEWMODEL_H
