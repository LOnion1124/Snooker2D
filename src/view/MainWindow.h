#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Snooker2D {

class GameUiBus;
class GameView;
class CueControl;
class ScoreBoard;
class GameInfoPanel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    // 由 App 层传入 Bus，绑定所有子 View
    void bindAll(GameUiBus* bus);

private:
    void setupUI();

    // View 子控件
    GameView* m_gameView = nullptr;
    CueControl* m_cueControl = nullptr;
    ScoreBoard* m_scoreBoard = nullptr;
    GameInfoPanel* m_gameInfoPanel = nullptr;
};

} // namespace Snooker2D

#endif // MAINWINDOW_H
