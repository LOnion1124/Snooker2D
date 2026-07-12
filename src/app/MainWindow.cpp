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
    m_gameInfoPanel->setViewModel(m_gameViewModel);

    // 开始游戏
    m_gameState->startNewGame();
}

void MainWindow::setupBindings() {
    // 球桌左键单击先播放球杆前移动画，动画结束后再进入真实物理模拟。
    connect(m_gameView, &GameView::shotAnimationFinished,
            this, [this]() {
        m_gameViewModel->shoot();
    });

    // 角度/力度实时同步 → GameViewModel（供瞄准线等用）
    connect(m_cueViewModel, &CueControlViewModel::angleChanged,
            m_gameViewModel, &GameViewModel::setAngle);
    connect(m_cueViewModel, &CueControlViewModel::powerChanged,
            m_gameViewModel, &GameViewModel::setPower);

    // 球桌鼠标移动/滚轮调整角度和力度后，同步回击球控制面板。
    connect(m_gameViewModel, &GameViewModel::cueAngleChanged,
            this, [this]() {
        m_cueViewModel->setAngle(m_gameViewModel->cueAngle());
    });
    connect(m_gameViewModel, &GameViewModel::cuePowerChanged,
            this, [this]() {
        m_cueViewModel->setPower(m_gameViewModel->cuePower());
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
    connect(m_gameViewModel, &GameViewModel::gameRestarted,
            this, [this]() {
        m_scoreViewModel->setFoulMessage(QString());
        m_scoreViewModel->setStatusMessage(QString());
    });

    // 犯规提示
    connect(m_gameViewModel, &GameViewModel::foulOccurred,
            this, [this](const QString& desc) {
        m_scoreViewModel->setFoulMessage(desc);
    });

    // 比赛结束
    connect(m_gameViewModel, &GameViewModel::gameOver,
            this, [this](int winner) {
        QString msg = QString("玩家 %1 获胜！").arg(winner);
        m_scoreViewModel->setStatusMessage(msg);
    });
}

} // namespace Snooker2D
