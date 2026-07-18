#include "App.h"
#include <QApplication>
#include <QIcon>
#include "MainWindow.h"
#include "GameSessionViewModel.h"
#include "GameView.h"
#include "CueControl.h"
#include "ScoreBoard.h"
#include "GameInfoPanel.h"
#include "GameControlPanel.h"
#include "EnglishControlPanel.h"

namespace Snooker2D {

int App::run(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon("assets/favicon.ico"));

    GameSessionViewModel sessionViewModel;
    MainWindow mainWindow;

    GameView* gameView = mainWindow.gameView();
    CueControl* cueControl = mainWindow.cueControl();
    ScoreBoard* scoreBoard = mainWindow.scoreBoard();
    GameInfoPanel* gameInfoPanel = mainWindow.gameInfoPanel();
    EnglishControlPanel* englishControlPanel = mainWindow.englishControlPanel();
    GameControlPanel* gameControlPanel = mainWindow.gameControlPanel();

    // ViewModel → View（属性绑定）

    QObject::connect(&sessionViewModel, &GameSessionViewModel::tableStateReady,
            gameView, &GameView::applyTableState);
    QObject::connect(&sessionViewModel, &GameSessionViewModel::cueStateReady,
            cueControl, &CueControl::applyCueState);
    QObject::connect(&sessionViewModel, &GameSessionViewModel::cueStateReady,
            englishControlPanel, &EnglishControlPanel::applyCueState);
    QObject::connect(&sessionViewModel, &GameSessionViewModel::scoreStateReady,
            scoreBoard, &ScoreBoard::applyScoreState);
    QObject::connect(&sessionViewModel, &GameSessionViewModel::gameInfoStateReady,
            gameInfoPanel, &GameInfoPanel::applyGameInfoState);
    QObject::connect(&sessionViewModel, &GameSessionViewModel::shotAnimationCancelled,
            gameView, &GameView::cancelShotAnimation);

    // View → ViewModel（命令绑定）

    QObject::connect(gameView, &GameView::angleChanged,
            &sessionViewModel, &GameSessionViewModel::setAngle);
    QObject::connect(gameView, &GameView::powerChanged,
            &sessionViewModel, &GameSessionViewModel::setPower);
    QObject::connect(gameView, &GameView::shotAnimationFinished,
            &sessionViewModel, &GameSessionViewModel::onShotAnimationFinished);
    QObject::connect(gameView, &GameView::whiteBallPlacementRequested,
            &sessionViewModel, &GameSessionViewModel::placeWhiteBall);
    QObject::connect(gameControlPanel, &GameControlPanel::restartRequested,
            &sessionViewModel, &GameSessionViewModel::restart);
    QObject::connect(englishControlPanel, &EnglishControlPanel::englishChanged,
            &sessionViewModel, &GameSessionViewModel::setEnglish);
    QObject::connect(gameControlPanel, &GameControlPanel::aimingGuideVisibilityChanged,
            gameView, &GameView::setAimingGuideEnabled);

    // 启动

    sessionViewModel.start();
    mainWindow.show();
    return app.exec();
}

} // namespace Snooker2D
