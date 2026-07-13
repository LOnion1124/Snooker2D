#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Snooker2D {

// Forward declarations
class GameUiBus;
class GameSessionViewModel;
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
    void initGame();

    // 通信总线（契约层）
    GameUiBus* m_uiBus = nullptr;

    // ViewModel 协调器
    GameSessionViewModel* m_sessionViewModel = nullptr;

    // View
    GameView* m_gameView = nullptr;
    CueControl* m_cueControl = nullptr;
    ScoreBoard* m_scoreBoard = nullptr;
    GameInfoPanel* m_gameInfoPanel = nullptr;
};

} // namespace Snooker2D

#endif // MAINWINDOW_H
