#include "MainWindow.h"
#include "Constants.h"
#include "GameView.h"
#include "CueControl.h"
#include "ScoreBoard.h"
#include "GameInfoPanel.h"
#include "GameControlPanel.h"
#include "EnglishControlPanel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

namespace Snooker2D {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle(APP_NAME);
    resize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    setupUI();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QHBoxLayout(centralWidget);

    auto* leftLayout = new QVBoxLayout();
    m_gameView = new GameView(this);
    m_cueControl = new CueControl(this);
    leftLayout->addWidget(m_gameView);
    leftLayout->addWidget(m_cueControl);

    auto* rightLayout = new QVBoxLayout();
    m_scoreBoard = new ScoreBoard(this);
    m_gameInfoPanel = new GameInfoPanel(this);
    m_englishControlPanel = new EnglishControlPanel(this);
    m_gameControlPanel = new GameControlPanel(this);

    rightLayout->setContentsMargins(4, 0, 0, 0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(m_scoreBoard);
    rightLayout->addStretch(1);
    rightLayout->addWidget(m_gameInfoPanel);
    rightLayout->addStretch(1);
    rightLayout->addWidget(m_englishControlPanel);
    rightLayout->addStretch(1);
    rightLayout->addWidget(m_gameControlPanel);

    connect(m_gameControlPanel, &GameControlPanel::languageChanged,
            this, &MainWindow::setLanguage);

    mainLayout->addLayout(leftLayout, 3);
    mainLayout->addLayout(rightLayout, 1);
    setCentralWidget(centralWidget);
    setLanguage(m_language);
}

void MainWindow::setLanguage(UiLanguage language) {
    m_language = language;
    if (m_cueControl) {
        m_cueControl->setLanguage(language);
    }
    if (m_scoreBoard) {
        m_scoreBoard->setLanguage(language);
    }
    if (m_gameInfoPanel) {
        m_gameInfoPanel->setLanguage(language);
    }
    if (m_englishControlPanel) {
        m_englishControlPanel->setLanguage(language);
    }
    if (m_gameControlPanel) {
        m_gameControlPanel->setLanguage(language);
    }
}

} // namespace Snooker2D
