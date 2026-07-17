#include "GameSessionViewModel.h"
#include "../model/GameState.h"
#include "../model/Player.h"
#include "../model/Ball.h"
#include "../model/Rules.h"
#include "../common/Types.h"

namespace Snooker2D {

GameSessionViewModel::GameSessionViewModel(QObject* parent)
    : QObject(parent)
{
    m_gameState = new GameState(this);

    m_simulationTimer = new QTimer(this);
    m_simulationTimer->setInterval(1000 / 60);
    connect(m_simulationTimer, &QTimer::timeout,
            this, &GameSessionViewModel::onSimulationTick);

    // Model 信号 → 内部处理
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
}

GameSessionViewModel::~GameSessionViewModel() = default;

void GameSessionViewModel::start() {
    pushAllStates();
    m_gameState->startNewGame();
}

// ---- View 命令 ----

void GameSessionViewModel::setAngle(double angle) {
    while (angle < 0.0) angle += 360.0;
    while (angle >= 360.0) angle -= 360.0;
    if (m_cueAngle != angle) {
        m_cueAngle = angle;
        pushCueState();
        pushTableState();
    }
}

void GameSessionViewModel::setPower(double power) {
    if (power < 0.0) power = 0.0;
    if (power > 100.0) power = 100.0;
    if (m_cuePower != power) {
        m_cuePower = power;
        pushCueState();
        pushTableState();
    }
}

void GameSessionViewModel::setEnglish(double englishX, double englishY) {
    if (englishX < -1.0) englishX = -1.0;
    if (englishX > 1.0) englishX = 1.0;
    if (englishY < -1.0) englishY = -1.0;
    if (englishY > 1.0) englishY = 1.0;

    if (m_englishX != englishX || m_englishY != englishY) {
        m_englishX = englishX;
        m_englishY = englishY;
        pushCueState();
        pushTableState();
    }
}

void GameSessionViewModel::onShotAnimationFinished() {
    if (m_gameState) {
        m_gameState->performShot(m_cueAngle, m_cuePower);
    }
}

void GameSessionViewModel::placeWhiteBall(double x, double y) {
    if (m_gameState && m_gameState->isWhiteBallPlacing()) {
        m_gameState->placeWhiteBall(Vector2D(x, y));
    }
}

void GameSessionViewModel::restart() {
    if (!m_gameState) return;
    m_simulationTimer->stop();
    m_cueAngle = 0.0;
    m_cuePower = 50.0;
    m_englishX = 0.0;
    m_englishY = 0.0;
    m_foulType = FoulType::None;
    m_foulPenaltyPoints = 0;
    m_foulingPlayer = 0;
    m_gameState->startNewGame();
}

// ---- Model 信号回调 ----

void GameSessionViewModel::onModelPhaseChanged(GamePhase /*phase*/) {
    pushAllStates();
}

void GameSessionViewModel::onModelTurnChanged() {
    pushGameInfoState();
}

void GameSessionViewModel::onModelSimulationStarted() {
    m_simulationTimer->start();
    m_foulType = FoulType::None;
    m_foulPenaltyPoints = 0;
    m_foulingPlayer = 0;
    pushAllStates();
}

void GameSessionViewModel::onSimulationTick() {
    if (!m_gameState || !m_gameState->isSimulationRunning()) {
        m_simulationTimer->stop();
        return;
    }
    m_gameState->updateSimulation();
    pushTableState();
}

void GameSessionViewModel::onModelSimulationFinished() {
    m_simulationTimer->stop();
    pushAllStates();
}

void GameSessionViewModel::onModelFoulOccurred(const FoulResult& result) {
    m_foulType = result.type;
    m_foulPenaltyPoints = result.penaltyPoints;
    if (m_gameState && m_gameState->currentPlayer()) {
        m_foulingPlayer = (m_gameState->currentPlayer() == m_gameState->player1()) ? 1 : 2;
    }
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
    pushScoreState();
}

// ---- 状态推送 ----

void GameSessionViewModel::pushTableState() {
    TableViewState state;

    if (m_gameState) {
        for (const auto& ball : m_gameState->balls()) {
            BallViewState bvs;
            bvs.x = ball->position().x;
            bvs.y = ball->position().y;
            bvs.type = static_cast<int>(ball->type());
            bvs.pocketed = ball->isPocketed();
            bvs.onTable = ball->isOnTable();
            state.balls.append(bvs);
        }
    }

    state.cueAngle = m_cueAngle;
    state.cuePower = m_cuePower;
    state.cueEnglishX = m_englishX;
    state.cueEnglishY = m_englishY;

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

    emit tableStateReady(state);
}

void GameSessionViewModel::pushCueState() {
    CueViewState state;
    state.angle = m_cueAngle;
    state.power = m_cuePower;
    state.englishX = m_englishX;
    state.englishY = m_englishY;
    emit cueStateReady(state);
}

void GameSessionViewModel::pushScoreState() {
    ScoreViewState state;
    if (m_gameState) {
        state.player1Score = m_gameState->player1()->score();
        state.player2Score = m_gameState->player2()->score();
        state.player1Break = m_gameState->player1()->currentBreak();
        state.player2Break = m_gameState->player2()->currentBreak();
        state.foulingPlayer = m_foulingPlayer;
        state.foulType = static_cast<int>(m_foulType);
        state.foulPenaltyPoints = m_foulPenaltyPoints;
        state.statusMessage = m_statusMessage;
    }
    emit scoreStateReady(state);
}

void GameSessionViewModel::pushGameInfoState() {
    GameInfoViewState state;
    if (m_gameState) {
        const Player* cp = m_gameState->currentPlayer();
        state.currentPlayer = (cp == m_gameState->player1()) ? 1 : 2;
        state.phaseKind = static_cast<int>(m_gameState->currentPhase());
        state.isSimulating = m_gameState->isSimulationRunning();
        state.showWhiteBallPlacementHint = m_gameState->isWhiteBallPlacing();
    }
    emit gameInfoStateReady(state);
}

void GameSessionViewModel::pushAllStates() {
    pushTableState();
    pushCueState();
    pushScoreState();
    pushGameInfoState();
}

} // namespace Snooker2D
