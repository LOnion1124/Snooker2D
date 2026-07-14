#include "App.h"
#include <QApplication>
#include "MainWindow.h"
#include "GameSessionViewModel.h"
#include "GameView.h"
#include "CueControl.h"
#include "ScoreBoard.h"
#include "GameInfoPanel.h"

namespace Snooker2D {

int App::run(int argc, char* argv[])
{
    QApplication app(argc, argv);

    GameSessionViewModel sessionViewModel;
    MainWindow mainWindow;

    GameView* gameView = mainWindow.gameView();
    CueControl* cueControl = mainWindow.cueControl();
    ScoreBoard* scoreBoard = mainWindow.scoreBoard();
    GameInfoPanel* gameInfoPanel = mainWindow.gameInfoPanel();

    // ViewModel → View（属性绑定）

    QObject::connect(&sessionViewModel, &GameSessionViewModel::tableStateReady,
            gameView, &GameView::applyTableState);
    QObject::connect(&sessionViewModel, &GameSessionViewModel::cueStateReady,
            cueControl, &CueControl::applyCueState);
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
    QObject::connect(gameInfoPanel, &GameInfoPanel::restartRequested,
            &sessionViewModel, &GameSessionViewModel::restart);

    // 启动

    sessionViewModel.start();
    mainWindow.show();
    return app.exec();
}

} // namespace Snooker2D
