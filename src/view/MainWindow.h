#pragma once

#include <QMainWindow>

namespace Snooker2D {

class GameView;
class CueControl;
class ScoreBoard;
class GameInfoPanel;
class GameControlPanel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    GameView* gameView() const { return m_gameView; }
    CueControl* cueControl() const { return m_cueControl; }
    ScoreBoard* scoreBoard() const { return m_scoreBoard; }
    GameInfoPanel* gameInfoPanel() const { return m_gameInfoPanel; }
    GameControlPanel* gameControlPanel() const { return m_gameControlPanel; }

private:
    void setupUI();

    GameView* m_gameView = nullptr;
    CueControl* m_cueControl = nullptr;
    ScoreBoard* m_scoreBoard = nullptr;
    GameInfoPanel* m_gameInfoPanel = nullptr;
    GameControlPanel* m_gameControlPanel = nullptr;
};

} // namespace Snooker2D
