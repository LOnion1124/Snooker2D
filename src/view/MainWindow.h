#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Snooker2D {

// Forward declarations (contracts + view only, no viewmodel/model)
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

    // 由外部（main.cpp / App 层）注入 Bus 并完成绑定与启动
    void init(GameUiBus* bus);

private:
    void setupUI();

    // 通信总线（契约层）
    GameUiBus* m_uiBus = nullptr;

    // View 子控件
    GameView* m_gameView = nullptr;
    CueControl* m_cueControl = nullptr;
    ScoreBoard* m_scoreBoard = nullptr;
    GameInfoPanel* m_gameInfoPanel = nullptr;
};

} // namespace Snooker2D

#endif // MAINWINDOW_H
