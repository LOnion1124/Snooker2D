#pragma once

#include <QMainWindow>
#include "UiLanguage.h"

namespace Snooker2D {

class GameView;
class CueControl;
class ScoreBoard;
class GameInfoPanel;
class GameControlPanel;
class EnglishControlPanel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    GameView* gameView() const { return m_gameView; }
    CueControl* cueControl() const { return m_cueControl; }
    ScoreBoard* scoreBoard() const { return m_scoreBoard; }
    GameInfoPanel* gameInfoPanel() const { return m_gameInfoPanel; }
    EnglishControlPanel* englishControlPanel() const { return m_englishControlPanel; }
    GameControlPanel* gameControlPanel() const { return m_gameControlPanel; }

private:
    void setupUI();
    void setLanguage(UiLanguage language);

    GameView* m_gameView = nullptr;
    CueControl* m_cueControl = nullptr;
    ScoreBoard* m_scoreBoard = nullptr;
    GameInfoPanel* m_gameInfoPanel = nullptr;
    EnglishControlPanel* m_englishControlPanel = nullptr;
    GameControlPanel* m_gameControlPanel = nullptr;
    UiLanguage m_language = UiLanguage::Chinese;
};

} // namespace Snooker2D
