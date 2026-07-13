#include "MainWindow.h"
#include "../app/AppConfig.h"

// Contracts
#include "contracts/GameUiBus.h"
#include "contracts/ContractsInit.h"

// ViewModel
#include "../viewmodel/GameSessionViewModel.h"

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

    // 注册 contracts 自定义类型到 Qt 元对象系统
    registerContractTypes();

    setupUI();
    initGame();
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
    // 创建 ViewModel 协调器（内部创建 Bus、Model、子 ViewModel）
    m_sessionViewModel = new GameSessionViewModel(this);
    m_uiBus = m_sessionViewModel->bus();

    // View 统一绑定 Bus（不感知具体 ViewModel）
    m_gameView->bind(m_uiBus);
    m_cueControl->bind(m_uiBus);
    m_scoreBoard->bind(m_uiBus);
    m_gameInfoPanel->bind(m_uiBus);

    // 启动游戏
    m_sessionViewModel->start();
}

} // namespace Snooker2D
