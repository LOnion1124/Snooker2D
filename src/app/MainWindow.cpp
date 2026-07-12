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

void MainWindow::setupBindings() {
    // --- CueControl → GameViewModel 桥接 ---
    // TODO: to be refined — 后续可考虑将 CueControlViewModel 合并进 GameViewModel
    connect(m_cueControlViewModel, &CueControlViewModel::shootRequested,
            m_gameViewModel, &GameViewModel::shoot);
    connect(m_cueControlViewModel, &CueControlViewModel::angleChanged,
            m_gameViewModel, &GameViewModel::setAngle);
    connect(m_cueControlViewModel, &CueControlViewModel::powerChanged,
            m_gameViewModel, &GameViewModel::setPower);

    // --- GameViewModel → GameInfoPanel ---
    // TODO: to be refined — 当前直接用 lambda 转发，后续可改用 QBind 或统一属性层
    connect(m_gameViewModel, &GameViewModel::currentPlayerChanged, this, [this]() {
        m_gameInfoPanel->setCurrentPlayer(m_gameViewModel->currentPlayer());
    });
    connect(m_gameViewModel, &GameViewModel::gamePhaseChanged, this, [this]() {
        m_gameInfoPanel->setPhase(m_gameViewModel->gamePhase());
    });
    connect(m_gameViewModel, &GameViewModel::foulOccurred, this, [this](const QString& desc) {
        m_gameInfoPanel->setMessage(desc);
    });

    // --- GameViewModel → ScoreViewModel 桥接 ---
    // TODO: to be refined — 双 ViewModel 同步比较脆弱, 后续可考虑直接让 ScoreBoard 绑定 GameViewModel
    connect(m_gameViewModel, &GameViewModel::player1ScoreChanged, this, [this]() {
        m_scoreViewModel->setPlayer1Score(m_gameViewModel->player1Score());
    });
    connect(m_gameViewModel, &GameViewModel::player2ScoreChanged, this, [this]() {
        m_scoreViewModel->setPlayer2Score(m_gameViewModel->player2Score());
    });

    // --- 初始状态同步（绑定前信号已发射，补一次同步） ---
    // TODO: to be refined — 后续可改用属性绑定的方式自动同步
    m_gameInfoPanel->setCurrentPlayer(m_gameViewModel->currentPlayer());
    m_gameInfoPanel->setPhase(m_gameViewModel->gamePhase());
    m_scoreViewModel->setPlayer1Score(m_gameViewModel->player1Score());
    m_scoreViewModel->setPlayer2Score(m_gameViewModel->player2Score());
}

void MainWindow::initGame() {
    // 创建核心对象
    // TODO: to be refined — 后续应改用依赖注入或工厂模式统一管理生命周期
    m_gameState = new GameState(this);
    m_gameViewModel = new GameViewModel(this);
    m_cueControlViewModel = new CueControlViewModel(this);
    m_scoreViewModel = new ScoreViewModel(this);

    // 注入 ViewModel → View
    m_gameView->setViewModel(m_gameViewModel);
    m_cueControl->setViewModel(m_cueControlViewModel);
    m_scoreBoard->setViewModel(m_scoreViewModel);

    // 连接 Model → ViewModel
    m_gameViewModel->setGameState(m_gameState);

    // 开局
    m_gameState->startNewGame();
}

} // namespace Snooker2D
