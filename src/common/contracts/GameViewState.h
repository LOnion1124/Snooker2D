#pragma once

#include <QString>
#include <QVector>

namespace Snooker2D {

// ---------------------------------------------------------------------------
// 单个球的展示状态
// ---------------------------------------------------------------------------
struct BallViewState {
    double x = 0.0;
    double y = 0.0;
    int type = 0;          // 对应 BallType 枚举值
    bool pocketed = false;
    bool onTable = false;
};

// ---------------------------------------------------------------------------
// 球桌整体展示状态（供 GameView 绘制）
// ---------------------------------------------------------------------------
struct TableViewState {
    QVector<BallViewState> balls;
    double cueAngle = 0.0;
    double cuePower = 50.0;
    bool centeredCoordinates = false; // true 表示以球桌中心为原点
    bool canAim = false;
    bool canShoot = false;
    bool isPlacingWhiteBall = false;
    bool isSimulating = false;
};

// ---------------------------------------------------------------------------
// 球杆控制面板展示状态（供 CueControl 显示）
// ---------------------------------------------------------------------------
struct CueViewState {
    double angle = 0.0;
    double power = 50.0;
    double englishX = 0.0;
    double englishY = 0.0;
};

// ---------------------------------------------------------------------------
// 计分板展示状态（供 ScoreBoard 显示）
// ---------------------------------------------------------------------------
struct ScoreViewState {
    int player1Score = 0;
    int player2Score = 0;
    int player1Break = 0;
    int player2Break = 0;
    QString foulMessage;
    QString statusMessage;
};

// ---------------------------------------------------------------------------
// 游戏信息面板展示状态（供 GameInfoPanel 显示）
// ---------------------------------------------------------------------------
struct GameInfoViewState {
    int currentPlayer = 0; // 1 或 2
    QString phaseText;     // 展示用文本，不应作为业务判断依据
    int phaseKind = 0;     // 对应 GamePhase 枚举值，供 View 做样式分支
    QString message;       // 犯规提示 / 比赛结束等
    bool showWhiteBallPlacementHint = false;
};

} // namespace Snooker2D
