#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Forward declarations
class GameViewModel;
class GameView;
class CueControl;
class ScoreBoard;
class GameInfoPanel;

namespace Snooker2D {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void setupUI();
    void setupBindings();
    void initGame();

    // Model / ViewModel / View 持有
    // TODO: 添加 Model 层对象
    GameViewModel* m_gameViewModel = nullptr;
    GameView* m_gameView = nullptr;
    CueControl* m_cueControl = nullptr;
    ScoreBoard* m_scoreBoard = nullptr;
    GameInfoPanel* m_gameInfoPanel = nullptr;
};

} // namespace Snooker2D

#endif // MAINWINDOW_H
