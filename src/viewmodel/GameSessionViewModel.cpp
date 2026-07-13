#include "GameSessionViewModel.h"
#include "GameViewModel.h"
#include "CueControlViewModel.h"
#include "ScoreViewModel.h"
#include "../model/GameState.h"
#include "../model/Player.h"
#include "../model/Ball.h"
#include "../model/Rules.h"
#include "../common/Types.h"

namespace Snooker2D {

GameSessionViewModel::GameSessionViewModel(QObject* parent)
    : QObject(parent)
{
    // 创建 Bus
    m_bus = new GameUiBus(this);

    // 创建 Model
    m_gameState = new GameState(this);

    // 创建子 ViewModel
    m_gameViewModel = new GameViewModel(this);
    m_gameViewModel->setGameState(m_gameState);

    m_cueViewModel = new CueControlViewModel(this);
    m_scoreViewModel = new ScoreViewModel(this);

    // View 请求 → 内部处理
    connect(m_bus, &GameUiBus::cueAngleRequested,
            this, &GameSessionViewModel::onCueAngleRequested);
    connect(m_bus, &GameUiBus::cuePowerRequested,
            this, &GameSessionViewModel::onCuePowerRequested);
    connect(m_bus, &GameUiBus::shotAnimationFinished,
            this, &GameSessionViewModel::onShotAnimationFinished);
    connect(m_bus, &GameUiBus::whiteBallPlacementRequested,
            this, &GameSessionViewModel::onWhiteBallPlacementRequested);
    connect(m_bus, &GameUiBus::restartRequested,
            this, &GameSessionViewModel::onRestartRequested);

    // Model 信号 → 状态推送
    connect(m_gameState, &GameState::phaseChanged,
            this, &GameSessionViewModel::onModelPhaseChanged);
    connect(m_gameState, &GameState::turnChanged,
            this, &GameSessionViewModel::onModelTurnChanged);
    connect(m_gameState, &GameState::simulationStarted,
            this, &GameSessionViewModel::onModelSimulationStarted);
    connect(m_gameState, &GameState::simulationFinished,
            this, &GameSessionViewModel::onModelSimulationFinished);
    connect(m_gameState, &GameState::foulOccurred,
            this, &GameSessionViewModel::onModelFoulOccurred);
    connect(m_gameState, &GameState::whiteBallPlacingStarted,
            this, &GameSessionViewModel::onModelWhiteBallPlacingStarted);
    connect(m_gameState, &GameState::whiteBallPlaced,
            this, &GameSessionViewModel::onModelWhiteBallPlaced);
    connect(m_gameState->player1(), &Player::scoreChanged,
            this, &GameSessionViewModel::onPlayerScoreChanged);
    connect(m_gameState->player2(), &Player::scoreChanged,
            this, &GameSessionViewModel::onPlayerScoreChanged);

    // 模拟逐帧刷新
    connect(m_gameViewModel, &GameViewModel::ballPositionsChanged,
            this, &GameSessionViewModel::pushTableState);

    // 子 VM 角度/力度变化 → 推送 Bus
    connect(m_cueViewModel, &CueControlViewModel::angleChanged,
            this, [this](double) { pushCueState(); pushTableState(); });
    connect(m_cueViewModel, &CueControlViewModel::powerChanged,
            this, [this](double) { pushCueState(); pushTableState(); });

    // 分数子 VM 变化 → 推送计分状态
    connect(m_scoreViewModel, &ScoreViewModel::player1ScoreChanged,
            this, &GameSessionViewModel::pushScoreState);
    connect(m_scoreViewModel, &ScoreViewModel::player2ScoreChanged,
            this, &GameSessionViewModel::pushScoreState);
    connect(m_scoreViewModel, &ScoreViewModel::foulMessageChanged,
            this, &GameSessionViewModel::pushScoreState);
    connect(m_scoreViewModel, &ScoreViewModel::statusMessageChanged,
            this, &GameSessionViewModel::pushScoreState);
}

GameSessionViewModel::~GameSessionViewModel() = default;

void GameSessionViewModel::start() {
    pushAllStates();
    m_gameState->startNewGame();
}

void GameSessionViewModel::onCueAngleRequested(double angle) {
    m_cueViewModel->setAngle(angle);
    m_gameViewModel->setAngle(angle);
}

void GameSessionViewModel::onCuePowerRequested(double power) {
    m_cueViewModel->setPower(power);
    m_gameViewModel->setPower(power);
}

void GameSessionViewModel::onShotAnimationFinished() {
    m_gameViewModel->shoot();
}

void GameSessionViewModel::onWhiteBallPlacementRequested(double x, double y) {
    m_gameViewModel->confirmWhiteBallPlacement(x, y);
}

void GameSessionViewModel::onRestartRequested() {
    m_gameViewModel->restartGame();
}

void GameSessionViewModel::onModelPhaseChanged(GamePhase /*phase*/) {
    pushAllStates();
}

void GameSessionViewModel::onModelTurnChanged() {
    pushGameInfoState();
}

void GameSessionViewModel::onModelSimulationStarted() {
    pushAllStates();
}

void GameSessionViewModel::onModelSimulationFinished() {
    pushAllStates();
}

void GameSessionViewModel::onModelFoulOccurred(const FoulResult& result) {
    m_scoreViewModel->setFoulMessage(result.description);
    pushScoreState();
    pushGameInfoState();
}

void GameSessionViewModel::onModelWhiteBallPlacingStarted() {
    pushAllStates();
}

void GameSessionViewModel::onModelWhiteBallPlaced() {
    pushAllStates();
}

void GameSessionViewModel::onPlayerScoreChanged(int /*score*/) {
    if (m_gameState) {
        m_scoreViewModel->setPlayer1Score(m_gameState->player1()->score());
        m_scoreViewModel->setPlayer2Score(m_gameState->player2()->score());
    }
}

void GameSessionViewModel::pushTableState() {
    TableViewState state;

    if (m_gameViewModel) {
        const QVariantList positions = m_gameViewModel->ballPositions();
        for (const QVariant& item : positions) {
            const QVariantMap ballData = item.toMap();
            BallViewState bvs;
            bvs.x = ballData.value("x").toDouble();
            bvs.y = ballData.value("y").toDouble();
            bvs.type = ballData.value("type").toInt();
            bvs.pocketed = ballData.value("pocketed").toBool();
            bvs.onTable = ballData.value("onTable").toBool();
            state.balls.append(bvs);
        }
    }

    if (m_cueViewModel) {
        state.cueAngle = m_cueViewModel->angle();
        state.cuePower = m_cueViewModel->power();
    }

    // 坐标系检测
    state.centeredCoordinates = false;
    for (const BallViewState& bvs : state.balls) {
        if (bvs.x < 0.0 || bvs.y < 0.0 ||
            bvs.x > TABLE_WIDTH || bvs.y > TABLE_HEIGHT) {
            state.centeredCoordinates = true;
            break;
        }
    }

    if (m_gameState) {
        const GamePhase phase = m_gameState->currentPhase();
        const bool isWhiteBallPlacing = m_gameState->isWhiteBallPlacing();
        const bool isSimulating = m_gameState->isSimulationRunning();

        state.isPlacingWhiteBall = isWhiteBallPlacing;
        state.isSimulating = isSimulating;

        state.canAim = !isWhiteBallPlacing
                    && !isSimulating
                    && phase != GamePhase::NotStarted
                    && phase != GamePhase::GameOver;

        bool whiteBallOnTable = false;
        for (const BallViewState& bvs : state.balls) {
            if (bvs.type == 0 && bvs.onTable) {
                whiteBallOnTable = true;
                break;
            }
        }
        state.canShoot = state.canAim && whiteBallOnTable;
    }

    emit m_bus->tableStateChanged(state);
}

void GameSessionViewModel::pushCueState() {
    CueViewState state;
    if (m_cueViewModel) {
        state.angle = m_cueViewModel->angle();
        state.power = m_cueViewModel->power();
        state.englishX = m_cueViewModel->englishX();
        state.englishY = m_cueViewModel->englishY();
    }
    emit m_bus->cueStateChanged(state);
}

void GameSessionViewModel::pushScoreState() {
    ScoreViewState state;
    if (m_scoreViewModel) {
        state.player1Score = m_scoreViewModel->player1Score();
        state.player2Score = m_scoreViewModel->player2Score();
        state.player1Break = m_scoreViewModel->player1Break();
        state.player2Break = m_scoreViewModel->player2Break();
        state.foulMessage = m_scoreViewModel->foulMessage();
        state.statusMessage = m_scoreViewModel->statusMessage();
    }
    emit m_bus->scoreStateChanged(state);
}

void GameSessionViewModel::pushGameInfoState() {
    GameInfoViewState state;
    if (m_gameState) {
        const Player* cp = m_gameState->currentPlayer();
        state.currentPlayer = (cp == m_gameState->player1()) ? 1 : 2;
        state.phaseKind = static_cast<int>(m_gameState->currentPhase());
        state.phaseText = phaseTextFromEnum(m_gameState->currentPhase());
        state.showWhiteBallPlacementHint = m_gameState->isWhiteBallPlacing();
    }
    emit m_bus->gameInfoStateChanged(state);
}

void GameSessionViewModel::pushAllStates() {
    pushTableState();
    pushCueState();
    pushScoreState();
    pushGameInfoState();
}

QString GameSessionViewModel::phaseTextFromEnum(GamePhase phase) const {
    switch (phase) {
        case GamePhase::NotStarted: return QStringLiteral("未开始");
        case GamePhase::RedBall:    return QStringLiteral("请击红球");
        case GamePhase::ColorBall:  return QStringLiteral("请击彩球");
        case GamePhase::FreeBall:   return QStringLiteral("自由球");
        case GamePhase::Foul:       return QStringLiteral("犯规");
        case GamePhase::GameOver:   return QStringLiteral("比赛结束");
    }
    return QString();
}

} // namespace Snooker2D
