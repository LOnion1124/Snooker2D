#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Snooker2D {

// Forward declarations
class GameState;
class GameViewModel;
class CueControlViewModel;
class ScoreViewModel;
class GameView;
class CueControl;
class ScoreBoard;
class GameInfoPanel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void setupUI();
    void setupBindings();
    void initGame();

    // Model
    GameState* m_gameState = nullptr;

    // ViewModel
    GameViewModel* m_gameViewModel = nullptr;
    CueControlViewModel* m_cueControlViewModel = nullptr;
    ScoreViewModel* m_scoreViewModel = nullptr;

    // View
    GameView* m_gameView = nullptr;
    CueControl* m_cueControl = nullptr;
    ScoreBoard* m_scoreBoard = nullptr;
    GameInfoPanel* m_gameInfoPanel = nullptr;
};

} // namespace Snooker2D

#endif // MAINWINDOW_H
