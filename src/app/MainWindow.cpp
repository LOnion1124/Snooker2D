#include "MainWindow.h"
#include "../app/AppConfig.h"

// Model
#include "../model/GameState.h"

// ViewModel
#include "../viewmodel/GameViewModel.h"
#include "../viewmodel/CueControlViewModel.h"
#include "../viewmodel/ScoreViewModel.h"

// View
#include "../view/GameView.h"
#include "../view/CueControl.h"
#include "../view/ScoreBoard.h"
#include "../view/GameInfoPanel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

namespace Snooker2D {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(APP_NAME);
    resize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

    setupUI();
    initGame();
    setupBindings();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QHBoxLayout(centralWidget);

    // 左侧：球桌 + 击球控制
    auto* leftLayout = new QVBoxLayout();
    m_gameView = new GameView(this);
    m_cueControl = new CueControl(this);
    leftLayout->addWidget(m_gameView);
    leftLayout->addWidget(m_cueControl);

    // 右侧：计分板 + 信息面板
    auto* rightLayout = new QVBoxLayout();
    m_scoreBoard = new ScoreBoard(this);
    m_gameInfoPanel = new GameInfoPanel(this);
    rightLayout->addWidget(m_scoreBoard);
    rightLayout->addWidget(m_gameInfoPanel);
    rightLayout->addStretch();

    mainLayout->addLayout(leftLayout, 3);
    mainLayout->addLayout(rightLayout, 1);

    setCentralWidget(centralWidget);
}

void MainWindow::initGame() {
    // 创建 Model
    m_gameState = new GameState(this);

    // 创建 ViewModel 并绑定 Model
    m_gameViewModel = new GameViewModel(this);
    m_gameViewModel->setGameState(m_gameState);

    m_cueViewModel = new CueControlViewModel(this);
    m_scoreViewModel = new ScoreViewModel(this);

    // ViewModel 注入 View
    m_gameView->setViewModel(m_gameViewModel);
    m_cueControl->setViewModel(m_cueViewModel);
    m_scoreBoard->setViewModel(m_scoreViewModel);

    // 开始游戏
    m_gameState->startNewGame();
}

void MainWindow::setupBindings() {
    // 击球请求：CueControlViewModel → GameViewModel
    connect(m_cueViewModel, &CueControlViewModel::shootRequested,
            this, [this]() {
        m_gameViewModel->setAngle(m_cueViewModel->angle());
        m_gameViewModel->setPower(m_cueViewModel->power());
        m_gameViewModel->shoot();
    });

    // 角度/力度实时同步 → GameViewModel（供瞄准线等用）
    connect(m_cueViewModel, &CueControlViewModel::angleChanged,
            m_gameViewModel, &GameViewModel::setAngle);
    connect(m_cueViewModel, &CueControlViewModel::powerChanged,
            m_gameViewModel, &GameViewModel::setPower);

    // 当前玩家 → GameInfoPanel
    connect(m_gameViewModel, &GameViewModel::currentPlayerChanged,
            this, [this]() {
        m_gameInfoPanel->setCurrentPlayer(m_gameViewModel->currentPlayer());
    });

    // 游戏阶段 → GameInfoPanel
    connect(m_gameViewModel, &GameViewModel::gamePhaseChanged,
            this, [this]() {
        m_gameInfoPanel->setPhase(m_gameViewModel->gamePhase());
    });

    // 分数同步：GameViewModel → ScoreViewModel
    connect(m_gameViewModel, &GameViewModel::player1ScoreChanged,
            this, [this]() {
        m_scoreViewModel->setPlayer1Score(m_gameViewModel->player1Score());
    });
    connect(m_gameViewModel, &GameViewModel::player2ScoreChanged,
            this, [this]() {
        m_scoreViewModel->setPlayer2Score(m_gameViewModel->player2Score());
    });

    // 犯规提示
    connect(m_gameViewModel, &GameViewModel::foulOccurred,
            this, [this](const QString& desc) {
        m_scoreViewModel->setFoulMessage(desc);
        m_gameInfoPanel->setMessage(desc);
    });

    // 比赛结束
    connect(m_gameViewModel, &GameViewModel::gameOver,
            this, [this](int winner) {
        QString msg = QString("玩家 %1 获胜！").arg(winner);
        m_scoreViewModel->setStatusMessage(msg);
        m_gameInfoPanel->setMessage(msg);
    });

    // 同步初始状态
    m_gameInfoPanel->setCurrentPlayer(m_gameViewModel->currentPlayer());
    m_gameInfoPanel->setPhase(m_gameViewModel->gamePhase());
}

} // namespace Snooker2D
