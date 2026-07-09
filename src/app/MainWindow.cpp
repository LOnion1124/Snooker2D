#include "MainWindow.h"
#include "../app/AppConfig.h"

// ViewModel
#include "../viewmodel/GameViewModel.h"

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
    setupBindings();
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

void MainWindow::setupBindings() {
    // TODO: 绑定 ViewModel 属性到 View
    // TODO: 连接 View 控件信号到 ViewModel 命令
}

void MainWindow::initGame() {
    // TODO: 初始化 Model、ViewModel，开始一局新游戏
}

} // namespace Snooker2D
