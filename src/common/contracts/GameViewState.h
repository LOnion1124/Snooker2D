#pragma once

#include <QString>
#include <QVector>

namespace Snooker2D {

// 单球展示状态
struct BallViewState {
    double x = 0.0;
    double y = 0.0;
    int    type = 0;
    bool   pocketed = false;
    bool   onTable  = false;
};

// 球桌整体展示状态（供 GameView 绘制）
struct TableViewState {
    QVector<BallViewState> balls;
    double cueAngle   = 0.0;
    double cuePower   = 50.0;
    bool   centeredCoordinates = false;
    bool   canAim     = false;
    bool   canShoot   = false;
    bool   isPlacingWhiteBall = false;
    bool   isSimulating = false;
};

// 球杆控制面板展示状态
struct CueViewState {
    double angle    = 0.0;
    double power    = 50.0;
    double englishX = 0.0;
    double englishY = 0.0;
};

// 计分板展示状态
struct ScoreViewState {
    int     player1Score = 0;
    int     player2Score = 0;
    int     player1Break = 0;
    int     player2Break = 0;
    int     foulingPlayer = 0;  // 犯规者编号: 0=无, 1=玩家1, 2=玩家2
    QString foulMessage;
    QString statusMessage;
};

// 游戏信息面板展示状态
// phaseKind 对应 GamePhase 枚举，供 View 做样式分支，避免解析中文
struct GameInfoViewState {
    int     currentPlayer = 0;
    QString phaseText;
    int     phaseKind = 0;
    QString message;
    bool    showWhiteBallPlacementHint = false;
};

} // namespace Snooker2D
